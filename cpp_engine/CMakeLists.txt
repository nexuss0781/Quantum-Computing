cmake_minimum_required(VERSION 3.15)
project(qau_cpp_engine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(qau_tests tests/test_qvs.cpp)

enable_testing()
add_test(NAME QAUTests COMMAND qau_tests)
