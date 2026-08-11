#ifndef QAU_ASC_HPP
#define QAU_ASC_HPP

#include <vector>
#include <unordered_map>
#include <complex>
#include <cmath>
#include <stdexcept>
#include <numeric>

namespace qau {

using BasisState = std::vector<int>;

// Hash function for BasisState so it can be used in unordered_map
struct BasisStateHash {
    std::size_t operator()(const BasisState& state) const noexcept {
        std::size_t seed = 0;
        for (int bit : state) {
            seed ^= std::hash<int>{}(bit) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

class ASC {
public:
    int size; // Number of qubits/bits
    std::unordered_map<BasisState, std::complex<double>, BasisStateHash> amplitudes;

    ASC(int size = 0, const std::unordered_map<BasisState, std::complex<double>, BasisStateHash>& amps = {})
        : size(size), amplitudes(amps) {
        if (amplitudes.empty() && size > 0) {
            BasisState default_state(size, 0);
            amplitudes[default_state] = std::complex<double>(1.0, 0.0);
        }
    }

    ASC& normalize() {
        double norm_sq = 0.0;
        for (const auto& pair : amplitudes) {
            norm_sq += std::norm(pair.second);
        }
        double norm = std::sqrt(norm_sq);
        if (norm > 0.0) {
            for (auto& pair : amplitudes) {
                pair.second /= norm;
            }
        }
        return *this;
    }

    std::vector<std::complex<double>> get_state_vector() const {
        int dim = 1 << size;
        std::vector<std::complex<double>> vec(dim, {0.0, 0.0});
        for (const auto& pair : amplitudes) {
            int idx = 0;
            const auto& state = pair.first;
            for (size_t i = 0; i < state.size(); ++i) {
                idx += state[state.size() - 1 - i] * (1 << i);
            }
            if (idx < dim) {
                vec[idx] = pair.second;
            }
        }
        return vec;
    }
};

} // namespace qau

#endif // QAU_ASC_HPP
