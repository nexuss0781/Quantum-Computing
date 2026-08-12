#include "qau/phase_calibrator.hpp"

#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>

namespace {

void expect_close(double actual, double expected, double tolerance = 1e-12) {
    assert(std::isfinite(actual));
    assert(std::abs(actual - expected) <= tolerance);
}

void test_online_statistics() {
    qau::OnlineStatistics statistics;
    statistics.add(1.0);
    statistics.add(2.0);
    statistics.add(3.0);
    expect_close(statistics.mean, 2.0);
    expect_close(statistics.variance(), 1.0);
}

void test_csv_calibration() {
    const auto path = std::filesystem::temp_directory_path() / "qau_phase_calibrator_fixture.csv";
    {
        std::ofstream output(path);
        output << "run_id,phase_index,phase_rad,channel,sample_index,value\n";
        output << "11,1,0,0,0,1\n";
        output << "11,1,0,0,1,-1\n";
        output << "11,1,0,0,2,2\n";
        output << "11,1,0,0,3,-2\n";
        output << "12,2,0.5,0,0,0.5\n";
        output << "12,2,0.5,0,1,-0.5\n";
        output << "12,2,0.5,0,2,0.25\n";
        output << "12,2,0.5,0,3,-0.25\n";
    }

    const auto calibrator = qau::PhaseCalibrator::from_csv(path.string());
    const auto result = calibrator.analyze(1);
    assert(result.observation_count == 8);
    assert(result.phase_count == 2);
    assert(result.recommended_phase_index == 2);
    assert(result.recommended_run_id == 12);
    assert(result.validation_gate_passed);
    assert(result.validation_relative_reduction > 0.9);
    std::filesystem::remove(path);
}

void test_invalid_measurement() {
    qau::PhaseCalibrator calibrator;
    bool rejected = false;
    try {
        calibrator.add(qau::PhaseObservation{1, 1, 0.0, 0, 0, std::numeric_limits<double>::quiet_NaN()}
);
    } catch (const std::invalid_argument&) {
        rejected = true;
    }
    assert(rejected);
}

}  // namespace

int main() {
    test_online_statistics();
    test_csv_calibration();
    test_invalid_measurement();
    std::cout << "PhaseCalibratorTests: PASS\n";
    return 0;
}
