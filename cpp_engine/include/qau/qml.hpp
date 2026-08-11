#ifndef QAU_QML_HPP
#define QAU_QML_HPP

#include "qvs.hpp"
#include <vector>
#include <functional>

namespace qau {

/**
 * Parameterized Quantum Circuit (PQC) for QML.
 */
struct ParameterizedGate {
    int target_id;
    double theta;
    double gradient = 0.0;
};

class QMLEngine {
public:
    std::vector<ParameterizedGate> parameters;

    void add_parameterized_weave(int target_id, double initial_theta) {
        parameters.push_back({target_id, initial_theta, 0.0});
    }

    /**
     * Compute gradients using the parameter-shift rule:
     * df/dtheta = (f(theta + pi/2) - f(theta - pi/2)) / 2
     */
    void compute_gradients(QVS& qvs, std::function<double(QVS&)> cost_function) {
        for (auto& param : parameters) {
            double original_theta = param.theta;

            // Shift forward
            param.theta = original_theta + M_PI / 2.0;
            qvs.WEAVE(param.target_id, nullptr, param.theta);
            double f_plus = cost_function(qvs);

            // Shift backward
            param.theta = original_theta - M_PI / 2.0;
            qvs.WEAVE(param.target_id, nullptr, param.theta);
            double f_minus = cost_function(qvs);

            // Restore and assign gradient
            param.theta = original_theta;
            qvs.WEAVE(param.target_id, nullptr, original_theta);
            param.gradient = (f_plus - f_minus) / 2.0;
        }
    }
};

} // namespace qau

#endif // QAU_QML_HPP
