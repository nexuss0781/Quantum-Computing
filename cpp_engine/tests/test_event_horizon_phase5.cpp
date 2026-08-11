#include "qau/wormhole.hpp"

#include <array>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

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

void test_ideal_six_state_transfer() {
    std::cout << "[1/5] Ideal entanglement-assisted coherent transfer...\n";
    for (const auto& message : qau::ER_EPR_WormholeToy::six_state_ensemble()) {
        const auto outcome = qau::ER_EPR_WormholeToy::transfer(message, true, true);
        require_close(outcome.carrier_bell_fidelity, 1.0, kTolerance, "Bell-pair carrier fidelity");
        require_close(outcome.receiver_fidelity, 1.0, kTolerance, "Ideal transfer fidelity");
        require_close(outcome.receiver_purity, 1.0, kTolerance, "Ideal decoded receiver purity");
        require_true(outcome.global_norm_error < kTolerance, "Ideal protocol must preserve global norm.");
    }
    require_close(qau::ER_EPR_WormholeToy::average_fidelity(true, true), 1.0, kTolerance,
                  "Six-state average ideal fidelity");
}

void test_missing_classical_correction_channel() {
    std::cout << "[2/5] Causal control: no correction/feed-forward channel...\n";
    const auto message = qau::QubitMessage::from_angles(0.7, 1.1);
    const auto decoded = qau::ER_EPR_WormholeToy::transfer(message, true, true);
    const auto no_correction = qau::ER_EPR_WormholeToy::transfer(message, true, false);
    require_true(decoded.receiver_fidelity > 1.0 - kTolerance, "Decoded branch must transfer the message.");
    require_true(no_correction.receiver_fidelity < 0.75,
                 "Without the correction channel, receiver fidelity must remain below a decoded-state claim.");
    require_close(no_correction.receiver_purity, 0.5, kTolerance,
                  "Receiver without correction must be maximally mixed in ideal teleportation.");
}

void test_no_entanglement_control() {
    std::cout << "[3/5] No-entanglement transfer control...\n";
    const double ideal = qau::ER_EPR_WormholeToy::average_fidelity(true, true);
    const double no_entanglement = qau::ER_EPR_WormholeToy::average_fidelity(false, true);
    require_true(ideal > no_entanglement + 0.20,
                 "Pre-existing entanglement must materially improve this transfer protocol.");
    require_true(no_entanglement < 0.80,
                 "No-entanglement control must not report high-fidelity teleportation.");
}

void test_bloch_vector_recovery() {
    std::cout << "[4/5] Receiver Bloch-vector recovery...\n";
    const std::array<qau::QubitMessage, 3> messages{
        qau::QubitMessage{{1.0, 0.0}, {0.0, 0.0}},
        qau::QubitMessage{{1.0 / std::sqrt(2.0), 0.0}, {1.0 / std::sqrt(2.0), 0.0}},
        qau::QubitMessage{{1.0 / std::sqrt(2.0), 0.0}, {0.0, 1.0 / std::sqrt(2.0)}}
    };
    const std::array<std::array<double, 3>, 3> expected{{
        {0.0, 0.0, 1.0},
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0}
    }};
    for (std::size_t i = 0; i < messages.size(); ++i) {
        const auto outcome = qau::ER_EPR_WormholeToy::transfer(messages[i], true, true);
        for (std::size_t component = 0; component < 3; ++component) {
            require_close(outcome.receiver_bloch[component], expected[i][component], kTolerance,
                          "Decoded receiver Bloch-vector component");
        }
    }
}

void test_input_validation_and_determinism() {
    std::cout << "[5/5] Message validation and deterministic outcomes...\n";
    bool threw = false;
    try {
        qau::ER_EPR_WormholeToy::transfer({{0.0, 0.0}, {0.0, 0.0}});
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require_true(threw, "Zero message state must be rejected.");

    const auto message = qau::QubitMessage::from_angles(1.2, -0.7);
    const auto first = qau::ER_EPR_WormholeToy::transfer(message, true, true);
    const auto second = qau::ER_EPR_WormholeToy::transfer(message, true, true);
    require_close(first.receiver_fidelity, second.receiver_fidelity, kTolerance, "Deterministic fidelity");
    require_close(first.receiver_bloch[0], second.receiver_bloch[0], kTolerance, "Deterministic Bloch x");
    require_close(first.receiver_bloch[1], second.receiver_bloch[1], kTolerance, "Deterministic Bloch y");
    require_close(first.receiver_bloch[2], second.receiver_bloch[2], kTolerance, "Deterministic Bloch z");
}
} // namespace

int main() {
    std::cout << "=== Project Event Horizon — Phase V ER=EPR Toy-Model Harness ===\n";
    std::cout << "Model status: entanglement-assisted quantum teleportation; not a physical macroscopic wormhole.\n";
    try {
        test_ideal_six_state_transfer();
        test_missing_classical_correction_channel();
        test_no_entanglement_control();
        test_bloch_vector_recovery();
        test_input_validation_and_determinism();
        std::cout << "=== Phase V harness: ALL TESTS GREEN ===\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Phase V harness failure: " << error.what() << "\n";
        return 1;
    }
}
