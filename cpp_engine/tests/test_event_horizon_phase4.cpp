#include "qau/backreaction.hpp"

#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
constexpr double kTolerance = 1e-10;

void require_true(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

void require_close(double actual, double expected, double tolerance, const char* message) {
    if (std::abs(actual - expected) > tolerance) {
        throw std::runtime_error(std::string(message) + ": expected " + std::to_string(expected) +
                                 ", got " + std::to_string(actual));
    }
}

void test_flat_geometry_baseline() {
    std::cout << "[1/5] Flat triangulated-geometry curvature baseline...\n";
    const auto geometry = qau::DiscreteGeometry::triangular_grid(7, 7);
    require_true(geometry.metric_is_valid(), "Initial metric must be valid.");
    require_close(geometry.mean_abs_interior_curvature(), 0.0, 1e-9,
                  "Regular planar triangulation must have zero interior angle-deficit curvature");
    require_close(geometry.maximum_abs_interior_curvature(), 0.0, 1e-9,
                  "Regular planar triangulation must have zero maximum interior curvature");
    const double transport_curvature = geometry.mean_ollivier_ricci();
    require_true(std::isfinite(transport_curvature), "Ollivier-Ricci diagnostic must be finite.");
}

void test_source_backreaction_and_stability() {
    std::cout << "[2/5] Localized matter source and bounded backreaction...\n";
    auto geometry = qau::DiscreteGeometry::triangular_grid(7, 7);
    geometry.set_radial_gaussian_source(3.0, 3.0, 2.0, 0.75);

    const double initial_energy = geometry.total_energy();
    const double initial_average_length = [&]() {
        double sum = 0.0;
        for (const auto& edge : geometry.edges()) sum += edge.length;
        return sum / geometry.edges().size();
    }();

    const auto trajectory = geometry.evolve(8, 0.35, 0.25, 0.55);
    require_true(trajectory.size() == 9, "Trajectory must include the initial state and all updates.");
    require_true(geometry.metric_is_valid(), "Backreaction must preserve a valid metric.");
    require_true(trajectory.back().average_edge_length < initial_average_length,
                 "Positive source must reduce the effective average edge scale under this toy coupling.");
    require_true(trajectory.back().mean_abs_interior_curvature > 1e-5,
                 "Localized source must create a measurable non-flat curvature response.");
    require_true(std::isfinite(trajectory.back().mean_ollivier_ricci),
                 "Transport curvature must remain finite after backreaction.");

    for (const auto& point : trajectory) {
        require_close(point.total_energy, initial_energy, kTolerance,
                      "Geometry evolution must not mutate the source field");
        require_true(point.metric_valid, "Every evolution step must keep a valid discrete metric.");
        require_true(std::isfinite(point.average_edge_length) && point.average_edge_length > 0.0,
                     "Every evolution step must retain positive finite edge scale.");
    }

    const int center_id = 3 * 7 + 3;
    double central_edge_mean = 0.0;
    int central_edge_count = 0;
    for (const auto& edge : geometry.edges()) {
        if (edge.a == center_id || edge.b == center_id) {
            central_edge_mean += edge.length / edge.baseline_length;
            ++central_edge_count;
        }
    }
    central_edge_mean /= central_edge_count;
    require_true(central_edge_mean < 0.90,
                 "The source-adjacent effective geometry must respond more than numerical noise.");
}

void test_deterministic_source_trajectory() {
    std::cout << "[3/5] Deterministic geometry trajectory...\n";
    auto left = qau::DiscreteGeometry::triangular_grid(6, 6);
    auto right = qau::DiscreteGeometry::triangular_grid(6, 6);
    left.set_radial_gaussian_source(2.5, 2.5, 1.25, 0.8);
    right.set_radial_gaussian_source(2.5, 2.5, 1.25, 0.8);

    const auto left_trajectory = left.evolve(6, 0.2, 0.2, 0.6);
    const auto right_trajectory = right.evolve(6, 0.2, 0.2, 0.6);
    assert(left_trajectory.size() == right_trajectory.size());
    for (std::size_t i = 0; i < left_trajectory.size(); ++i) {
        require_close(left_trajectory[i].average_edge_length, right_trajectory[i].average_edge_length,
                      kTolerance, "Deterministic average-edge trajectory");
        require_close(left_trajectory[i].mean_abs_interior_curvature,
                      right_trajectory[i].mean_abs_interior_curvature, kTolerance,
                      "Deterministic curvature trajectory");
        require_close(left_trajectory[i].mean_ollivier_ricci, right_trajectory[i].mean_ollivier_ricci,
                      kTolerance, "Deterministic transport-curvature trajectory");
    }
    for (std::size_t i = 0; i < left.edges().size(); ++i) {
        require_close(left.edges()[i].length, right.edges()[i].length, kTolerance,
                      "Deterministic final edge geometry");
    }
}

void test_zero_source_fixed_point() {
    std::cout << "[4/5] Zero-source fixed-point evolution...\n";
    auto geometry = qau::DiscreteGeometry::triangular_grid(5, 5);
    const std::vector<double> initial_lengths = [&]() {
        std::vector<double> values;
        for (const auto& edge : geometry.edges()) values.push_back(edge.length);
        return values;
    }();
    const auto trajectory = geometry.evolve(5, 0.5, 0.4, 0.5);
    require_close(trajectory.back().mean_abs_interior_curvature, 0.0, 1e-9,
                  "Zero source must retain the flat curvature baseline");
    for (std::size_t i = 0; i < geometry.edges().size(); ++i) {
        require_close(geometry.edges()[i].length, initial_lengths[i], kTolerance,
                      "Zero source must retain baseline edge lengths");
    }
}

void test_parameter_validation() {
    std::cout << "[5/5] Parameter and metric safety checks...\n";
    auto geometry = qau::DiscreteGeometry::triangular_grid(5, 5);
    bool threw = false;
    try {
        geometry.backreact_step(-0.1);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require_true(threw, "Negative coupling must be rejected.");

    threw = false;
    try {
        geometry.set_energy_density(0, -1.0);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require_true(threw, "Negative source density must be rejected.");
}
} // namespace

int main() {
    std::cout << "=== Project Event Horizon — Phase IV Validation Harness ===\n";
    std::cout << "Model status: a discrete spatial-geometry backreaction toy model, not full general relativity.\n";
    try {
        test_flat_geometry_baseline();
        test_source_backreaction_and_stability();
        test_deterministic_source_trajectory();
        test_zero_source_fixed_point();
        test_parameter_validation();
        std::cout << "=== Phase IV harness: ALL TESTS GREEN ===\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Phase IV harness failure: " << error.what() << "\n";
        return 1;
    }
}
