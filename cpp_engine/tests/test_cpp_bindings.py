import sys
import os

# Add the build directory to sys.path to find the compiled module
sys.path.append(os.path.join(os.path.dirname(__file__), '../build'))

import qau_cpp

def test_qvs_bindings():
    print("Testing QVS C++ Bindings from Python...")
    qvs = qau_cpp.QVS()
    asc_id = qvs.create_asc(1)
    qvs.SUPERPOSE(asc_id, [[0], [1]])
    qvs.WEAVE(asc_id, 3.14159 / 2.0)
    result = qvs.COLLAPSE(asc_id)
    print(f"Collapse result: {result}")
    assert len(result) == 1
    print("QVS Bindings: GREEN")

def test_vm_bindings():
    print("Testing QASMR Bytecode VM Bindings from Python...")
    qvs = qau_cpp.QVS()
    prog = qau_cpp.QASMRProgram()
    
    # Instruction: SUPERPOSE(id=0, states=[[0], [1]])
    inst1 = qau_cpp.Instruction(
        qau_cpp.OpCode.SUPERPOSE, 
        [0], [], [[0], [1]], ""
    )
    
    # Instruction: WEAVE(id=0, theta=1.57)
    inst2 = qau_cpp.Instruction(
        qau_cpp.OpCode.WEAVE,
        [0], [1.57], [], ""
    )
    
    prog.add(inst1)
    prog.add(inst2)
    
    qvs.create_asc(1)
    qau_cpp.BytecodeVM.execute(qvs, prog)
    print("VM Execution from Python: GREEN")

if __name__ == "__main__":
    print("=== QAU C++ Engine Phase 4 Integration Harness ===")
    try:
        test_qvs_bindings()
        test_vm_bindings()
        print("=== All Phase 4 Integration Tests: ULTIMATE GREEN SUCCESS! ===")
    except Exception as e:
        print(f"Phase 4 Integration Failed: {e}")
        sys.exit(1)
