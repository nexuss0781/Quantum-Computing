#include <iostream>
#include <cassert>
#include <cmath>
#include "qau/asc.hpp"
#include "qau/rpw.hpp"
#include "qau/ncb.hpp"
#include "qau/qvs.hpp"

void test_superposition() {
    std::cout << "Running C++ Superposition Test..." << std::endl;
    qau::ASC asc(1);
    assert(asc.size == 1);
    assert(asc.amplitudes.size() == 1);
    std::cout << "Superposition test passed." << std::endl;
}

void test_interference() {
    std::cout << "Running C++ Interference (RPW) Test..." << std::endl;
    qau::ASC asc(1);
    asc.amplitudes.clear();
    asc.amplitudes[{0}] = {1.0 / std::sqrt(2.0), 0.0};
    asc.amplitudes[{1}] = {1.0 / std::sqrt(2.0), 0.0};

    qau::RPW::apply_phase(asc, {1}, M_PI / 2.0);
    std::complex<double> expected = {0.0, 1.0 / std::sqrt(2.0)};
    auto val = asc.amplitudes[{1}];
    assert(std::abs(val.real() - expected.real()) < 1e-6);
    assert(std::abs(val.imag() - expected.imag()) < 1e-6);
    std::cout << "Interference test passed." << std::endl;
}

void test_entanglement() {
    std::cout << "Running C++ Entanglement (NCB / Bell State) Test..." << std::endl;
    qau::QVS qvs;
    int id_a = qvs.create_asc(1);
    int id_b = qvs.create_asc(1);

    int bonded_id = qvs.BOND(id_a, id_b, "bell");
    auto bonded_asc = qvs.ascs[bonded_id];
    assert(bonded_asc->size == 2);
    assert(bonded_asc->amplitudes.size() == 2);
    assert(bonded_asc->amplitudes.count({0, 0}) == 1);
    assert(bonded_asc->amplitudes.count({1, 1}) == 1);
    std::cout << "Entanglement test passed with Bell state amplitudes verified." << std::endl;
}

void test_collapse() {
    std::cout << "Running C++ Collapse Test..." << std::endl;
    qau::QVS qvs;
    int id = qvs.create_asc(1);
    qvs.SUPERPOSE(id, {{0}, {1}});
    auto result = qvs.COLLAPSE(id);
    assert(result.size() == 1);
    assert(qvs.ascs[id]->amplitudes.size() == 1);
    std::cout << "Collapse test passed with result: (" << result[0] << ")" << std::endl;
}

int main() {
    std::cout << "=== QAU C++ Engine Phase 1 Test Suite ===" << std::endl;
    try {
        test_superposition();
        test_interference();
        test_entanglement();
        test_collapse();
        std::cout << "=== All C++ Phase 1 Tests Passed Successfully! ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
