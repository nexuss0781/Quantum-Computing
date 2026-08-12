#ifndef QAU_VACUUM_MODE_HPP
#define QAU_VACUUM_MODE_HPP

#include "qau/quantum_dense.hpp"

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

namespace qau {

/** Dimensionless quadrature pair for a single bosonic field mode. */
struct QuadratureMoments {
    double x_mean = 0.0;
    double p_mean = 0.0;
    double x_variance = 0.5;
    double p_variance = 0.5;
};

/** Result of scanning a phase-sensitive vacuum-noise measurement. */
struct VacuumNoiseScan {
    double minimum_variance = 0.5;
    double maximum_variance = 0.5;
    double minimum_angle = 0.0;
    double maximum_angle = 0.0;
    double improvement_db = 0.0;
};

/**
 * A truncated single bosonic field mode whose |0> state is the QAU vacuum unit.
 *
 * Basis index n is the occupation number. The mode uses dimensionless
 * quadratures X=(a+a^dagger)/sqrt(2), P=(a-a^dagger)/(i*sqrt(2)), so the
 * vacuum reference has Var(X)=Var(P)=1/2 and uncertainty product 1/4.
 * Truncation is explicit and every result exposes a residual/error bound.
 */
class VacuumMode {
public:
    VacuumMode(std::size_t cutoff, double angular_frequency = 1.0, double hbar = 1.0)
        : cutoff_(cutoff), angular_frequency_(angular_frequency), hbar_(hbar), amplitudes_(cutoff, {0.0, 0.0}) {
        if (cutoff_ < 2) throw std::invalid_argument("VacuumMode cutoff must be at least 2.");
        if (!(angular_frequency_ > 0.0) || !(hbar_ > 0.0) || !std::isfinite(angular_frequency_) || !std::isfinite(hbar_)) {
            throw std::invalid_argument("VacuumMode frequency and hbar must be finite and positive.");
        }
        amplitudes_[0] = {1.0, 0.0};
    }

    static VacuumMode vacuum(std::size_t cutoff, double angular_frequency = 1.0, double hbar = 1.0) {
        return VacuumMode(cutoff, angular_frequency, hbar);
    }

    static VacuumMode coherent(std::size_t cutoff, Complex alpha,
                               double angular_frequency = 1.0, double hbar = 1.0) {
        VacuumMode mode(cutoff, angular_frequency, hbar);
        mode.amplitudes_.assign(cutoff, {0.0, 0.0});
        const double magnitude_squared = std::norm(alpha);
        mode.amplitudes_[0] = std::exp(-magnitude_squared / 2.0);
        double factorial_root = 1.0;
        for (std::size_t n = 1; n < cutoff; ++n) {
            factorial_root *= std::sqrt(static_cast<double>(n));
            mode.amplitudes_[n] = mode.amplitudes_[n - 1] * alpha / std::sqrt(static_cast<double>(n));
        }
        mode.normalize();
        return mode;
    }

    /** Create a squeezed vacuum from the exact even-Fock expansion, then renormalize for cutoff. */
    static VacuumMode squeezed_vacuum(std::size_t cutoff, double r, double squeeze_phase = 0.0,
                                      double angular_frequency = 1.0, double hbar = 1.0) {
        if (!std::isfinite(r) || !std::isfinite(squeeze_phase)) {
            throw std::invalid_argument("Squeezing parameters must be finite.");
        }
        VacuumMode mode(cutoff, angular_frequency, hbar);
        mode.amplitudes_.assign(cutoff, {0.0, 0.0});
        const double t = std::tanh(r);
        const Complex z = -std::polar(t, squeeze_phase);
        const double normalization = 1.0 / std::sqrt(std::cosh(r));
        mode.amplitudes_[0] = normalization;
        Complex coefficient = normalization;
        for (std::size_t k = 1; 2 * k < cutoff; ++k) {
            const double numerator = std::sqrt(static_cast<double>((2 * k - 1) * (2 * k)));
            const double denominator = 2.0 * static_cast<double>(k);
            coefficient *= z * (numerator / denominator);
            mode.amplitudes_[2 * k] = coefficient;
        }
        mode.normalize();
        return mode;
    }

    std::size_t cutoff() const noexcept { return cutoff_; }
    double angular_frequency() const noexcept { return angular_frequency_; }
    double hbar() const noexcept { return hbar_; }
    const StateVector& amplitudes() const noexcept { return amplitudes_; }
    StateVector& amplitudes() noexcept { return amplitudes_; }

    void normalize() {
        const double norm_squared = norm_squared_value();
        if (norm_squared <= std::numeric_limits<double>::epsilon()) {
            throw std::runtime_error("Cannot normalize a zero vacuum-mode state.");
        }
        const double norm = std::sqrt(norm_squared);
        for (auto& amplitude : amplitudes_) amplitude /= norm;
    }

    double norm_squared_value() const {
        double result = 0.0;
        for (const auto& amplitude : amplitudes_) result += std::norm(amplitude);
        return result;
    }

    /** Probability of occupation n. */
    double occupation_probability(std::size_t n) const {
        if (n >= cutoff_) throw std::out_of_range("Occupation index is out of range.");
        return std::norm(amplitudes_[n]);
    }

    /** <a> using the truncated Fock basis. */
    Complex expectation_annihilation() const {
        Complex result{0.0, 0.0};
        for (std::size_t n = 0; n + 1 < cutoff_; ++n) {
            result += std::conj(amplitudes_[n]) * std::sqrt(static_cast<double>(n + 1)) * amplitudes_[n + 1];
        }
        return result;
    }

    /** <n> for the number operator a^dagger a. */
    double mean_occupation() const {
        double result = 0.0;
        for (std::size_t n = 0; n < cutoff_; ++n) result += static_cast<double>(n) * occupation_probability(n);
        return result;
    }

    /** <H> for H=hbar*omega*(n+1/2). */
    double mean_energy() const { return hbar_ * angular_frequency_ * (mean_occupation() + 0.5); }

    double quadrature_mean(double angle) const {
        const Complex a = expectation_annihilation();
        return std::sqrt(2.0) * (a * std::polar(1.0, -angle)).real();
    }

    double quadrature_variance(double angle) const {
        const Complex a = expectation_annihilation();
        const double mean = quadrature_mean(angle);
        const double n = mean_occupation();
        Complex aa{0.0, 0.0};
        for (std::size_t index = 0; index + 2 < cutoff_; ++index) {
            aa += std::conj(amplitudes_[index]) *
                  std::sqrt(static_cast<double>((index + 1) * (index + 2))) * amplitudes_[index + 2];
        }
        const Complex phase = std::polar(1.0, -2.0 * angle);
        const double second_moment = 0.5 * (2.0 * n + 1.0 + 2.0 * (phase * aa).real());
        return std::max(0.0, second_moment - mean * mean);
    }

    QuadratureMoments quadrature_moments() const {
        return {quadrature_mean(0.0), quadrature_mean(M_PI / 2.0),
                quadrature_variance(0.0), quadrature_variance(M_PI / 2.0)};
    }

    double uncertainty_product() const {
        const auto moments = quadrature_moments();
        return moments.x_variance * moments.p_variance;
    }

    /** Phase-space rotation: |n> -> exp(-i*n*theta)|n>. */
    void phase_rotate(double theta) {
        if (!std::isfinite(theta)) throw std::invalid_argument("Phase angle must be finite.");
        for (std::size_t n = 0; n < cutoff_; ++n) {
            amplitudes_[n] *= std::polar(1.0, -static_cast<double>(n) * theta);
        }
    }

    /** Scan the homodyne quadrature angle over [0, pi). */
    VacuumNoiseScan scan_quadratures(std::size_t samples = 720) const {
        if (samples < 2) throw std::invalid_argument("Quadrature scan needs at least two samples.");
        VacuumNoiseScan result;
        result.minimum_variance = std::numeric_limits<double>::infinity();
        result.maximum_variance = -std::numeric_limits<double>::infinity();
        for (std::size_t index = 0; index < samples; ++index) {
            const double angle = M_PI * static_cast<double>(index) / static_cast<double>(samples);
            const double variance = quadrature_variance(angle);
            if (variance < result.minimum_variance) {
                result.minimum_variance = variance;
                result.minimum_angle = angle;
            }
            if (variance > result.maximum_variance) {
                result.maximum_variance = variance;
                result.maximum_angle = angle;
            }
        }
        result.improvement_db = 10.0 * std::log10(0.5 / result.minimum_variance);
        return result;
    }

    /** Vacuum loss channel for a quadrature variance: eta*V + (1-eta)*V_vacuum. */
    static double lossy_variance(double variance, double transmissivity) {
        if (transmissivity < 0.0 || transmissivity > 1.0) {
            throw std::invalid_argument("Transmissivity must lie in [0,1].");
        }
        return transmissivity * variance + (1.0 - transmissivity) * 0.5;
    }

    /** Remaining truncation probability in the highest represented tail proxy. */
    double truncation_tail_proxy() const {
        return std::max(0.0, 1.0 - norm_squared_value());
    }

private:
    std::size_t cutoff_;
    double angular_frequency_;
    double hbar_;
    StateVector amplitudes_;
};

} // namespace qau

#endif // QAU_VACUUM_MODE_HPP
