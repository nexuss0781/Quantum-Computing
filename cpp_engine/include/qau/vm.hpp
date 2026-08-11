#ifndef QAU_VM_HPP
#define QAU_VM_HPP

#include "qvs.hpp"
#include "qasm.hpp"
#include <iostream>

namespace qau {

class BytecodeVM {
public:
    static void execute(QVS& qvs, const QASMRProgram& program) {
        for (const auto& inst : program.instructions) {
            switch (inst.opcode) {
                case OpCode::SUPERPOSE:
                    qvs.SUPERPOSE(inst.target_ids[0], inst.states);
                    break;
                case OpCode::WEAVE:
                    if (inst.states.empty()) {
                        qvs.WEAVE(inst.target_ids[0], nullptr, inst.params[0]);
                    } else {
                        qvs.WEAVE(inst.target_ids[0], &inst.states[0], inst.params[0]);
                    }
                    break;
                case OpCode::BOND:
                    qvs.BOND(inst.target_ids[0], inst.target_ids[1], inst.metadata);
                    break;
                case OpCode::COLLAPSE:
                    qvs.COLLAPSE(inst.target_ids[0]);
                    break;
                default:
                    std::cerr << "Unknown OpCode in VM execution." << std::endl;
            }
        }
    }
};

} // namespace qau

#endif // QAU_VM_HPP
