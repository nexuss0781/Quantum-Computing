cmake_minimum_required(VERSION 3.15)
project(qau_cpp_engine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include /usr/include/eigen3)

add_executable(qau_tests tests/test_qvs.cpp)
add_executable(phase2_tests tests/test_phase2.cpp)

enable_testing()
add_test(NAME QAUTests COMMAND qau_tests)
add_test(NAME Phase2Tests COMMAND phase2_tests)
