#include "qau/phase_calibrator.hpp"

#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

struct Options {
    std::string input;
    std::string summary;
    std::string result;
    int default_phase_index{1};
    qau::SelectionPolicy selection_policy{qau::SelectionPolicy::AllObservations};
};

void usage(const char* executable) {
    std::cerr << "Usage: " << executable
              << " --input measured.csv --summary summary.csv --result result.json"
              << " [--default-phase-index 1] [--selection all|train]\n";
}

Options parse_options(int argc, char** argv) {
    Options options;
    for (int index = 1; index < argc; ++index) {
        const std::string argument(argv[index]);
        auto value = [&](const char* name) -> std::string {
            if (index + 1 >= argc) {
                throw std::invalid_argument(std::string("missing value for ") + name);
            }
            return argv[++index];
        };
        if (argument == "--input") {
            options.input = value("--input");
        } else if (argument == "--summary") {
            options.summary = value("--summary");
        } else if (argument == "--result") {
            options.result = value("--result");
        } else if (argument == "--default-phase-index") {
            options.default_phase_index = std::stoi(value("--default-phase-index"));
        } else if (argument == "--selection") {
            const std::string selection = value("--selection");
            if (selection == "all") {
                options.selection_policy = qau::SelectionPolicy::AllObservations;
            } else if (selection == "train") {
                options.selection_policy = qau::SelectionPolicy::TrainingSplit;
            } else {
                throw std::invalid_argument("--selection must be all or train");
            }
        } else if (argument == "--help" || argument == "-h") {
            usage(argv[0]);
            std::exit(0);
        } else {
            throw std::invalid_argument("unknown option: " + argument);
        }
    }
    if (options.input.empty() || options.summary.empty() || options.result.empty()) {
        usage(argv[0]);
        throw std::invalid_argument("--input, --summary, and --result are required");
    }
    return options;
}

}  // namespace

int main(int argc, char** argv) {
    try {
        const Options options = parse_options(argc, argv);
        const qau::PhaseCalibrator calibrator = qau::PhaseCalibrator::from_csv(options.input);
        const qau::CalibrationResult result = calibrator.analyze(options.default_phase_index, options.selection_policy);
        calibrator.write_summary_csv(options.summary);
        calibrator.write_result_json(options.result, result);

        std::cout << std::setprecision(17);
        std::cout << "QAU MEASURED PHASE CALIBRATION\n";
        std::cout << "selection_policy=" << (result.selection_policy == qau::SelectionPolicy::AllObservations ? "all_observations" : "training_split") << '\n';
        std::cout << "observations=" << result.observation_count << " phases=" << result.phase_count << '\n';
        std::cout << "default_phase_index=" << result.default_phase_index
                  << " default_phase_rad=" << result.default_phase_rad << '\n';
        std::cout << "recommended_phase_index=" << result.recommended_phase_index
                  << " recommended_run_id=" << result.recommended_run_id
                  << " recommended_phase_rad=" << result.recommended_phase_rad << '\n';
        std::cout << "observed_relative_variance_reduction=" << result.observed_relative_reduction
                  << " observed_reduction_db=" << result.observed_reduction_db << '\n';
        std::cout << "training_relative_variance_reduction=" << result.training_relative_reduction
                  << " training_reduction_db=" << result.training_reduction_db << '\n';
        std::cout << "validation_relative_variance_reduction=" << result.validation_relative_reduction
                  << " validation_reduction_db=" << result.validation_reduction_db << '\n';
        std::cout << "validation_gate=" << (result.validation_gate_passed ? "PASS" : "FAIL") << '\n';
        return result.validation_gate_passed ? 0 : 2;
    } catch (const std::exception& error) {
        std::cerr << "qau_phase_calibrate: " << error.what() << '\n';
        return 1;
    }
}
