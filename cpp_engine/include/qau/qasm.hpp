#ifndef QAU_QASM_HPP
#define QAU_QASM_HPP

#include <string>
#include <vector>
#include <variant>

namespace qau {

enum class OpCode {
    SUPERPOSE,
    WEAVE,
    BOND,
    ROTATE,
    COLLAPSE,
    TENSOR
};

struct Instruction {
    OpCode opcode;
    std::vector<int> target_ids;
    std::vector<double> params;
    std::vector<std::vector<int>> states;
    std::string metadata;
};

class QASMRProgram {
public:
    std::vector<Instruction> instructions;

    void add(Instruction inst) {
        instructions.push_back(std::move(inst));
    }

    void optimize() {
        if (instructions.empty()) return;
        
        std::vector<Instruction> optimized;
        for (auto& inst : instructions) {
            if (!optimized.empty() && 
                optimized.back().opcode == OpCode::WEAVE && 
                inst.opcode == OpCode::WEAVE &&
                optimized.back().target_ids == inst.target_ids &&
                optimized.back().states.empty() && inst.states.empty()) {
                // Fuse consecutive global weaves
                optimized.back().params[0] += inst.params[0];
            } else {
                optimized.push_back(inst);
            }
        }
        instructions = std::move(optimized);
    }
};

} // namespace qau

#endif // QAU_QASM_HPP
