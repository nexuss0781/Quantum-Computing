#include "qau/vacuum_adaptive.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void require_true(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
void require_close(double a, double b, double tol, const char* message) {
    if (std::abs(a - b) > tol) {
        throw std::runtime_error(std::string(message) + " expected=" + std::to_string(b) +
                                 " actual=" + std::to_string(a));
    }
}
}

int main() {
    std::cout << "=== QAU Adaptive Vacuum Controller — Derivation Harness ===\n";
    try {
        const double eta = 0.86;
        const double sigma2 = 0.08;
        const auto policy = qau::VacuumAdaptiveController::optimize(eta, sigma2);
        const double kappa = std::exp(-2.0 * sigma2);
        const double expected_r = 0.5 * std::atanh(kappa);
        const double expected_v = 0.5 * (1.0 - eta) +
                                  0.5 * eta * std::sqrt(1.0 - kappa * kappa);

        std::cout << "[1/5] Closed-form policy...\n";
        require_true(policy.finite_optimum, "Positive phase diffusion must create a finite optimum.");
        require_close(policy.squeeze_parameter, expected_r, 1e-12, "Analytic r* mismatch");
        require_close(policy.predicted_minimum_variance, expected_v, 1e-12, "Analytic V* mismatch");

        std::cout << "[2/5] Stationarity condition...\n";
        const double slope = qau::VacuumAdaptiveController::derivative(policy.squeeze_parameter, eta, sigma2);
        require_close(slope, 0.0, 1e-7, "Closed-form optimum is not stationary");

        std::cout << "[3/5] Local minimum property...\n";
        const double left = qau::VacuumAdaptiveController::minimum_variance(policy.squeeze_parameter - 0.01, eta, sigma2);
        const double right = qau::VacuumAdaptiveController::minimum_variance(policy.squeeze_parameter + 0.01, eta, sigma2);
        require_true(policy.predicted_minimum_variance < left && policy.predicted_minimum_variance < right,
                     "Closed-form policy is not locally minimal");

        std::cout << "[4/5] Loss monotonicity...\n";
        const double low_loss = qau::VacuumAdaptiveController::minimum_variance(policy.squeeze_parameter, 0.95, sigma2);
        const double high_loss = qau::VacuumAdaptiveController::minimum_variance(policy.squeeze_parameter, 0.70, sigma2);
        require_true(low_loss < high_loss, "More loss must not improve the predicted minimum variance");

        std::cout << "[5/5] Limiting cases and input safety...\n";
        const auto zero_phase = qau::VacuumAdaptiveController::optimize(eta, 0.0);
        require_true(!zero_phase.finite_optimum, "Zero phase diffusion must report no finite optimum");
        require_close(qau::VacuumAdaptiveController::minimum_variance(0.0, 1.0, 0.0), 0.5,
                      1e-12, "Vacuum limit mismatch");
        bool rejected = false;
        try {
            (void)qau::VacuumAdaptiveController::optimize(1.2, sigma2);
        } catch (const std::invalid_argument&) {
            rejected = true;
        }
        require_true(rejected, "Invalid transmissivity must be rejected");

        std::cout << "eta=" << eta << " sigma2=" << sigma2
                  << " kappa=" << kappa << " r_star=" << policy.squeeze_parameter
                  << " V_star=" << policy.predicted_minimum_variance << "\n";
        std::cout << "=== Adaptive derivation harness: ALL TESTS GREEN ===\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Adaptive derivation harness failure: " << error.what() << "\n";
        return 1;
    }
}
