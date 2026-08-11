#ifndef QAU_BLACK_HOLE_HPP
#define QAU_BLACK_HOLE_HPP

#include "quantum_dense.hpp"

#include <Eigen/Dense>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace qau {

/** Exact density-matrix diagnostics for small pure state vectors. */
class DenseEntropyDiagnostics {
public:
    static Eigen::MatrixXcd reduced_density_matrix(
        const DenseQuantumState& state, const std::vector<int>& kept_qubits) {
        const int n = state.num_qubits();
        validate_partition(n, kept_qubits);
        const int kept_count = static_cast<int>(kept_qubits.size());
        const int kept_dim = 1 << kept_count;
        Eigen::MatrixXcd rho = Eigen::MatrixXcd::Zero(kept_dim, kept_dim);

        std::vector<int> complement;
        complement.reserve(n - kept_count);
        for (int qubit = 0; qubit < n; ++qubit) {
            if (std::find(kept_qubits.begin(), kept_qubits.end(), qubit) == kept_qubits.end()) {
                complement.push_back(qubit);
            }
        }

        const auto& amplitudes = state.amplitudes();
        for (std::size_t i = 0; i < amplitudes.size(); ++i) {
            const int row = projected_index(i, kept_qubits);
            const int complement_i = projected_index(i, complement);
            for (std::size_t j = 0; j < amplitudes.size(); ++j) {
                if (projected_index(j, complement) != complement_i) continue;
                const int col = projected_index(j, kept_qubits);
                rho(row, col) += amplitudes[i] * std::conj(amplitudes[j]);
            }
        }
        return rho;
    }

    static double von_neumann_entropy(const Eigen::MatrixXcd& density_matrix) {
        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXcd> solver(density_matrix);
        if (solver.info() != Eigen::Success) {
            throw std::runtime_error("Unable to diagonalize density matrix.");
        }
        double entropy = 0.0;
        for (int i = 0; i < solver.eigenvalues().size(); ++i) {
            const double probability = std::max(0.0, solver.eigenvalues()(i));
            if (probability > 1e-13) entropy -= probability * std::log(probability);
        }
        return entropy;
    }

    static double purity(const Eigen::MatrixXcd& density_matrix) {
        const Complex trace = (density_matrix * density_matrix).trace();
        return trace.real();
    }

private:
    static void validate_partition(int num_qubits, const std::vector<int>& kept_qubits) {
        std::vector<int> sorted = kept_qubits;
        std::sort(sorted.begin(), sorted.end());
        if (std::adjacent_find(sorted.begin(), sorted.end()) != sorted.end()) {
            throw std::invalid_argument("Subsystem qubit indices must be unique.");
        }
        for (int qubit : sorted) {
            if (qubit < 0 || qubit >= num_qubits) {
                throw std::out_of_range("Subsystem qubit index is out of range.");
            }
        }
    }

    static int projected_index(std::size_t full_index, const std::vector<int>& selected_qubits) {
        int result = 0;
        for (std::size_t position = 0; position < selected_qubits.size(); ++position) {
            const int qubit = selected_qubits[position];
            result |= static_cast<int>((full_index >> qubit) & std::size_t{1}) << position;
        }
        return result;
    }
};

struct PageCurvePoint {
    int evaporation_step;
    int black_hole_qubits;
    int radiation_qubits;
    double radiation_entropy;
    double radiation_purity;
    double global_norm_error;
};

/**
 * Finite random-circuit evaporation toy model.
 *
 * The full state remains closed and unitary. “Evaporation” changes only the
 * bookkeeping partition: a qubit leaves the active black-hole subsystem and
 * joins radiation after a seeded scrambling circuit. This permits exact Page
 * diagnostics while honestly remaining a finite-dimensional toy model.
 */
class EvaporatingBlackHoleToyModel {
public:
    EvaporatingBlackHoleToyModel(int total_qubits, std::uint64_t seed, int scrambling_layers_per_step = 3)
        : state_(total_qubits), seed_(seed), scrambling_layers_per_step_(scrambling_layers_per_step) {
        if (total_qubits < 2 || total_qubits > 12) {
            throw std::invalid_argument("EvaporatingBlackHoleToyModel supports 2 to 12 qubits.");
        }
        if (scrambling_layers_per_step_ < 1) {
            throw std::invalid_argument("At least one scrambling layer is required per evaporation step.");
        }
        black_hole_qubits_.resize(total_qubits);
        std::iota(black_hole_qubits_.begin(), black_hole_qubits_.end(), 0);
    }

    const DenseQuantumState& state() const noexcept { return state_; }
    const std::vector<int>& black_hole_qubits() const noexcept { return black_hole_qubits_; }
    const std::vector<int>& radiation_qubits() const noexcept { return radiation_qubits_; }

    void prepare_seeded_initial_state(int layers = 6) {
        if (layers < 0) throw std::invalid_argument("Initial layers cannot be negative.");
        const auto circuit = QuantumCircuit::brickwork_scrambler(
            state_.num_qubits(), layers, seed_, black_hole_qubits_);
        circuit.apply(state_);
    }

    PageCurvePoint evaporate_one_qubit() {
        if (black_hole_qubits_.empty()) {
            throw std::runtime_error("No black-hole qubits remain to radiate.");
        }
        const auto circuit = QuantumCircuit::brickwork_scrambler(
            state_.num_qubits(), scrambling_layers_per_step_, seed_ + static_cast<std::uint64_t>(step_ + 1),
            black_hole_qubits_);
        circuit.apply(state_);

        radiation_qubits_.push_back(black_hole_qubits_.back());
        black_hole_qubits_.pop_back();
        ++step_;
        return diagnostics();
    }

    std::vector<PageCurvePoint> evaporate_all() {
        std::vector<PageCurvePoint> results;
        results.reserve(black_hole_qubits_.size() + 1);
        results.push_back(diagnostics());
        while (!black_hole_qubits_.empty()) results.push_back(evaporate_one_qubit());
        return results;
    }

    PageCurvePoint diagnostics() const {
        Eigen::MatrixXcd rho_radiation = DenseEntropyDiagnostics::reduced_density_matrix(state_, radiation_qubits_);
        return {
            step_,
            static_cast<int>(black_hole_qubits_.size()),
            static_cast<int>(radiation_qubits_.size()),
            DenseEntropyDiagnostics::von_neumann_entropy(rho_radiation),
            DenseEntropyDiagnostics::purity(rho_radiation),
            std::abs(state_.norm_squared() - 1.0)
        };
    }

private:
    DenseQuantumState state_;
    std::uint64_t seed_;
    int scrambling_layers_per_step_;
    int step_ = 0;
    std::vector<int> black_hole_qubits_;
    std::vector<int> radiation_qubits_;
};

} // namespace qau

#endif // QAU_BLACK_HOLE_HPP
