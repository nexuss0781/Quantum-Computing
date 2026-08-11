#include <iostream>
#include <vector>
#include <complex>
#include <chrono>
#include <cassert>
#include "qau/simd_kernels.hpp"

void benchmark_simd_phase_rotation() {
    std::cout << "Running SIMD Phase Rotation Benchmark..." << std::endl;
    const size_t N = 1 << 22; // 4 million states
    std::vector<std::complex<double>> amplitudes(N, {1.0, 0.0});
    double theta = 0.5;

    auto start = std::chrono::high_resolution_clock::now();
    qau::SIMDKernels::apply_phase_parallel(amplitudes.data(), N, theta);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Rotated " << N << " states in " << elapsed.count() << " ms." << std::endl;
    
    // Verify first element
    std::complex<double> expected = std::polar(1.0, theta);
    assert(std::abs(amplitudes[0] - expected) < 1e-9);
    std::cout << "SIMD Phase Rotation Test: GREEN" << std::endl;
}

void benchmark_parallel_dot_product() {
    std::cout << "Running Parallel Dot Product Benchmark..." << std::endl;
    const size_t N = 1 << 22;
    std::vector<std::complex<double>> a(N, {1.0, 0.0});
    std::vector<std::complex<double>> b(N, {1.0, 0.0});

    auto start = std::chrono::high_resolution_clock::now();
    std::complex<double> result = qau::SIMDKernels::dot_product_parallel(a.data(), b.data(), N);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Computed dot product of " << N << " elements in " << elapsed.count() << " ms." << std::endl;
    assert(std::abs(result.real() - (double)N) < 1e-9);
    std::cout << "Parallel Dot Product Test: GREEN" << std::endl;
}

int main() {
    std::cout << "=== QAU C++ Engine Phase 3 Performance Harness ===" << std::endl;
    try {
        benchmark_simd_phase_rotation();
        benchmark_parallel_dot_product();
        std::cout << "=== All Phase 3 Performance Tests: ULTIMATE GREEN SUCCESS! ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Phase 3 Performance Harness Failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
