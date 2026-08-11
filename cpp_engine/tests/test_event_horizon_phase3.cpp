#include "qau/black_hole.hpp"
#include "qau/quantum_dense.hpp"

#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace {
constexpr double kTolerance = 1e-10;

void require_close(double actual, double expected, double tolerance, const char* message) {
    if (std::abs(actual - expected) > tolerance) {
        throw std::runtime_error(std::string(message) + ": expected " + std::to_string(expected) +
                                 ", got " + std::to_string(actual));
    }
}

void test_unitary_reversibility_and_determinism() {
    std::cout << "[1/5] Unitary reversibility and deterministic seeded circuits...\n";
    qau::DenseQuantumState state_a(5);
    qau::DenseQuantumState state_b(5);
    const qau::StateVector initial = state_a.amplitudes();

    const auto circuit_a = qau::QuantumCircuit::brickwork_scrambler(5, 6, 0xC0FFEEULL);
    const auto circuit_b = qau::QuantumCircuit::brickwork_scrambler(5, 6, 0xC0FFEEULL);
    assert(circuit_a.gates().size() == circuit_b.gates().size());

    circuit_a.apply(state_a);
    circuit_b.apply(state_b);
    require_close(state_a.norm_squared(), 1.0, kTolerance, "Forward circuit must preserve norm");
    for (std::size_t i = 0; i < state_a.amplitudes().size(); ++i) {
        require_close(std::abs(state_a.amplitudes()[i] - state_b.amplitudes()[i]), 0.0, kTolerance,
                      "Seeded circuits must be deterministic");
    }

    circuit_a.apply_inverse(state_a);
    require_close(state_a.norm_squared(), 1.0, kTolerance, "Inverse circuit must preserve norm");
    for (std::size_t i = 0; i < initial.size(); ++i) {
        require_close(std::abs(state_a.amplitudes()[i] - initial[i]), 0.0, kTolerance,
                      "Circuit inverse must restore the initial state");
    }
}

void test_otoc_baseline_and_operator_spreading() {
    std::cout << "[2/5] Exact OTOC baseline and operator-spreading diagnostics...\n";
    qau::DenseQuantumState initial(3);
    qau::QuantumCircuit identity(3);

    // Z_2 and X_0 commute before time evolution, so F(0)=1 and C(0)=0.
    const auto baseline = qau::OTOCAnalyzer::evaluate(initial, identity, 2, 'Z', 0, 'X');
    require_close(baseline.correlator.real(), 1.0, kTolerance, "Commuting OTOC baseline");
    require_close(baseline.correlator.imag(), 0.0, kTolerance, "OTOC baseline imaginary part");
    require_close(baseline.commutator_squared, 0.0, kTolerance, "Commutator baseline");

    // U=CNOT(0,1)CNOT(1,2). Under U^dagger Z_2 U, support reaches qubit 0.
    // It then anticommutes with X_0, giving F=-1 and C=4 exactly.
    qau::QuantumCircuit propagating(3);
    propagating.controlled_not(0, 1);
    propagating.controlled_not(1, 2);
    const auto spread = qau::OTOCAnalyzer::evaluate(initial, propagating, 2, 'Z', 0, 'X');
    require_close(spread.correlator.real(), -1.0, kTolerance, "Propagated OTOC value");
    require_close(spread.correlator.imag(), 0.0, kTolerance, "Propagated OTOC imaginary part");
    require_close(spread.commutator_squared, 4.0, kTolerance, "Operator spreading commutator");
    require_close(spread.norm_error, 0.0, kTolerance, "OTOC evaluation norm stability");
}

void test_dense_entropy_analytic_cases() {
    std::cout << "[3/5] Analytic reduced-density and entropy cases...\n";
    qau::DenseQuantumState product(2);
    auto product_rho = qau::DenseEntropyDiagnostics::reduced_density_matrix(product, {0});
    require_close(qau::DenseEntropyDiagnostics::von_neumann_entropy(product_rho), 0.0, kTolerance,
                  "Product-state entropy");
    require_close(qau::DenseEntropyDiagnostics::purity(product_rho), 1.0, kTolerance,
                  "Product-state purity");

    qau::DenseQuantumState bell(2);
    qau::QuantumCircuit bell_preparation(2);
    bell_preparation.hadamard(0);
    bell_preparation.controlled_not(0, 1);
    bell_preparation.apply(bell);
    auto bell_rho = qau::DenseEntropyDiagnostics::reduced_density_matrix(bell, {0});
    require_close(qau::DenseEntropyDiagnostics::von_neumann_entropy(bell_rho), std::log(2.0), kTolerance,
                  "Bell-pair entropy");
    require_close(qau::DenseEntropyDiagnostics::purity(bell_rho), 0.5, kTolerance,
                  "Bell-pair reduced purity");
}

void test_page_curve_toy_model() {
    std::cout << "[4/5] Closed-unitary evaporation toy model and Page-like entropy curve...\n";
    qau::EvaporatingBlackHoleToyModel model(6, 0xBADC0DEULL, 4);
    model.prepare_seeded_initial_state(8);
    const auto curve = model.evaporate_all();

    assert(curve.size() == 7);
    require_close(curve.front().radiation_entropy, 0.0, kTolerance, "Initial radiation entropy");
    require_close(curve.back().radiation_entropy, 0.0, kTolerance, "Final radiation entropy of globally pure state");
    require_close(curve.back().radiation_purity, 1.0, kTolerance, "Final radiation purity");

    double peak_entropy = 0.0;
    std::size_t peak_index = 0;
    for (std::size_t i = 0; i < curve.size(); ++i) {
        if (curve[i].radiation_entropy > peak_entropy) {
            peak_entropy = curve[i].radiation_entropy;
            peak_index = i;
        }
        if (curve[i].global_norm_error > kTolerance) {
            throw std::runtime_error("Evaporation model violated global unitarity tolerance.");
        }
    }
    if (peak_index == 0 || peak_index + 1 >= curve.size() || peak_entropy < 0.25) {
        throw std::runtime_error("Toy evaporation model did not yield a resolvable Page-like entropy peak.");
    }

    std::cout << std::fixed << std::setprecision(6) << "    radiation entropy: ";
    for (const auto& point : curve) std::cout << point.radiation_entropy << " ";
    std::cout << "\n";
}

void test_page_curve_reproducibility() {
    std::cout << "[5/5] Page-curve reproducibility...\n";
    qau::EvaporatingBlackHoleToyModel left(5, 20260811ULL, 3);
    qau::EvaporatingBlackHoleToyModel right(5, 20260811ULL, 3);
    left.prepare_seeded_initial_state(7);
    right.prepare_seeded_initial_state(7);
    const auto left_curve = left.evaporate_all();
    const auto right_curve = right.evaporate_all();
    assert(left_curve.size() == right_curve.size());
    for (std::size_t i = 0; i < left_curve.size(); ++i) {
        require_close(left_curve[i].radiation_entropy, right_curve[i].radiation_entropy, kTolerance,
                      "Seeded Page-curve entropy reproducibility");
        require_close(left_curve[i].radiation_purity, right_curve[i].radiation_purity, kTolerance,
                      "Seeded Page-curve purity reproducibility");
    }
}
} // namespace

int main() {
    std::cout << "=== Project Event Horizon — Phase III Validation Harness ===\n";
    std::cout << "Model status: finite unitary-circuit toy model; not an astrophysical black-hole solver.\n";
    try {
        test_unitary_reversibility_and_determinism();
        test_otoc_baseline_and_operator_spreading();
        test_dense_entropy_analytic_cases();
        test_page_curve_toy_model();
        test_page_curve_reproducibility();
        std::cout << "=== Phase III harness: ALL TESTS GREEN ===\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Phase III harness failure: " << error.what() << "\n";
        return 1;
    }
}
