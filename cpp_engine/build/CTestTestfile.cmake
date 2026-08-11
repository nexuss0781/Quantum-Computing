# CMake generated Testfile for 
# Source directory: /home/ubuntu/Quantum-Computing/cpp_engine
# Build directory: /home/ubuntu/Quantum-Computing/cpp_engine/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(QAUTests "/home/ubuntu/Quantum-Computing/cpp_engine/build/qau_tests")
set_tests_properties(QAUTests PROPERTIES  _BACKTRACE_TRIPLES "/home/ubuntu/Quantum-Computing/cpp_engine/CMakeLists.txt;35;add_test;/home/ubuntu/Quantum-Computing/cpp_engine/CMakeLists.txt;0;")
add_test(Phase2Tests "/home/ubuntu/Quantum-Computing/cpp_engine/build/phase2_tests")
set_tests_properties(Phase2Tests PROPERTIES  _BACKTRACE_TRIPLES "/home/ubuntu/Quantum-Computing/cpp_engine/CMakeLists.txt;36;add_test;/home/ubuntu/Quantum-Computing/cpp_engine/CMakeLists.txt;0;")
add_test(Phase3Tests "/home/ubuntu/Quantum-Computing/cpp_engine/build/phase3_tests")
set_tests_properties(Phase3Tests PROPERTIES  _BACKTRACE_TRIPLES "/home/ubuntu/Quantum-Computing/cpp_engine/CMakeLists.txt;37;add_test;/home/ubuntu/Quantum-Computing/cpp_engine/CMakeLists.txt;0;")
add_test(EventHorizonPhase3Tests "/home/ubuntu/Quantum-Computing/cpp_engine/build/event_horizon_phase3_tests")
set_tests_properties(EventHorizonPhase3Tests PROPERTIES  _BACKTRACE_TRIPLES "/home/ubuntu/Quantum-Computing/cpp_engine/CMakeLists.txt;38;add_test;/home/ubuntu/Quantum-Computing/cpp_engine/CMakeLists.txt;0;")
add_test(EventHorizonPhase3PythonIntegration "/usr/bin/python3" "/home/ubuntu/Quantum-Computing/cpp_engine/tests/phase3_visualization.py")
set_tests_properties(EventHorizonPhase3PythonIntegration PROPERTIES  _BACKTRACE_TRIPLES "/home/ubuntu/Quantum-Computing/cpp_engine/CMakeLists.txt;39;add_test;/home/ubuntu/Quantum-Computing/cpp_engine/CMakeLists.txt;0;")
