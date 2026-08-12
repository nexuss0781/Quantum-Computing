#include "qau/vacuum_mode.hpp"

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

void test_vacuum_unit() {
    std::cout << "[1/6] Vacuum unit and zero-point invariants...\n";
    const auto vacuum = qau::VacuumMode::vacuum(32);
    const auto moments = vacuum.quadrature_moments();
    require_close(vacuum.occupation_probability(0), 1.0, kTolerance, "Vacuum occupation P(0)");
    require_close(vacuum.mean_occupation(), 0.0, kTolerance, "Vacuum mean occupation");
    require_close(vacuum.mean_energy(), 0.5, kTolerance, "Vacuum zero-point energy");
    require_close(moments.x_mean, 0.0, kTolerance, "Vacuum X mean");
    require_close(moments.p_mean, 0.0, kTolerance, "Vacuum P mean");
    require_close(moments.x_variance, 0.5, kTolerance, "Vacuum X variance");
    require_close(moments.p_variance, 0.5, kTolerance, "Vacuum P variance");
    require_close(vacuum.uncertainty_product(), 0.25, kTolerance, "Vacuum uncertainty product");
    require_close(vacuum.norm_squared_value(), 1.0, kTolerance, "Vacuum norm");
}

void test_coherent_mode() {
    std::cout << "[2/6] Coherent excitation and displacement observables...\n";
    const qau::Complex alpha{0.6, -0.4};
    const auto coherent = qau::VacuumMode::coherent(48, alpha);
    const auto moments = coherent.quadrature_moments();
    require_close(coherent.mean_occupation(), std::norm(alpha), 1e-8, "Coherent occupation");
    require_close(moments.x_mean, std::sqrt(2.0) * alpha.real(), 1e-8, "Coherent X mean");
    require_close(moments.p_mean, std::sqrt(2.0) * alpha.imag(), 1e-8, "Coherent P mean");
    require_close(moments.x_variance, 0.5, 1e-8, "Coherent X variance");
    require_close(moments.p_variance, 0.5, 1e-8, "Coherent P variance");
}

void test_squeezed_vacuum() {
    std::cout << "[3/6] Squeezed vacuum quadratures and uncertainty bound...\n";
    const double r = 0.75;
    const auto squeezed = qau::VacuumMode::squeezed_vacuum(64, r, 0.0);
    const auto moments = squeezed.quadrature_moments();
    require_true(moments.x_variance < 0.5, "Squeezed X quadrature must be below vacuum noise.");
    require_true(moments.p_variance > 0.5, "Anti-squeezed P quadrature must exceed vacuum noise.");
    require_close(moments.x_variance, 0.5 * std::exp(-2.0 * r), 1e-8, "Squeezed X variance");
    require_close(moments.p_variance, 0.5 * std::exp(2.0 * r), 1e-8, "Anti-squeezed P variance");
    require_true(squeezed.uncertainty_product() >= 0.25 - 1e-10,
                 "Squeezed state must obey the uncertainty bound.");
    require_close(squeezed.norm_squared_value(), 1.0, kTolerance, "Squeezed-state norm");
}

void test_phase_rotation() {
    std::cout << "[4/6] Vacuum-mode phase rotation covariance...\n";
    const auto before = qau::VacuumMode::squeezed_vacuum(64, 0.55, 0.0);
    auto rotated = before;
    rotated.phase_rotate(M_PI / 2.0);
    const auto scan_before = before.scan_quadratures(720);
    const auto scan_after = rotated.scan_quadratures(720);
    require_close(scan_before.minimum_variance, scan_after.minimum_variance, 1e-8,
                  "Phase rotation must preserve squeezing magnitude");
    require_close(before.mean_energy(), rotated.mean_energy(), 1e-8,
                  "Phase rotation must preserve energy");
}

void test_loss_and_sensing_application() {
    std::cout << "[5/6] Vacuum-referenced quantum sensing application...\n";
    const auto vacuum = qau::VacuumMode::vacuum(32);
    const auto squeezed = qau::VacuumMode::squeezed_vacuum(64, 0.8, 0.0);
    const auto scan = squeezed.scan_quadratures(1440);
    require_true(scan.minimum_variance < 0.5, "Squeezed sensor must beat the vacuum reference.");
    require_true(scan.improvement_db > 0.0, "Squeezing improvement must be positive in dB.");
    const double lossy = qau::VacuumMode::lossy_variance(scan.minimum_variance, 0.90);
    require_true(lossy > scan.minimum_variance, "Optical loss must degrade squeezing.");
    require_true(lossy < vacuum.quadrature_variance(0.0), "Moderate loss must retain sub-vacuum sensitivity.");
    std::cout << "  minimum variance=" << scan.minimum_variance
              << ", improvement_dB=" << scan.improvement_db
              << ", 90% loss-channel variance=" << lossy << "\n";
}

void test_validation_errors() {
    std::cout << "[6/6] Vacuum-unit input validation...\n";
    bool threw = false;
    try {
        qau::VacuumMode::vacuum(1);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require_true(threw, "A one-state cutoff must be rejected.");
    threw = false;
    try {
        qau::VacuumMode::lossy_variance(0.2, 1.2);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require_true(threw, "Invalid transmissivity must be rejected.");
}
} // namespace

int main() {
    std::cout << "=== QAU Vacuum Atomic Unit — Quantum Sensing Harness ===\n";
    std::cout << "Model: truncated bosonic vacuum mode with explicit cutoff and quadrature invariants.\n";
    try {
        test_vacuum_unit();
        test_coherent_mode();
        test_squeezed_vacuum();
        test_phase_rotation();
        test_loss_and_sensing_application();
        test_validation_errors();
        std::cout << "=== Vacuum Atomic Unit harness: ALL TESTS GREEN ===\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Vacuum Atomic Unit harness failure: " << error.what() << "\n";
        return 1;
    }
}
