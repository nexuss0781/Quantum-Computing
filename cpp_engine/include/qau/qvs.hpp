#ifndef QAU_QVS_HPP
#define QAU_QVS_HPP

#include "asc.hpp"
#include "rpw.hpp"
#include "ncb.hpp"
#include <unordered_map>
#include <memory>
#include <random>

namespace qau {

class QVS {
private:
    int next_id = 0;
    std::mt19937 rng{1337};

public:
    std::unordered_map<int, std::shared_ptr<ASC>> ascs;

    int create_asc(int size) {
        int id = next_id++;
        ascs[id] = std::make_shared<ASC>(size);
        return id;
    }

    void SUPERPOSE(int asc_id, const std::vector<BasisState>& states) {
        if (ascs.find(asc_id) == ascs.end()) return;
        auto& asc = ascs[asc_id];
        asc->amplitudes.clear();
        double amp = 1.0 / std::sqrt(states.size());
        for (const auto& st : states) {
            asc->amplitudes[st] = {amp, 0.0};
        }
    }

    void WEAVE(int asc_id, const BasisState* basis_state, double theta) {
        if (ascs.find(asc_id) == ascs.end()) return;
        auto& asc = ascs[asc_id];
        if (basis_state) {
            RPW::apply_phase(*asc, *basis_state, theta);
        } else {
            // Apply phase to all states
            for (auto& pair : asc->amplitudes) {
                pair.second *= std::polar(1.0, theta);
            }
        }
    }

    int BOND(int id_a, int id_b, const std::string& correlation_type) {
        if (ascs.find(id_a) == ascs.end() || ascs.find(id_b) == ascs.end()) {
            throw std::runtime_error("ASC IDs not found for bonding.");
        }
        if (correlation_type == "bell") {
            auto bonded = NCB::forge_bell_state(ascs[id_a], ascs[id_b]);
            int bonded_id = next_id++;
            ascs[bonded_id] = bonded;
            return bonded_id;
        }
        throw std::runtime_error("Unsupported correlation type.");
    }

    BasisState COLLAPSE(int asc_id) {
        if (ascs.find(asc_id) == ascs.end()) {
            throw std::runtime_error("ASC ID not found for collapse.");
        }
        auto& asc = ascs[asc_id];
        std::vector<BasisState> states;
        std::vector<double> weights;
        for (const auto& pair : asc->amplitudes) {
            states.push_back(pair.first);
            weights.push_back(std::norm(pair.second));
        }

        std::discrete_distribution<int> dist(weights.begin(), weights.end());
        int chosen_idx = dist(rng);
        BasisState result = states[chosen_idx];

        // Collapse to deterministic state
        asc->amplitudes.clear();
        asc->amplitudes[result] = {1.0, 0.0};
        return result;
    }
};

} // namespace qau

#endif // QAU_QVS_HPP
