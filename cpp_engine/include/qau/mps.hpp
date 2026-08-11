#ifndef QAU_MPS_HPP
#define QAU_MPS_HPP

#include <vector>
#include <complex>
#include <Eigen/Dense>
#include <memory>
#include <stdexcept>

namespace qau {

/**
 * Matrix Product State (MPS) representing multi-qubit quantum states
 * with polynomial memory scaling via bond dimension chi.
 */
class MatrixProductState {
public:
    int num_qubits;
    int max_bond_dimension;
    std::vector<Eigen::MatrixXcd> tensors; // Using Eigen complex matrices for site tensors

    MatrixProductState(int qubits, int max_chi = 16) 
        : num_qubits(qubits), max_bond_dimension(max_chi) {
        // Initialize trivial product state |00...0>
        for (int i = 0; i < num_qubits; ++i) {
            Eigen::MatrixXcd t(2, 1);
            t.setZero();
            t(0, 0) = std::complex<double>(1.0, 0.0); // |0> state
            tensors.push_back(t);
        }
    }

    // Apply single-qubit gate
    void apply_single_qubit_gate(int site, const Eigen::Matrix2cd& gate) {
        if (site < 0 || site >= num_qubits) {
            throw std::out_of_range("Qubit site index out of range.");
        }
        tensors[site] = gate * tensors[site];
    }

    int get_num_qubits() const { return num_qubits; }
    int get_max_bond_dimension() const { return max_bond_dimension; }
};

} // namespace qau

#endif // QAU_MPS_HPP
