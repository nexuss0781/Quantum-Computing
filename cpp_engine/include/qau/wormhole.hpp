#ifndef QAU_WORMHOLE_HPP
#define QAU_WORMHOLE_HPP

#include "qau/quantum_dense.hpp"

#include <array>
#include <cmath>
#include <complex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace qau {

/** A normalized one-qubit message state |psi> = alpha|0> + beta|1>. */
struct QubitMessage {
    Complex alpha{1.0, 0.0};
    Complex beta{0.0, 0.0};

    static QubitMessage from_angles(double theta, double phi) {
        if (!std::isfinite(theta) || !std::isfinite(phi)) {
            throw std::invalid_argument("Message-state angles must be finite.");
        }
        return {std::cos(theta / 2.0), std::polar(std::sin(theta / 2.0), phi)};
    }

    void normalize() {
        const double norm = std::sqrt(std::norm(alpha) + std::norm(beta));
        if (norm <= 1e-14) throw std::invalid_argument("A message state cannot be zero.");
        alpha /= norm;
        beta /= norm;
    }
};

struct QubitDensityMatrix {
    Complex rho00{1.0, 0.0};
    Complex rho01{0.0, 0.0};
    Complex rho10{0.0, 0.0};
    Complex rho11{0.0, 0.0};

    double trace() const { return (rho00 + rho11).real(); }
    double purity() const {
        return (rho00 * rho00 + rho01 * rho10 + rho10 * rho01 + rho11 * rho11).real();
    }
    std::array<double, 3> bloch_vector() const {
        return {2.0 * rho01.real(), -2.0 * rho01.imag(), (rho00 - rho11).real()};
    }
};

struct WormholeTransferOutcome {
    bool carriers_entangled;
    bool coherent_feedforward;
    double carrier_bell_fidelity;
    double receiver_fidelity;
    double receiver_purity;
    double global_norm_error;
    QubitDensityMatrix receiver_density;
    std::array<double, 3> receiver_bloch;
};

/**
 * ER=EPR-inspired finite-register state-transfer experiment.
 *
 * Qubits are ordered [message M=0, left carrier L=1, right carrier R=2].
 * The ideal branch is coherent quantum teleportation: a Bell pair is prepared
 * on L/R, M/L are mapped to a Bell basis, and conditional receiver corrections
 * are performed coherently. The coherent correction circuit is equivalent to
 * measurement plus classical feed-forward after tracing the M/L record; it does
 * not permit signalling without the correction channel. It is a quantum-
 * information toy model, not a physical traversable wormhole.
 */
class ER_EPR_WormholeToy {
public:
    static constexpr int kMessageQubit = 0;
    static constexpr int kLeftCarrierQubit = 1;
    static constexpr int kRightCarrierQubit = 2;

    static WormholeTransferOutcome transfer(
        QubitMessage message,
        bool entangle_carriers = true,
        bool coherent_feedforward = true) {
        message.normalize();
        DenseQuantumState state(3);
        state.amplitudes().assign(8, {0.0, 0.0});
        state.amplitudes()[0] = message.alpha;
        state.amplitudes()[1] = message.beta;

        if (entangle_carriers) {
            QuantumCircuit bell_prepare(3);
            bell_prepare.hadamard(kLeftCarrierQubit);
            bell_prepare.controlled_not(kLeftCarrierQubit, kRightCarrierQubit);
            bell_prepare.apply(state);
        }
        const double bell_fidelity = carrier_bell_fidelity(state);

        // Bell-basis analysis of the message and left carrier.
        QuantumCircuit bell_analysis(3);
        bell_analysis.controlled_not(kMessageQubit, kLeftCarrierQubit);
        bell_analysis.hadamard(kMessageQubit);
        bell_analysis.apply(state);

        // Coherent form of the two classical correction bits. When this channel
        // is disabled, the receiver has no decoded message state.
        if (coherent_feedforward) {
            QuantumCircuit x_correction(3);
            x_correction.controlled_not(kLeftCarrierQubit, kRightCarrierQubit);
            x_correction.apply(state);
            apply_controlled_z(state.amplitudes(), kMessageQubit, kRightCarrierQubit);
        }

        const QubitDensityMatrix receiver = reduced_qubit_density(state, kRightCarrierQubit);
        const double fidelity = pure_state_fidelity(message, receiver);
        return {
            entangle_carriers,
            coherent_feedforward,
            bell_fidelity,
            fidelity,
            receiver.purity(),
            std::abs(state.norm_squared() - 1.0),
            receiver,
            receiver.bloch_vector()
        };
    }

    static QubitDensityMatrix reduced_qubit_density(const DenseQuantumState& state, int target_qubit) {
        if (target_qubit < 0 || target_qubit >= state.num_qubits()) {
            throw std::out_of_range("Reduced-density target is out of range.");
        }
        QubitDensityMatrix density{{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}};
        const std::size_t mask = std::size_t{1} << target_qubit;
        const auto& amplitudes = state.amplitudes();
        for (std::size_t index = 0; index < amplitudes.size(); ++index) {
            if (index & mask) continue;
            const std::size_t paired = index | mask;
            const Complex zero = amplitudes[index];
            const Complex one = amplitudes[paired];
            density.rho00 += zero * std::conj(zero);
            density.rho01 += zero * std::conj(one);
            density.rho10 += one * std::conj(zero);
            density.rho11 += one * std::conj(one);
        }
        return density;
    }

    static double pure_state_fidelity(const QubitMessage& message, const QubitDensityMatrix& density) {
        return std::clamp((std::conj(message.alpha) * (density.rho00 * message.alpha + density.rho01 * message.beta) +
                           std::conj(message.beta) * (density.rho10 * message.alpha + density.rho11 * message.beta)).real(),
                          0.0, 1.0);
    }

    static std::vector<QubitMessage> six_state_ensemble() {
        const double inv_sqrt2 = 1.0 / std::sqrt(2.0);
        return {
            {{1.0, 0.0}, {0.0, 0.0}},
            {{0.0, 0.0}, {1.0, 0.0}},
            {{inv_sqrt2, 0.0}, {inv_sqrt2, 0.0}},
            {{inv_sqrt2, 0.0}, {-inv_sqrt2, 0.0}},
            {{inv_sqrt2, 0.0}, {0.0, inv_sqrt2}},
            {{inv_sqrt2, 0.0}, {0.0, -inv_sqrt2}}
        };
    }

    static double average_fidelity(bool entangle_carriers, bool coherent_feedforward) {
        double sum = 0.0;
        const auto ensemble = six_state_ensemble();
        for (const auto& message : ensemble) {
            sum += transfer(message, entangle_carriers, coherent_feedforward).receiver_fidelity;
        }
        return sum / static_cast<double>(ensemble.size());
    }

private:
    static void apply_controlled_z(StateVector& amplitudes, int control, int target) {
        if (control == target) throw std::invalid_argument("Controlled-Z qubits must differ.");
        const std::size_t control_mask = std::size_t{1} << control;
        const std::size_t target_mask = std::size_t{1} << target;
        for (std::size_t index = 0; index < amplitudes.size(); ++index) {
            if ((index & control_mask) && (index & target_mask)) amplitudes[index] = -amplitudes[index];
        }
    }

    static double carrier_bell_fidelity(const DenseQuantumState& state) {
        // Compare the reduced L/R state to |Phi+>; the message state factors out
        // at this stage, so inner-product amplitudes can be grouped by M.
        const auto& amplitudes = state.amplitudes();
        const double inv_sqrt2 = 1.0 / std::sqrt(2.0);
        Complex overlap_for_message_zero = inv_sqrt2 * (amplitudes[0] + amplitudes[6]);
        Complex overlap_for_message_one = inv_sqrt2 * (amplitudes[1] + amplitudes[7]);
        return std::clamp(std::norm(overlap_for_message_zero) + std::norm(overlap_for_message_one), 0.0, 1.0);
    }
};

} // namespace qau

#endif // QAU_WORMHOLE_HPP
