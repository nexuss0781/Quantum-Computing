#ifndef QAU_RPW_HPP
#define QAU_RPW_HPP

#include "asc.hpp"
#include <complex>
#include <cmath>

namespace qau {

class RPW {
public:
    double angle;

    explicit RPW(double angle = 0.0) : angle(angle) {}

    static ASC& apply_phase(ASC& asc, const BasisState& basis_state, double theta) {
        auto it = asc.amplitudes.find(basis_state);
        if (it != asc.amplitudes.end()) {
            // Rotor application: multiplying by e^(i * theta)
            std::complex<double> rotor = std::polar(1.0, theta);
            it->second *= rotor;
        }
        return asc;
    }
};

} // namespace qau

#endif // QAU_RPW_HPP
