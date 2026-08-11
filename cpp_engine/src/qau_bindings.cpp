#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include "qau/qvs.hpp"
#include "qau/qasm.hpp"
#include "qau/vm.hpp"

namespace py = pybind11;

PYBIND11_MODULE(qau_cpp, m) {
    m.doc() = "QAU C++ Engine Python Bindings";

    py::class_<qau::QVS>(m, "QVS")
        .def(py::init<>())
        .def("create_asc", &qau::QVS::create_asc)
        .def("SUPERPOSE", &qau::QVS::SUPERPOSE)
        .def("WEAVE", [](qau::QVS& self, int id, double theta) {
            self.WEAVE(id, nullptr, theta);
        })
        .def("BOND", &qau::QVS::BOND)
        .def("COLLAPSE", &qau::QVS::COLLAPSE);

    py::class_<qau::Instruction>(m, "Instruction")
        .def(py::init<qau::OpCode, std::vector<int>, std::vector<double>, std::vector<std::vector<int>>, std::string>(),
             py::arg("opcode"), py::arg("target_ids"), py::arg("params"), py::arg("states"), py::arg("metadata") = "")
        .def_readwrite("opcode", &qau::Instruction::opcode)
        .def_readwrite("target_ids", &qau::Instruction::target_ids)
        .def_readwrite("params", &qau::Instruction::params)
        .def_readwrite("states", &qau::Instruction::states)
        .def_readwrite("metadata", &qau::Instruction::metadata);

    py::enum_<qau::OpCode>(m, "OpCode")
        .value("SUPERPOSE", qau::OpCode::SUPERPOSE)
        .value("WEAVE", qau::OpCode::WEAVE)
        .value("BOND", qau::OpCode::BOND)
        .value("ROTATE", qau::OpCode::ROTATE)
        .value("COLLAPSE", qau::OpCode::COLLAPSE)
        .export_values();

    py::class_<qau::QASMRProgram>(m, "QASMRProgram")
        .def(py::init<>())
        .def("add", &qau::QASMRProgram::add)
        .def("optimize", &qau::QASMRProgram::optimize)
        .def_readwrite("instructions", &qau::QASMRProgram::instructions);

    py::class_<qau::BytecodeVM>(m, "BytecodeVM")
        .def_static("execute", &qau::BytecodeVM::execute);
}
