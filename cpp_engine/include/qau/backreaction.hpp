#ifndef QAU_BACKREACTION_HPP
#define QAU_BACKREACTION_HPP

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <deque>
#include <functional>
#include <limits>
#include <map>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

namespace qau {

struct GeometryVertex {
    int id;
    double x;
    double y;
    double energy_density = 0.0;
    bool boundary = false;
};

struct GeometryEdge {
    int id;
    int a;
    int b;
    double baseline_length;
    double length;
};

struct GeometryTriangle {
    int a;
    int b;
    int c;
};

struct VertexCurvature {
    double angle_deficit;
    double dual_area;
    double curvature_density;
};

struct GeometryDiagnostics {
    int step;
    double total_energy;
    double average_edge_length;
    double mean_abs_interior_curvature;
    double maximum_abs_interior_curvature;
    double mean_ollivier_ricci;
    double maximum_edge_delta;
    bool metric_valid;
};

/**
 * A discrete two-dimensional spatial-slice toy model.
 *
 * The object is a weighted triangulated graph. Triangle angle deficits provide
 * a Regge-inspired intrinsic-curvature diagnostic, while a transport-based
 * Ollivier-Ricci observable provides an independent graph-curvature diagnostic.
 * It is not a Lorentzian spacetime solver and does not implement full Einstein
 * equations. Its purpose is controlled, reproducible matter-to-geometry
 * experiments on small discrete geometries.
 */
class DiscreteGeometry {
public:
    static DiscreteGeometry triangular_grid(int width, int height, double spacing = 1.0) {
        if (width < 3 || height < 3 || spacing <= 0.0) {
            throw std::invalid_argument("Triangular grid requires width/height >= 3 and positive spacing.");
        }
        DiscreteGeometry geometry;
        geometry.width_ = width;
        geometry.height_ = height;

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const int id = y * width + x;
                geometry.vertices_.push_back({
                    id,
                    x * spacing,
                    y * spacing,
                    0.0,
                    x == 0 || y == 0 || x == width - 1 || y == height - 1
                });
            }
        }

        for (int y = 0; y < height - 1; ++y) {
            for (int x = 0; x < width - 1; ++x) {
                const int a = y * width + x;
                const int b = y * width + (x + 1);
                const int c = (y + 1) * width + x;
                const int d = (y + 1) * width + (x + 1);
                geometry.add_triangle(a, b, d);
                geometry.add_triangle(a, d, c);
            }
        }
        geometry.rebuild_incidence();
        return geometry;
    }

    const std::vector<GeometryVertex>& vertices() const noexcept { return vertices_; }
    const std::vector<GeometryEdge>& edges() const noexcept { return edges_; }
    const std::vector<GeometryTriangle>& triangles() const noexcept { return triangles_; }
    int step() const noexcept { return step_; }

    void set_energy_density(int vertex_id, double density) {
        validate_vertex(vertex_id);
        if (!std::isfinite(density) || density < 0.0) {
            throw std::invalid_argument("Energy density must be finite and nonnegative.");
        }
        vertices_[vertex_id].energy_density = density;
    }

    void clear_energy_density() {
        for (auto& vertex : vertices_) vertex.energy_density = 0.0;
    }

    void set_radial_gaussian_source(double center_x, double center_y, double amplitude, double sigma) {
        if (!std::isfinite(amplitude) || amplitude < 0.0 || !std::isfinite(sigma) || sigma <= 0.0) {
            throw std::invalid_argument("Gaussian source amplitude must be nonnegative and sigma positive.");
        }
        for (auto& vertex : vertices_) {
            const double dx = vertex.x - center_x;
            const double dy = vertex.y - center_y;
            vertex.energy_density = amplitude * std::exp(-(dx * dx + dy * dy) / (2.0 * sigma * sigma));
        }
    }

    double total_energy() const {
        double total = 0.0;
        for (const auto& vertex : vertices_) total += vertex.energy_density;
        return total;
    }

    std::vector<VertexCurvature> vertex_curvature() const {
        std::vector<double> angle_sums(vertices_.size(), 0.0);
        std::vector<double> dual_areas(vertices_.size(), 0.0);

        for (const auto& triangle : triangles_) {
            const auto lengths = triangle_lengths(triangle);
            const double area = triangle_area(lengths[0], lengths[1], lengths[2]);
            const double angle_a = angle_from_lengths(lengths[1], lengths[2], lengths[0]);
            const double angle_b = angle_from_lengths(lengths[0], lengths[2], lengths[1]);
            const double angle_c = angle_from_lengths(lengths[0], lengths[1], lengths[2]);
            angle_sums[triangle.a] += angle_a;
            angle_sums[triangle.b] += angle_b;
            angle_sums[triangle.c] += angle_c;
            dual_areas[triangle.a] += area / 3.0;
            dual_areas[triangle.b] += area / 3.0;
            dual_areas[triangle.c] += area / 3.0;
        }

        std::vector<VertexCurvature> result(vertices_.size());
        for (std::size_t i = 0; i < vertices_.size(); ++i) {
            // Boundary curvature is intentionally excluded: the grid has an open
            // boundary, so no boundary-geodesic-curvature prescription is implied.
            const double deficit = vertices_[i].boundary ? 0.0 : (2.0 * M_PI - angle_sums[i]);
            const double area = dual_areas[i];
            result[i] = {deficit, area, area > 1e-14 ? deficit / area : 0.0};
        }
        return result;
    }

    double mean_abs_interior_curvature() const {
        const auto curvatures = vertex_curvature();
        double sum = 0.0;
        int count = 0;
        for (std::size_t i = 0; i < curvatures.size(); ++i) {
            if (!vertices_[i].boundary) {
                sum += std::abs(curvatures[i].curvature_density);
                ++count;
            }
        }
        return count == 0 ? 0.0 : sum / count;
    }

    double maximum_abs_interior_curvature() const {
        const auto curvatures = vertex_curvature();
        double maximum = 0.0;
        for (std::size_t i = 0; i < curvatures.size(); ++i) {
            if (!vertices_[i].boundary) maximum = std::max(maximum, std::abs(curvatures[i].curvature_density));
        }
        return maximum;
    }

    /** Exact finite-support Ollivier-Ricci curvature on a selected weighted edge. */
    double ollivier_ricci_edge(int edge_id, double idleness = 0.5) const {
        if (edge_id < 0 || edge_id >= static_cast<int>(edges_.size()) || idleness < 0.0 || idleness > 1.0) {
            throw std::invalid_argument("Invalid edge id or idleness.");
        }
        const auto& edge = edges_[edge_id];
        const auto source_measure = lazy_measure(edge.a, idleness);
        const auto target_measure = lazy_measure(edge.b, idleness);

        std::vector<std::vector<double>> costs(source_measure.size(), std::vector<double>(target_measure.size(), 0.0));
        for (std::size_t i = 0; i < source_measure.size(); ++i) {
            const auto distances = shortest_paths(source_measure[i].first);
            for (std::size_t j = 0; j < target_measure.size(); ++j) {
                costs[i][j] = distances[target_measure[j].first];
            }
        }
        const double wasserstein = transport_cost(source_measure, target_measure, costs);
        return 1.0 - wasserstein / edge.length;
    }

    double mean_ollivier_ricci(double idleness = 0.5) const {
        if (edges_.empty()) return 0.0;
        double sum = 0.0;
        for (const auto& edge : edges_) sum += ollivier_ricci_edge(edge.id, idleness);
        return sum / static_cast<double>(edges_.size());
    }

    /**
     * Controlled source-to-geometry coupling.
     *
     * Positive energy shortens adjacent effective edge lengths by a bounded,
     * relaxed amount. A metric repair pass maintains strict triangle inequalities.
     * This is a phenomenological backreaction rule, not an Einstein-equation
     * discretization.
     */
    GeometryDiagnostics backreact_step(double coupling, double relaxation = 0.15, double min_length_ratio = 0.60) {
        if (!std::isfinite(coupling) || coupling < 0.0 || relaxation <= 0.0 || relaxation > 1.0 ||
            min_length_ratio <= 0.0 || min_length_ratio >= 1.0) {
            throw std::invalid_argument("Invalid backreaction parameters.");
        }

        double maximum_delta = 0.0;
        for (auto& edge : edges_) {
            const double source = 0.5 * (vertices_[edge.a].energy_density + vertices_[edge.b].energy_density);
            const double target = edge.baseline_length * std::max(min_length_ratio, std::exp(-coupling * source));
            const double updated = (1.0 - relaxation) * edge.length + relaxation * target;
            maximum_delta = std::max(maximum_delta, std::abs(updated - edge.length));
            edge.length = updated;
        }
        repair_triangle_metrics();
        ++step_;
        return diagnostics(maximum_delta);
    }

    std::vector<GeometryDiagnostics> evolve(
        int steps, double coupling, double relaxation = 0.15, double min_length_ratio = 0.60) {
        if (steps < 0) throw std::invalid_argument("Evolution steps cannot be negative.");
        std::vector<GeometryDiagnostics> trajectory;
        trajectory.reserve(static_cast<std::size_t>(steps) + 1);
        trajectory.push_back(diagnostics(0.0));
        for (int i = 0; i < steps; ++i) trajectory.push_back(backreact_step(coupling, relaxation, min_length_ratio));
        return trajectory;
    }

    bool metric_is_valid(double tolerance = 1e-12) const {
        for (const auto& edge : edges_) {
            if (!std::isfinite(edge.length) || edge.length <= tolerance) return false;
        }
        for (const auto& triangle : triangles_) {
            const auto lengths = triangle_lengths(triangle);
            if (lengths[0] + lengths[1] <= lengths[2] + tolerance ||
                lengths[0] + lengths[2] <= lengths[1] + tolerance ||
                lengths[1] + lengths[2] <= lengths[0] + tolerance) {
                return false;
            }
        }
        return true;
    }

private:
    int width_ = 0;
    int height_ = 0;
    int step_ = 0;
    std::vector<GeometryVertex> vertices_;
    std::vector<GeometryEdge> edges_;
    std::vector<GeometryTriangle> triangles_;
    std::map<std::pair<int, int>, int> edge_ids_;
    std::vector<std::vector<int>> incident_edges_;

    void validate_vertex(int vertex_id) const {
        if (vertex_id < 0 || vertex_id >= static_cast<int>(vertices_.size())) {
            throw std::out_of_range("Vertex index is out of range.");
        }
    }

    int edge_id(int a, int b) const {
        if (a > b) std::swap(a, b);
        const auto iterator = edge_ids_.find({a, b});
        if (iterator == edge_ids_.end()) throw std::logic_error("Triangle references a missing edge.");
        return iterator->second;
    }

    void add_edge(int a, int b) {
        if (a > b) std::swap(a, b);
        if (edge_ids_.contains({a, b})) return;
        const double dx = vertices_[a].x - vertices_[b].x;
        const double dy = vertices_[a].y - vertices_[b].y;
        const double length = std::sqrt(dx * dx + dy * dy);
        const int id = static_cast<int>(edges_.size());
        edges_.push_back({id, a, b, length, length});
        edge_ids_[{a, b}] = id;
    }

    void add_triangle(int a, int b, int c) {
        add_edge(a, b);
        add_edge(b, c);
        add_edge(c, a);
        triangles_.push_back({a, b, c});
    }

    void rebuild_incidence() {
        incident_edges_.assign(vertices_.size(), {});
        for (const auto& edge : edges_) {
            incident_edges_[edge.a].push_back(edge.id);
            incident_edges_[edge.b].push_back(edge.id);
        }
    }

    std::array<double, 3> triangle_lengths(const GeometryTriangle& triangle) const {
        return {
            edges_[edge_id(triangle.a, triangle.b)].length,
            edges_[edge_id(triangle.b, triangle.c)].length,
            edges_[edge_id(triangle.c, triangle.a)].length
        };
    }

    static double triangle_area(double ab, double bc, double ca) {
        const double semiperimeter = 0.5 * (ab + bc + ca);
        const double radicand = std::max(0.0, semiperimeter * (semiperimeter - ab) *
                                              (semiperimeter - bc) * (semiperimeter - ca));
        return std::sqrt(radicand);
    }

    static double angle_from_lengths(double adjacent_1, double adjacent_2, double opposite) {
        const double denominator = 2.0 * adjacent_1 * adjacent_2;
        if (denominator <= 1e-14) throw std::runtime_error("Degenerate triangle metric.");
        const double cosine = std::clamp((adjacent_1 * adjacent_1 + adjacent_2 * adjacent_2 - opposite * opposite) /
                                         denominator, -1.0, 1.0);
        return std::acos(cosine);
    }

    std::vector<std::pair<int, double>> lazy_measure(int vertex_id, double idleness) const {
        const auto& incident = incident_edges_.at(vertex_id);
        if (incident.empty()) throw std::runtime_error("Graph contains an isolated vertex.");
        std::vector<std::pair<int, double>> result;
        result.push_back({vertex_id, idleness});
        const double neighbor_mass = (1.0 - idleness) / static_cast<double>(incident.size());
        for (int id : incident) {
            const auto& edge = edges_[id];
            result.push_back({edge.a == vertex_id ? edge.b : edge.a, neighbor_mass});
        }
        return result;
    }

    std::vector<double> shortest_paths(int source) const {
        constexpr double infinity = std::numeric_limits<double>::infinity();
        std::vector<double> distances(vertices_.size(), infinity);
        using QueueItem = std::pair<double, int>;
        std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<QueueItem>> queue;
        distances[source] = 0.0;
        queue.push({0.0, source});
        while (!queue.empty()) {
            const auto [distance, vertex] = queue.top();
            queue.pop();
            if (distance > distances[vertex] + 1e-14) continue;
            for (int id : incident_edges_[vertex]) {
                const auto& edge = edges_[id];
                const int neighbor = edge.a == vertex ? edge.b : edge.a;
                const double candidate = distance + edge.length;
                if (candidate + 1e-14 < distances[neighbor]) {
                    distances[neighbor] = candidate;
                    queue.push({candidate, neighbor});
                }
            }
        }
        return distances;
    }

    static double transport_cost(
        const std::vector<std::pair<int, double>>& supply,
        const std::vector<std::pair<int, double>>& demand,
        const std::vector<std::vector<double>>& costs) {
        // Exact minimum-cost flow for the small complete transport network that
        // defines each edge-local Wasserstein problem in this toy geometry.
        struct FlowEdge {
            int to;
            int reverse;
            double capacity;
            double cost;
        };
        const int supply_count = static_cast<int>(supply.size());
        const int demand_count = static_cast<int>(demand.size());
        const int source = supply_count + demand_count;
        const int sink = source + 1;
        std::vector<std::vector<FlowEdge>> graph(sink + 1);
        auto add_edge = [&](int from, int to, double capacity, double cost) {
            graph[from].push_back({to, static_cast<int>(graph[to].size()), capacity, cost});
            graph[to].push_back({from, static_cast<int>(graph[from].size()) - 1, 0.0, -cost});
        };

        double required_flow = 0.0;
        for (int i = 0; i < supply_count; ++i) {
            add_edge(source, i, supply[i].second, 0.0);
            required_flow += supply[i].second;
        }
        for (int j = 0; j < demand_count; ++j) {
            add_edge(supply_count + j, sink, demand[j].second, 0.0);
        }
        for (int i = 0; i < supply_count; ++i) {
            for (int j = 0; j < demand_count; ++j) {
                add_edge(i, supply_count + j, required_flow, costs[i][j]);
            }
        }

        constexpr double epsilon = 1e-13;
        double delivered_flow = 0.0;
        double total_cost = 0.0;
        while (delivered_flow + epsilon < required_flow) {
            std::vector<double> distance(graph.size(), std::numeric_limits<double>::infinity());
            std::vector<int> previous_vertex(graph.size(), -1);
            std::vector<int> previous_edge(graph.size(), -1);
            std::vector<bool> in_queue(graph.size(), false);
            std::deque<int> queue;
            distance[source] = 0.0;
            queue.push_back(source);
            in_queue[source] = true;

            while (!queue.empty()) {
                const int vertex = queue.front();
                queue.pop_front();
                in_queue[vertex] = false;
                for (std::size_t edge_index = 0; edge_index < graph[vertex].size(); ++edge_index) {
                    const auto& edge = graph[vertex][edge_index];
                    if (edge.capacity <= epsilon) continue;
                    const double candidate = distance[vertex] + edge.cost;
                    if (candidate + epsilon < distance[edge.to]) {
                        distance[edge.to] = candidate;
                        previous_vertex[edge.to] = vertex;
                        previous_edge[edge.to] = static_cast<int>(edge_index);
                        if (!in_queue[edge.to]) {
                            queue.push_back(edge.to);
                            in_queue[edge.to] = true;
                        }
                    }
                }
            }
            if (!std::isfinite(distance[sink])) {
                throw std::runtime_error("Unable to complete transport plan.");
            }

            double augmentation = required_flow - delivered_flow;
            for (int vertex = sink; vertex != source; vertex = previous_vertex[vertex]) {
                augmentation = std::min(augmentation, graph[previous_vertex[vertex]][previous_edge[vertex]].capacity);
            }
            for (int vertex = sink; vertex != source; vertex = previous_vertex[vertex]) {
                auto& edge = graph[previous_vertex[vertex]][previous_edge[vertex]];
                edge.capacity -= augmentation;
                graph[vertex][edge.reverse].capacity += augmentation;
            }
            delivered_flow += augmentation;
            total_cost += augmentation * distance[sink];
        }
        return total_cost;
    }

    void repair_triangle_metrics() {
        constexpr double safety = 0.999999;
        // Multiple passes handle shared edges while retaining a positive metric.
        for (int pass = 0; pass < 8; ++pass) {
            bool changed = false;
            for (const auto& triangle : triangles_) {
                const int ab_id = edge_id(triangle.a, triangle.b);
                const int bc_id = edge_id(triangle.b, triangle.c);
                const int ca_id = edge_id(triangle.c, triangle.a);
                std::array<int, 3> ids{ab_id, bc_id, ca_id};
                std::sort(ids.begin(), ids.end(), [&](int left, int right) {
                    return edges_[left].length < edges_[right].length;
                });
                const double allowable = safety * (edges_[ids[0]].length + edges_[ids[1]].length);
                if (edges_[ids[2]].length >= allowable) {
                    edges_[ids[2]].length = allowable;
                    changed = true;
                }
            }
            if (!changed) return;
        }
        if (!metric_is_valid()) throw std::runtime_error("Unable to repair the discrete metric.");
    }

    GeometryDiagnostics diagnostics(double maximum_edge_delta) const {
        double average_length = 0.0;
        for (const auto& edge : edges_) average_length += edge.length;
        average_length /= static_cast<double>(edges_.size());
        return {
            step_,
            total_energy(),
            average_length,
            mean_abs_interior_curvature(),
            maximum_abs_interior_curvature(),
            mean_ollivier_ricci(),
            maximum_edge_delta,
            metric_is_valid()
        };
    }
};

} // namespace qau

#endif // QAU_BACKREACTION_HPP
