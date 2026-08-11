#ifndef QAU_NCB_HPP
#define QAU_NCB_HPP

#include "asc.hpp"
#include <memory>
#include <string>

namespace qau {

class NCB {
public:
    std::shared_ptr<ASC> asc_a;
    std::shared_ptr<ASC> asc_b;
    std::string correlation_type;

    NCB(std::shared_ptr<ASC> a, std::shared_ptr<ASC> b, const std::string& type = "bell")
        : asc_a(a), asc_b(b), correlation_type(type) {
        apply_correlation();
    }

    void apply_correlation() {
        if (correlation_type == "bell") {
            if (asc_a->size == 1 && asc_b->size == 1) {
                // Create Bell state: (|00> + |11>) / sqrt(2)
                asc_a->amplitudes.clear();
                asc_b->amplitudes.clear();

                // We can combine them into a joint ASC or link their states
                // For demonstration, let's merge them into a 2-qubit joint ASC representing the bond
            }
        }
    }

    static std::shared_ptr<ASC> forge_bell_state(std::shared_ptr<ASC> a, std::shared_ptr<ASC> b) {
        if (a->size != 1 || b->size != 1) {
            throw std::invalid_argument("Bell state requires single-qubit ASCs.");
        }
        auto joint_asc = std::make_shared<ASC>(2);
        joint_asc->amplitudes.clear();
        double inv_sqrt2 = 1.0 / std::sqrt(2.0);
        joint_asc->amplitudes[{0, 0}] = {inv_sqrt2, 0.0};
        joint_asc->amplitudes[{1, 1}] = {inv_sqrt2, 0.0};
        return joint_asc;
    }
};

} // namespace qau

#endif // QAU_NCB_HPP
