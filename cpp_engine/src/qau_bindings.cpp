#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include "qau/qvs.hpp"
#include "qau/qasm.hpp"
#include "qau/vm.hpp"
#include "qau/fields.hpp"
#include "qau/qml.hpp"
#include "qau/entropy.hpp"
#include "qau/holography.hpp"

namespace py = pybind11;

PYBIND11_MODULE(qau_cpp, m) {
    m.doc() = "QAU C++ Engine Python Bindings";

    py::class_<qau::ASC, std::shared_ptr<qau::ASC>>(m, "ASC")
        .def(py::init<int>())
        .def_readwrite("size", &qau::ASC::size)
        .def("normalize", &qau::ASC::normalize)
        .def("get_state_vector", &qau::ASC::get_state_vector);

    py::class_<qau::QVS>(m, "QVS")
        .def(py::init<>())
        .def("create_asc", &qau::QVS::create_asc)
        .def("get_asc", [](qau::QVS& self, int id) {
            return self.ascs.at(id);
        })
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

    // Phase 5: Fields and QML
    py::class_<qau::QuantumLattice>(m, "QuantumLattice")
        .def(py::init<qau::QVS&, int, int>())
        .def("apply_field_operator", &qau::QuantumLattice::apply_field_operator)
        .def("establish_gauge_symmetry", &qau::QuantumLattice::establish_gauge_symmetry)
        .def_readwrite("site_ids", &qau::QuantumLattice::site_ids);

    py::class_<qau::ParameterizedGate>(m, "ParameterizedGate")
        .def_readwrite("target_id", &qau::ParameterizedGate::target_id)
        .def_readwrite("theta", &qau::ParameterizedGate::theta)
        .def_readwrite("gradient", &qau::ParameterizedGate::gradient);

    py::class_<qau::QMLEngine>(m, "QMLEngine")
        .def(py::init<>())
        .def("add_parameterized_weave", &qau::QMLEngine::add_parameterized_weave)
        .def_readwrite("parameters", &qau::QMLEngine::parameters);

    // Phase 1: Entropy and Holography
    py::class_<qau::EntropyCalculator>(m, "EntropyCalculator")
        .def_static("von_neumann_entropy", [](const qau::ASC& asc, const std::vector<int>& indices) {
            auto rho = qau::EntropyCalculator::reduced_density_matrix(asc, indices);
            return qau::EntropyCalculator::von_neumann_entropy(rho);
        });

    py::class_<qau::HolographyEngine>(m, "HolographyEngine")
        .def_static("mutual_information", &qau::HolographyEngine::mutual_information)
        .def_static("emergent_distance", &qau::HolographyEngine::emergent_distance);
}
