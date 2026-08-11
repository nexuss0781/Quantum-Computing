#ifndef QAU_ENTROPY_HPP
#define QAU_ENTROPY_HPP

#include "asc.hpp"
#include <Eigen/Dense>
#include <cmath>
#include <vector>
#include <algorithm>
#include <map>

namespace qau {

class EntropyCalculator {
public:
    /**
     * Compute the reduced density matrix for a subsystem of qubits.
     * @param asc The full system state.
     * @param subsystem_indices The indices of qubits in the subsystem.
     * @return Reduced density matrix as an Eigen::MatrixXcd.
     */
    static Eigen::MatrixXcd reduced_density_matrix(const ASC& asc, const std::vector<int>& subsystem_indices) {
        int n = asc.size;
        int k = subsystem_indices.size();
        int dim_sub = 1 << k;
        Eigen::MatrixXcd rho = Eigen::MatrixXcd::Zero(dim_sub, dim_sub);

        // Sort subsystem indices for easier bit manipulation
        std::vector<int> sub = subsystem_indices;
        std::sort(sub.begin(), sub.end());
        
        // Identify complement indices
        std::vector<int> complement;
        for (int i = 0; i < n; ++i) {
            if (std::find(sub.begin(), sub.end(), i) == sub.end()) {
                complement.push_back(i);
            }
        }

        // Trace out complement
        // For each basis state in the ASC, extract subsystem bits and complement bits
        for (auto const& [state_i, alpha_i] : asc.amplitudes) {
            for (auto const& [state_j, alpha_j] : asc.amplitudes) {
                // Check if complement bits are identical
                bool comp_match = true;
                for (int idx : complement) {
                    if (state_i[idx] != state_j[idx]) {
                        comp_match = false;
                        break;
                    }
                }

                if (comp_match) {
                    // Map subsystem bits to matrix indices
                    int row = 0, col = 0;
                    for (int i = 0; i < k; ++i) {
                        row += state_i[sub[i]] * (1 << i);
                        col += state_j[sub[i]] * (1 << i);
                    }
                    rho(row, col) += alpha_i * std::conj(alpha_j);
                }
            }
        }
        return rho;
    }

    /**
     * Compute Von Neumann Entropy: S = -Tr(rho * log(rho))
     */
    static double von_neumann_entropy(const Eigen::MatrixXcd& rho) {
        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXcd> solver(rho);
        auto eigenvalues = solver.eigenvalues();
        double entropy = 0.0;
        for (int i = 0; i < eigenvalues.size(); ++i) {
            double p = eigenvalues(i);
            if (p > 1e-12) {
                entropy -= p * std::log(p);
            }
        }
        return std::abs(entropy);
    }

    /**
     * Compute Rényi Entropy of order alpha: S_alpha = 1/(1-alpha) * log(Tr(rho^alpha))
     */
    static double renyi_entropy(const Eigen::MatrixXcd& rho, double alpha) {
        if (std::abs(alpha - 1.0) < 1e-6) return von_neumann_entropy(rho);
        
        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXcd> solver(rho);
        auto eigenvalues = solver.eigenvalues();
        double sum = 0.0;
        for (int i = 0; i < eigenvalues.size(); ++i) {
            double p = eigenvalues(i);
            if (p > 1e-12) {
                sum += std::pow(p, alpha);
            }
        }
        return (1.0 / (1.0 - alpha)) * std::log(sum);
    }
};

} // namespace qau

#endif // QAU_ENTROPY_HPP
