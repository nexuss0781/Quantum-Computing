#include "qau/phase_calibrator.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace qau {

void OnlineStatistics::add(double value) {
    if (!std::isfinite(value)) {
        throw std::invalid_argument("measurement value must be finite");
    }
    ++count;
    const double delta = value - mean;
    mean += delta / static_cast<double>(count);
    const double delta2 = value - mean;
    m2 += delta * delta2;
}

double OnlineStatistics::variance() const {
    if (count < 2) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return m2 / static_cast<double>(count - 1);
}

double OnlineStatistics::standard_deviation() const {
    const double value = variance();
    return value >= 0.0 ? std::sqrt(value) : std::numeric_limits<double>::quiet_NaN();
}

namespace {

std::vector<std::string> split_csv(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;
    while (std::getline(stream, field, ',')) {
        fields.push_back(field);
    }
    if (!line.empty() && line.back() == ',') {
        fields.emplace_back();
    }
    return fields;
}

double reduction(double baseline, double candidate) {
    if (!(baseline > 0.0) || !(candidate >= 0.0)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return (baseline - candidate) / baseline;
}

double reduction_db(double baseline, double candidate) {
    if (!(baseline > 0.0) || !(candidate > 0.0)) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    return 10.0 * std::log10(baseline / candidate);
}

void write_json_number(std::ofstream& output, const char* key, double value, bool comma = true) {
    output << "  \"" << key << "\": ";
    if (std::isfinite(value)) {
        output << std::setprecision(17) << value;
    } else {
        output << "null";
    }
    if (comma) {
        output << ',';
    }
    output << '\n';
}

}  // namespace

void PhaseCalibrator::add(const PhaseObservation& observation) {
    if (observation.phase_index <= 0) {
        throw std::invalid_argument("phase_index must be positive");
    }
    if (observation.run_id <= 0) {
        throw std::invalid_argument("run_id must be positive");
    }
    if (observation.sample_index < 0) {
        throw std::invalid_argument("sample_index must be non-negative");
    }
    if (!std::isfinite(observation.phase_rad)) {
        throw std::invalid_argument("phase_rad must be finite");
    }

    auto [iterator, inserted] = summaries_.try_emplace(observation.phase_index);
    PhaseSummary& summary = iterator->second;
    if (inserted) {
        summary.run_id = observation.run_id;
        summary.phase_index = observation.phase_index;
        summary.phase_rad = observation.phase_rad;
        summary.channel = observation.channel;
    } else {
        if (summary.run_id != observation.run_id || summary.channel != observation.channel) {
            throw std::invalid_argument("a calibration input must contain one channel and stable phase metadata");
        }
    }

    summary.all.add(observation.value);
    if ((observation.sample_index % 2) == 0) {
        summary.train.add(observation.value);
    } else {
        summary.validation.add(observation.value);
    }
    ++observation_count_;
}

PhaseCalibrator PhaseCalibrator::from_csv(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("cannot open calibration CSV: " + path);
    }

    PhaseCalibrator calibrator;
    std::string line;
    std::size_t line_number = 0;
    while (std::getline(input, line)) {
        ++line_number;
        if (line.empty()) {
            continue;
        }
        const auto fields = split_csv(line);
        if (fields.size() < 6 || fields[0] == "run_id") {
            continue;
        }
        try {
            PhaseObservation observation;
            observation.run_id = std::stoi(fields[0]);
            observation.phase_index = std::stoi(fields[1]);
            observation.phase_rad = std::stod(fields[2]);
            observation.channel = std::stoi(fields[3]);
            observation.sample_index = std::stoll(fields[4]);
            observation.value = std::stod(fields[5]);
            calibrator.add(observation);
        } catch (const std::exception& error) {
            throw std::runtime_error("invalid calibration CSV at line " + std::to_string(line_number) + ": " + error.what());
        }
    }
    if (calibrator.summaries_.empty()) {
        throw std::runtime_error("calibration CSV contains no observations");
    }
    return calibrator;
}

CalibrationResult PhaseCalibrator::analyze(int default_phase_index, SelectionPolicy selection_policy) const {
    if (summaries_.size() < 2) {
        throw std::runtime_error("at least two phase settings are required");
    }
    const auto default_iterator = summaries_.find(default_phase_index);
    if (default_iterator == summaries_.end()) {
        throw std::runtime_error("default phase index is absent from calibration data");
    }

    auto recommended_iterator = summaries_.end();
    for (auto iterator = summaries_.begin(); iterator != summaries_.end(); ++iterator) {
        const double variance = selection_policy == SelectionPolicy::AllObservations
            ? iterator->second.all.variance()
            : iterator->second.train.variance();
        if (!std::isfinite(variance)) {
            continue;
        }
        const double recommended_variance = recommended_iterator == summaries_.end()
            ? std::numeric_limits<double>::infinity()
            : (selection_policy == SelectionPolicy::AllObservations
                ? recommended_iterator->second.all.variance()
                : recommended_iterator->second.train.variance());
        if (recommended_iterator == summaries_.end() || variance < recommended_variance) {
            recommended_iterator = iterator;
        }
    }
    if (recommended_iterator == summaries_.end()) {
        throw std::runtime_error("no phase has enough training samples");
    }

    const PhaseSummary& baseline = default_iterator->second;
    const PhaseSummary& recommended = recommended_iterator->second;
    CalibrationResult result;
    result.selection_policy = selection_policy;
    result.default_phase_index = default_phase_index;
    result.recommended_phase_index = recommended.phase_index;
    result.recommended_run_id = recommended.run_id;
    result.default_phase_rad = baseline.phase_rad;
    result.recommended_phase_rad = recommended.phase_rad;
    result.default_all_variance = baseline.all.variance();
    result.recommended_all_variance = recommended.all.variance();
    result.default_train_variance = baseline.train.variance();
    result.recommended_train_variance = recommended.train.variance();
    result.default_validation_variance = baseline.validation.variance();
    result.recommended_validation_variance = recommended.validation.variance();
    result.observed_relative_reduction = reduction(result.default_all_variance, result.recommended_all_variance);
    result.observed_reduction_db = reduction_db(result.default_all_variance, result.recommended_all_variance);
    result.training_relative_reduction = reduction(result.default_train_variance, result.recommended_train_variance);
    result.validation_relative_reduction = reduction(result.default_validation_variance, result.recommended_validation_variance);
    result.training_reduction_db = reduction_db(result.default_train_variance, result.recommended_train_variance);
    result.validation_reduction_db = reduction_db(result.default_validation_variance, result.recommended_validation_variance);
    result.validation_gate_passed = std::isfinite(result.validation_relative_reduction) && result.validation_relative_reduction > 0.0;
    result.observation_count = observation_count_;
    result.phase_count = summaries_.size();
    return result;
}

void PhaseCalibrator::write_summary_csv(const std::string& path) const {
    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("cannot write calibration summary: " + path);
    }
    output << "phase_index,run_id,phase_rad,channel,all_count,all_mean,all_variance,train_count,train_variance,validation_count,validation_variance\n";
    output << std::setprecision(17);
    for (const auto& [phase_index, summary] : summaries_) {
        output << phase_index << ',' << summary.run_id << ',' << summary.phase_rad << ',' << summary.channel << ','
               << summary.all.count << ',' << summary.all.mean << ',' << summary.all.variance() << ','
               << summary.train.count << ',' << summary.train.variance() << ','
               << summary.validation.count << ',' << summary.validation.variance() << '\n';
    }
}

void PhaseCalibrator::write_result_json(const std::string& path, const CalibrationResult& result) const {
    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("cannot write calibration result: " + path);
    }
    output << "{\n";
    output << "  \"selection_policy\": \"" << (result.selection_policy == SelectionPolicy::AllObservations ? "all_observations" : "training_split") << "\",\n";
    output << "  \"default_phase_index\": " << result.default_phase_index << ",\n";
    output << "  \"recommended_phase_index\": " << result.recommended_phase_index << ",\n";
    output << "  \"recommended_run_id\": " << result.recommended_run_id << ",\n";
    write_json_number(output, "default_phase_rad", result.default_phase_rad);
    write_json_number(output, "recommended_phase_rad", result.recommended_phase_rad);
    write_json_number(output, "default_all_variance", result.default_all_variance);
    write_json_number(output, "recommended_all_variance", result.recommended_all_variance);
    write_json_number(output, "default_train_variance", result.default_train_variance);
    write_json_number(output, "recommended_train_variance", result.recommended_train_variance);
    write_json_number(output, "default_validation_variance", result.default_validation_variance);
    write_json_number(output, "recommended_validation_variance", result.recommended_validation_variance);
    write_json_number(output, "observed_relative_reduction", result.observed_relative_reduction);
    write_json_number(output, "observed_reduction_db", result.observed_reduction_db);
    write_json_number(output, "training_relative_reduction", result.training_relative_reduction);
    write_json_number(output, "validation_relative_reduction", result.validation_relative_reduction);
    write_json_number(output, "training_reduction_db", result.training_reduction_db);
    write_json_number(output, "validation_reduction_db", result.validation_reduction_db);
    output << "  \"validation_gate_passed\": " << (result.validation_gate_passed ? "true" : "false") << ",\n";
    output << "  \"observation_count\": " << result.observation_count << ",\n";
    output << "  \"phase_count\": " << result.phase_count << '\n';
    output << "}\n";
}

}  // namespace qau
