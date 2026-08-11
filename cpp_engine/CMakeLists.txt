cmake_minimum_required(VERSION 3.15)
project(qau_cpp_engine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable SIMD and optimization flags
if(MSVC)
    add_compile_options(/O2 /arch:AVX2)
else()
    add_compile_options(-O3 -march=native -ffast-math -fopenmp)
endif()

find_package(OpenMP REQUIRED)

include_directories(include /usr/include/eigen3)

add_executable(qau_tests tests/test_qvs.cpp)
add_executable(phase2_tests tests/test_phase2.cpp)
add_executable(phase3_tests tests/test_phase3.cpp)
add_executable(event_horizon_phase3_tests tests/test_event_horizon_phase3.cpp)

target_link_libraries(qau_tests PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(phase2_tests PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(phase3_tests PRIVATE OpenMP::OpenMP_CXX)
target_link_libraries(event_horizon_phase3_tests PRIVATE OpenMP::OpenMP_CXX)

# Python Bindings
find_package(pybind11 REQUIRED)
find_package(Python3 COMPONENTS Interpreter REQUIRED)
pybind11_add_module(qau_cpp src/qau_bindings.cpp)
target_link_libraries(qau_cpp PRIVATE OpenMP::OpenMP_CXX)

enable_testing()
add_test(NAME QAUTests COMMAND qau_tests)
add_test(NAME Phase2Tests COMMAND phase2_tests)
add_test(NAME Phase3Tests COMMAND phase3_tests)
add_test(NAME EventHorizonPhase3Tests COMMAND event_horizon_phase3_tests)
add_test(NAME EventHorizonPhase3PythonIntegration
         COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/phase3_visualization.py)
