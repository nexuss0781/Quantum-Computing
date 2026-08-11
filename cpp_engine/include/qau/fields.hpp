#ifndef QAU_FIELDS_HPP
#define QAU_FIELDS_HPP

#include "qvs.hpp"
#include <vector>
#include <cmath>

namespace qau {

/**
 * Quantum Lattice for Field Theory simulations.
 * Represents a lattice of ASCs linked by local gauge constraints (NCB).
 */
class QuantumLattice {
public:
    int width, height;
    std::vector<int> site_ids;

    QuantumLattice(QVS& qvs, int w, int h) : width(w), height(h) {
        for (int i = 0; i < w * h; ++i) {
            site_ids.push_back(qvs.create_asc(1));
        }
    }

    /**
     * Apply a local field operator (e.g., creation/annihilation approximation)
     * via phase weaving and superposition.
     */
    void apply_field_operator(QVS& qvs, int x, int y, double intensity) {
        int id = site_ids[y * width + x];
        qvs.WEAVE(id, nullptr, intensity);
    }

    /**
     * Forge gauge constraints (NCB) between adjacent sites.
     */
    void establish_gauge_symmetry(QVS& qvs) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width - 1; ++x) {
                qvs.BOND(site_ids[y * width + x], site_ids[y * width + x + 1], "bell");
            }
        }
    }
};

} // namespace qau

#endif // QAU_FIELDS_HPP
