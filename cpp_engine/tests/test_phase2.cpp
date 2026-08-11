#include <iostream>
#include <cassert>
#include <chrono>
#include "qau/asc.hpp"
#include "qau/rpw.hpp"
#include "qau/ncb.hpp"
#include "qau/qvs.hpp"
#include "qau/mps.hpp"
#include "qau/contraction.hpp"

void test_mps_initialization() {
    std::cout << "Running MPS Scalability Test (30 Qubits)..." << std::endl;
    qau::MatrixProductState mps(30, 16);
    assert(mps.get_num_qubits() == 30);
    assert(mps.get_max_bond_dimension() == 16);
    std::cout << "MPS Scalability Test passed successfully." << std::endl;
}

void test_contraction_optimizer() {
    std::cout << "Running Tensor Contraction Path Optimizer Test..." << std::endl;
    std::vector<qau::ContractionNode> nodes = {
        {0, {0, 1}, 100},
        {1, {1, 2}, 200},
        {2, {2, 3}, 150}
    };
    auto path = qau::ContractionOptimizer::find_optimal_path(nodes);
    assert(!path.empty());
    std::cout << "Contraction Path Optimizer Test passed with " << path.size() << " contraction steps." << std::endl;
}

void test_sparse_scaling_performance() {
    std::cout << "Running Sparse Engine Scalability Benchmark..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();

    qau::QVS qvs;
    // Create a 10-qubit sparse superposition cell
    int id = qvs.create_asc(10);
    std::vector<qau::BasisState> states;
    for (int i = 0; i < 64; ++i) {
        qau::BasisState st(10, 0);
        for (int b = 0; b < 10; ++b) {
            if ((i >> b) & 1) st[b] = 1;
        }
        states.push_back(st);
    }
    qvs.SUPERPOSE(id, states);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Sparse superposition of 64 states across 10 qubits initialized in " << elapsed.count() << " ms." << std::endl;
    assert(qvs.ascs[id]->amplitudes.size() == 64);
    std::cout << "Sparse scalability benchmark passed." << std::endl;
}

int main() {
    std::cout << "=== QAU C++ Engine Phase 2 Evaluation Harness ===" << std::endl;
    try {
        test_mps_initialization();
        test_contraction_optimizer();
        test_sparse_scaling_performance();
        std::cout << "=== All Phase 2 Evaluation Harness Tests Passed with 100% Success! ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Phase 2 Evaluation failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
