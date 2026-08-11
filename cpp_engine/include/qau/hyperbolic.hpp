#ifndef QAU_HYPERBOLIC_HPP
#define QAU_HYPERBOLIC_HPP

#include <vector>
#include <complex>
#include <cmath>
#include <Eigen/Dense>

namespace qau {

/**
 * Poincaré Disk Hyperbolic Geometry utility.
 * Used for discretizing bulk AdS space for tensor network mapping.
 */
class HyperbolicGeometry {
public:
    using Point = std::complex<double>;

    /**
     * Compute the hyperbolic distance between two points in the Poincaré disk.
     */
    static double distance(Point z1, Point z2) {
        double num = std::abs(z1 - z2);
        double den = std::abs(1.0 - z1 * std::conj(z2));
        double ratio = num / den;
        // Avoid domain errors at the boundary
        if (ratio >= 1.0) ratio = 0.999999;
        return 2.0 * std::atanh(ratio);
    }

    /**
     * Generate a regular tiling of the Poincaré disk (e.g., {p, q} tiling).
     * Simplified: Generate layers of points expanding towards the boundary.
     */
    static std::vector<Point> generate_discretization(int layers, int points_per_layer) {
        std::vector<Point> points;
        points.push_back({0.0, 0.0}); // Center point

        for (int l = 1; l <= layers; ++l) {
            double r = (double)l / (layers + 1.0); // Simple radial distribution
            for (int i = 0; i < points_per_layer * l; ++i) {
                double theta = 2.0 * M_PI * i / (points_per_layer * l);
                points.push_back(std::polar(r, theta));
            }
        }
        return points;
    }
};

/**
 * Bulk Node in the AdS space.
 */
struct BulkNode {
    int id;
    HyperbolicGeometry::Point coord;
    std::vector<int> neighbors;
};

} // namespace qau

#endif // QAU_HYPERBOLIC_HPP
