#pragma once

#include <cstddef>
#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace qau {

struct OnlineStatistics {
    std::size_t count{0};
    double mean{0.0};
    double m2{0.0};

    void add(double value);
    double variance() const;
    double standard_deviation() const;
};

struct PhaseObservation {
    int run_id{0};
    int phase_index{0};
    double phase_rad{0.0};
    int channel{0};
    std::int64_t sample_index{0};
    double value{0.0};
};

struct PhaseSummary {
    int run_id{0};
    int phase_index{0};
    double phase_rad{0.0};
    int channel{0};
    OnlineStatistics all;
    OnlineStatistics train;
    OnlineStatistics validation;
};

enum class SelectionPolicy {
    AllObservations,
    TrainingSplit,
};

struct CalibrationResult {
    SelectionPolicy selection_policy{SelectionPolicy::AllObservations};
    int default_phase_index{0};
    int recommended_phase_index{0};
    int recommended_run_id{0};
    double default_phase_rad{0.0};
    double recommended_phase_rad{0.0};
    double default_all_variance{0.0};
    double recommended_all_variance{0.0};
    double default_train_variance{0.0};
    double recommended_train_variance{0.0};
    double default_validation_variance{0.0};
    double recommended_validation_variance{0.0};
    double observed_relative_reduction{0.0};
    double observed_reduction_db{0.0};
    double training_relative_reduction{0.0};
    double validation_relative_reduction{0.0};
    double training_reduction_db{0.0};
    double validation_reduction_db{0.0};
    bool validation_gate_passed{false};
    std::size_t observation_count{0};
    std::size_t phase_count{0};
};

class PhaseCalibrator {
public:
    void add(const PhaseObservation& observation);

    static PhaseCalibrator from_csv(const std::string& path);

    const std::map<int, PhaseSummary>& summaries() const noexcept { return summaries_; }

    CalibrationResult analyze(
        int default_phase_index,
        SelectionPolicy selection_policy = SelectionPolicy::AllObservations) const;

    void write_summary_csv(const std::string& path) const;
    void write_result_json(const std::string& path, const CalibrationResult& result) const;

private:
    std::map<int, PhaseSummary> summaries_;
    std::size_t observation_count_{0};
};

}  // namespace qau
