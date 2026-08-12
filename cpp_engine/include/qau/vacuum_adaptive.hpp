#ifndef QAU_VACUUM_ADAPTIVE_HPP
#define QAU_VACUUM_ADAPTIVE_HPP

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace qau {

struct AdaptiveVacuumPolicy {
    double transmissivity = 1.0;
    double phase_diffusion_variance = 0.0;
    double squeeze_parameter = 0.0;
    double predicted_minimum_variance = 0.5;
    double coherence_factor = 1.0;
    bool finite_optimum = false;
};

/**
 * Analytic Gaussian single-mode controller for a vacuum-referenced sensor.
 *
 * This class is a model-level controller: it computes the optimal squeezing
 * parameter for declared transmissivity and phase-diffusion variance. It does
 * not estimate hardware parameters and does not claim a physical experiment.
 */
class VacuumAdaptiveController {
public:
    static double coherence_factor(double phase_diffusion_variance) {
        validate_phase_noise(phase_diffusion_variance);
        return std::exp(-2.0 * phase_diffusion_variance);
    }

    /** V_min(r) after loss and Gaussian phase diffusion. */
    static double minimum_variance(double squeeze_parameter,
                                   double transmissivity,
                                   double phase_diffusion_variance) {
        validate_transmissivity(transmissivity);
        validate_phase_noise(phase_diffusion_variance);
        if (!std::isfinite(squeeze_parameter) || squeeze_parameter < 0.0) {
            throw std::invalid_argument("Squeezing parameter must be finite and nonnegative.");
        }
        const double kappa = std::exp(-2.0 * phase_diffusion_variance);
        const double ch = std::cosh(2.0 * squeeze_parameter);
        const double sh = std::sinh(2.0 * squeeze_parameter);
        return 0.5 * (1.0 - transmissivity) +
               0.5 * transmissivity * (ch - kappa * sh);
    }

    /**
     * Optimal r for nonzero phase diffusion. When phase diffusion is zero,
     * the ideal loss-only model has no finite optimum; the caller must impose
     * a hardware or truncation bound instead.
     */
    static AdaptiveVacuumPolicy optimize(double transmissivity,
                                         double phase_diffusion_variance) {
        validate_transmissivity(transmissivity);
        validate_phase_noise(phase_diffusion_variance);
        AdaptiveVacuumPolicy result;
        result.transmissivity = transmissivity;
        result.phase_diffusion_variance = phase_diffusion_variance;
        result.coherence_factor = coherence_factor(phase_diffusion_variance);
        if (phase_diffusion_variance <= 0.0) {
            result.finite_optimum = false;
            result.squeeze_parameter = 0.0;
            result.predicted_minimum_variance = 0.5;
            return result;
        }
        result.finite_optimum = true;
        result.squeeze_parameter = 0.5 * std::atanh(result.coherence_factor);
        result.predicted_minimum_variance =
            0.5 * (1.0 - transmissivity) +
            0.5 * transmissivity * std::sqrt(1.0 - result.coherence_factor * result.coherence_factor);
        return result;
    }

    /** Numerically stable central derivative of V_min at r. */
    static double derivative(double squeeze_parameter,
                             double transmissivity,
                             double phase_diffusion_variance,
                             double step = 1e-6) {
        if (!(step > 0.0) || !std::isfinite(step)) throw std::invalid_argument("Step must be positive.");
        const double lower = std::max(0.0, squeeze_parameter - step);
        const double upper = squeeze_parameter + step;
        return (minimum_variance(upper, transmissivity, phase_diffusion_variance) -
                minimum_variance(lower, transmissivity, phase_diffusion_variance)) /
               (upper - lower);
    }

    /** Variance at an angle relative to the optimal squeezed axis. */
    static double quadrature_variance(double squeeze_parameter,
                                      double angle,
                                      double transmissivity,
                                      double phase_diffusion_variance) {
        validate_transmissivity(transmissivity);
        validate_phase_noise(phase_diffusion_variance);
        if (!std::isfinite(squeeze_parameter) || squeeze_parameter < 0.0 || !std::isfinite(angle)) {
            throw std::invalid_argument("Invalid adaptive covariance parameter.");
        }
        const double kappa = std::exp(-2.0 * phase_diffusion_variance);
        const double c2 = std::cos(2.0 * angle);
        const double ch = std::cosh(2.0 * squeeze_parameter);
        const double sh = std::sinh(2.0 * squeeze_parameter);
        const double anisotropy = kappa * sh * c2;
        return 0.5 * (1.0 - transmissivity) +
               0.5 * transmissivity * (ch - anisotropy);
    }

private:
    static void validate_transmissivity(double value) {
        if (!std::isfinite(value) || value < 0.0 || value > 1.0) {
            throw std::invalid_argument("Transmissivity must be finite and in [0,1].");
        }
    }
    static void validate_phase_noise(double value) {
        if (!std::isfinite(value) || value < 0.0) {
            throw std::invalid_argument("Phase-diffusion variance must be finite and nonnegative.");
        }
    }
};

} // namespace qau

#endif // QAU_VACUUM_ADAPTIVE_HPP
