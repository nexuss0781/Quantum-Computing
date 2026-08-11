#ifndef QAU_QUANTUM_DENSE_HPP
#define QAU_QUANTUM_DENSE_HPP

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace qau {

using Complex = std::complex<double>;
using StateVector = std::vector<Complex>;

/**
 * Small exact state-vector backend for validated quantum-information toy models.
 *
 * Qubit index 0 is the least-significant bit in the computational basis index.
 * This class intentionally limits itself to small systems because it is designed
 * for exact diagnostics, regression tests, and research prototypes—not claims
 * of large-scale black-hole or quantum-gravity simulation.
 */
class DenseQuantumState {
public:
    explicit DenseQuantumState(int num_qubits)
        : num_qubits_(num_qubits), amplitudes_(dimension_for(num_qubits), {0.0, 0.0}) {
        if (num_qubits < 1 || num_qubits > 20) {
            throw std::invalid_argument("DenseQuantumState supports 1 to 20 qubits.");
        }
        amplitudes_.at(0) = {1.0, 0.0};
    }

    DenseQuantumState(int num_qubits, StateVector amplitudes)
        : num_qubits_(num_qubits), amplitudes_(std::move(amplitudes)) {
        if (num_qubits < 1 || num_qubits > 20 || amplitudes_.size() != dimension_for(num_qubits)) {
            throw std::invalid_argument("Invalid dense quantum state dimensions.");
        }
    }

    int num_qubits() const noexcept { return num_qubits_; }
    const StateVector& amplitudes() const noexcept { return amplitudes_; }
    StateVector& amplitudes() noexcept { return amplitudes_; }

    double norm_squared() const {
        double result = 0.0;
        for (const auto& amplitude : amplitudes_) result += std::norm(amplitude);
        return result;
    }

    void normalize() {
        const double norm = std::sqrt(norm_squared());
        if (norm <= std::numeric_limits<double>::epsilon()) {
            throw std::runtime_error("Cannot normalize a zero quantum state.");
        }
        for (auto& amplitude : amplitudes_) amplitude /= norm;
    }

    static std::size_t dimension_for(int num_qubits) {
        if (num_qubits < 0 || num_qubits >= static_cast<int>(8 * sizeof(std::size_t))) {
            throw std::invalid_argument("Invalid qubit count.");
        }
        return std::size_t{1} << num_qubits;
    }

private:
    int num_qubits_;
    StateVector amplitudes_;
};

enum class GateType {
    Hadamard,
    RotationY,
    RotationZ,
    ControlledNot
};

struct CircuitGate {
    GateType type;
    int qubit_a;
    int qubit_b = -1;
    double angle = 0.0;
};

/** A reversible circuit with deterministic gates and a reverse-time executor. */
class QuantumCircuit {
public:
    explicit QuantumCircuit(int num_qubits) : num_qubits_(num_qubits) {
        if (num_qubits < 1 || num_qubits > 20) {
            throw std::invalid_argument("QuantumCircuit supports 1 to 20 qubits.");
        }
    }

    int num_qubits() const noexcept { return num_qubits_; }
    const std::vector<CircuitGate>& gates() const noexcept { return gates_; }

    void hadamard(int target) { gates_.push_back({GateType::Hadamard, validate_qubit(target)}); }
    void rotation_y(int target, double angle) {
        gates_.push_back({GateType::RotationY, validate_qubit(target), -1, angle});
    }
    void rotation_z(int target, double angle) {
        gates_.push_back({GateType::RotationZ, validate_qubit(target), -1, angle});
    }
    void controlled_not(int control, int target) {
        validate_qubit(control);
        validate_qubit(target);
        if (control == target) throw std::invalid_argument("CNOT control and target must differ.");
        gates_.push_back({GateType::ControlledNot, control, target});
    }

    void apply(DenseQuantumState& state) const {
        check_state(state);
        for (const auto& gate : gates_) apply_gate(state.amplitudes(), gate, false);
    }

    void apply_inverse(DenseQuantumState& state) const {
        check_state(state);
        for (auto it = gates_.rbegin(); it != gates_.rend(); ++it) {
            apply_gate(state.amplitudes(), *it, true);
        }
    }

    /**
     * Deterministic brickwork circuit for reproducible scrambling experiments.
     * It contains seeded single-qubit rotations and alternating nearest-neighbor
     * CNOT layers; it is a toy random-circuit construction, not an SYK solver.
     */
    static QuantumCircuit brickwork_scrambler(
        int num_qubits, int layers, std::uint64_t seed, const std::vector<int>& active_qubits = {}) {
        if (layers < 0) throw std::invalid_argument("Layer count cannot be negative.");
        QuantumCircuit circuit(num_qubits);
        std::vector<int> active = active_qubits;
        if (active.empty()) {
            active.resize(num_qubits);
            for (int i = 0; i < num_qubits; ++i) active[i] = i;
        }
        for (int qubit : active) circuit.validate_qubit(qubit);
        std::sort(active.begin(), active.end());
        if (std::adjacent_find(active.begin(), active.end()) != active.end()) {
            throw std::invalid_argument("Active qubits must be unique.");
        }

        std::mt19937_64 rng(seed);
        std::uniform_real_distribution<double> angle_distribution(-M_PI, M_PI);
        for (int layer = 0; layer < layers; ++layer) {
            for (int qubit : active) {
                circuit.rotation_y(qubit, angle_distribution(rng));
                circuit.rotation_z(qubit, angle_distribution(rng));
            }
            const int offset = layer % 2;
            for (std::size_t i = static_cast<std::size_t>(offset); i + 1 < active.size(); i += 2) {
                circuit.controlled_not(active[i], active[i + 1]);
            }
        }
        return circuit;
    }

    static void apply_pauli(StateVector& state, int num_qubits, int target, char pauli) {
        if (target < 0 || target >= num_qubits) throw std::out_of_range("Pauli target is out of range.");
        const std::size_t mask = std::size_t{1} << target;
        const std::size_t dim = DenseQuantumState::dimension_for(num_qubits);
        if (state.size() != dim) throw std::invalid_argument("State vector dimension mismatch.");

        if (pauli == 'I') return;
        if (pauli == 'Z') {
            for (std::size_t index = 0; index < dim; ++index) {
                if (index & mask) state[index] = -state[index];
            }
            return;
        }
        if (pauli == 'X' || pauli == 'Y') {
            for (std::size_t index = 0; index < dim; ++index) {
                if (index & mask) continue;
                const std::size_t paired = index | mask;
                const Complex a = state[index];
                const Complex b = state[paired];
                if (pauli == 'X') {
                    state[index] = b;
                    state[paired] = a;
                } else {
                    state[index] = Complex{0.0, -1.0} * b;
                    state[paired] = Complex{0.0, 1.0} * a;
                }
            }
            return;
        }
        throw std::invalid_argument("Pauli operator must be I, X, Y, or Z.");
    }

    static Complex inner_product(const StateVector& bra, const StateVector& ket) {
        if (bra.size() != ket.size()) throw std::invalid_argument("Inner-product dimensions do not match.");
        Complex result{0.0, 0.0};
        for (std::size_t i = 0; i < bra.size(); ++i) result += std::conj(bra[i]) * ket[i];
        return result;
    }

private:
    int num_qubits_;
    std::vector<CircuitGate> gates_;

    int validate_qubit(int qubit) const {
        if (qubit < 0 || qubit >= num_qubits_) throw std::out_of_range("Qubit index is out of range.");
        return qubit;
    }

    void check_state(const DenseQuantumState& state) const {
        if (state.num_qubits() != num_qubits_) {
            throw std::invalid_argument("Circuit and quantum state have incompatible qubit counts.");
        }
    }

    static void apply_single_qubit_matrix(
        StateVector& state, int qubit, Complex m00, Complex m01, Complex m10, Complex m11) {
        const std::size_t mask = std::size_t{1} << qubit;
        const std::size_t dim = state.size();
        for (std::size_t index = 0; index < dim; ++index) {
            if (index & mask) continue;
            const std::size_t paired = index | mask;
            const Complex a = state[index];
            const Complex b = state[paired];
            state[index] = m00 * a + m01 * b;
            state[paired] = m10 * a + m11 * b;
        }
    }

    static void apply_gate(StateVector& state, const CircuitGate& gate, bool inverse) {
        switch (gate.type) {
            case GateType::Hadamard: {
                const double inv_sqrt2 = 1.0 / std::sqrt(2.0);
                apply_single_qubit_matrix(state, gate.qubit_a,
                    {inv_sqrt2, 0.0}, {inv_sqrt2, 0.0},
                    {inv_sqrt2, 0.0}, {-inv_sqrt2, 0.0});
                break;
            }
            case GateType::RotationY: {
                const double signed_angle = inverse ? -gate.angle : gate.angle;
                const double c = std::cos(signed_angle / 2.0);
                const double s = std::sin(signed_angle / 2.0);
                apply_single_qubit_matrix(state, gate.qubit_a,
                    {c, 0.0}, {-s, 0.0}, {s, 0.0}, {c, 0.0});
                break;
            }
            case GateType::RotationZ: {
                const double signed_angle = inverse ? -gate.angle : gate.angle;
                const Complex lower = std::polar(1.0, -signed_angle / 2.0);
                const Complex upper = std::polar(1.0, signed_angle / 2.0);
                apply_single_qubit_matrix(state, gate.qubit_a,
                    lower, {0.0, 0.0}, {0.0, 0.0}, upper);
                break;
            }
            case GateType::ControlledNot: {
                const std::size_t control_mask = std::size_t{1} << gate.qubit_a;
                const std::size_t target_mask = std::size_t{1} << gate.qubit_b;
                for (std::size_t index = 0; index < state.size(); ++index) {
                    if ((index & control_mask) == 0 || (index & target_mask) != 0) continue;
                    const std::size_t paired = index | target_mask;
                    std::swap(state[index], state[paired]);
                }
                break;
            }
        }
    }
};

struct OTOCResult {
    Complex correlator;
    double commutator_squared;
    double norm_error;
};

/** Exact finite-register OTOC evaluator. */
class OTOCAnalyzer {
public:
    static OTOCResult evaluate(
        const DenseQuantumState& initial_state,
        const QuantumCircuit& evolution,
        int w_qubit,
        char w_pauli,
        int v_qubit,
        char v_pauli) {
        if (initial_state.num_qubits() != evolution.num_qubits()) {
            throw std::invalid_argument("OTOC state and circuit dimensions are incompatible.");
        }
        const int n = initial_state.num_qubits();

        auto apply_time_evolved_w = [&](DenseQuantumState& state) {
            evolution.apply(state);
            QuantumCircuit::apply_pauli(state.amplitudes(), n, w_qubit, w_pauli);
            evolution.apply_inverse(state);
        };

        DenseQuantumState work = initial_state;
        QuantumCircuit::apply_pauli(work.amplitudes(), n, v_qubit, v_pauli);
        apply_time_evolved_w(work);
        QuantumCircuit::apply_pauli(work.amplitudes(), n, v_qubit, v_pauli);
        apply_time_evolved_w(work);

        const Complex correlator = QuantumCircuit::inner_product(initial_state.amplitudes(), work.amplitudes());
        const double commutator_squared = std::max(0.0, 2.0 * (1.0 - correlator.real()));
        const double norm_error = std::abs(work.norm_squared() - 1.0);
        return {correlator, commutator_squared, norm_error};
    }
};

} // namespace qau

#endif // QAU_QUANTUM_DENSE_HPP
