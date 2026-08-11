#ifndef QAU_SIMD_KERNELS_HPP
#define QAU_SIMD_KERNELS_HPP

#include <complex>
#include <vector>
#include <omp.h>

namespace qau {

class SIMDKernels {
public:
    /**
     * Parallel phase rotation using OpenMP and SIMD auto-vectorization.
     * Applies e^(i*theta) to an array of complex amplitudes.
     */
    static void apply_phase_parallel(std::complex<double>* amplitudes, size_t size, double theta) {
        std::complex<double> rotor = std::polar(1.0, theta);
        
        #pragma omp parallel for simd
        for (size_t i = 0; i < size; ++i) {
            amplitudes[i] *= rotor;
        }
    }

    /**
     * Parallel complex dot product for state vector operations.
     */
    static std::complex<double> dot_product_parallel(const std::complex<double>* a, const std::complex<double>* b, size_t size) {
        double real_sum = 0.0;
        double imag_sum = 0.0;

        #pragma omp parallel for reduction(+:real_sum, imag_sum)
        for (size_t i = 0; i < size; ++i) {
            std::complex<double> res = a[i] * std::conj(b[i]);
            real_sum += res.real();
            imag_sum += res.imag();
        }

        return {real_sum, imag_sum};
    }
};

} // namespace qau

#endif // QAU_SIMD_KERNELS_HPP
