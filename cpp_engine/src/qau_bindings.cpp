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
#include "qau/hyperbolic.hpp"
#include "qau/holographic_network.hpp"
#include "qau/quantum_dense.hpp"
#include "qau/black_hole.hpp"
#include "qau/backreaction.hpp"

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

    // Phase 2: Hyperbolic Geometry and Holographic Networks
    py::class_<qau::HyperbolicGeometry>(m, "HyperbolicGeometry")
        .def_static("distance", &qau::HyperbolicGeometry::distance)
        .def_static("generate_discretization", &qau::HyperbolicGeometry::generate_discretization);

    py::class_<qau::BulkNode>(m, "BulkNode")
        .def_readwrite("id", &qau::BulkNode::id)
        .def_readwrite("coord", &qau::BulkNode::coord)
        .def_readwrite("neighbors", &qau::BulkNode::neighbors);

    py::class_<qau::HolographicNetwork>(m, "HolographicNetwork")
        .def(py::init<int, int>())
        .def("map_boundary", &qau::HolographicNetwork::map_boundary)
        .def_readwrite("bulk_nodes", &qau::HolographicNetwork::bulk_nodes)
        .def_readwrite("boundary_to_bulk_map", &qau::HolographicNetwork::boundary_to_bulk_map);

    // Phase III: dense scrambling, OTOCs, and Page-curve diagnostics
    py::class_<qau::DenseQuantumState, std::shared_ptr<qau::DenseQuantumState>>(m, "DenseQuantumState")
        .def(py::init<int>())
        .def("num_qubits", &qau::DenseQuantumState::num_qubits)
        .def("norm_squared", &qau::DenseQuantumState::norm_squared)
        .def("normalize", &qau::DenseQuantumState::normalize)
        .def("get_state_vector", [](const qau::DenseQuantumState& state) {
            return state.amplitudes();
        });

    py::class_<qau::QuantumCircuit>(m, "QuantumCircuit")
        .def(py::init<int>())
        .def("hadamard", &qau::QuantumCircuit::hadamard)
        .def("rotation_y", &qau::QuantumCircuit::rotation_y)
        .def("rotation_z", &qau::QuantumCircuit::rotation_z)
        .def("controlled_not", &qau::QuantumCircuit::controlled_not)
        .def("apply", &qau::QuantumCircuit::apply)
        .def("apply_inverse", &qau::QuantumCircuit::apply_inverse)
        .def_static("brickwork_scrambler", &qau::QuantumCircuit::brickwork_scrambler,
                    py::arg("num_qubits"), py::arg("layers"), py::arg("seed"),
                    py::arg("active_qubits") = std::vector<int>{});

    py::class_<qau::OTOCResult>(m, "OTOCResult")
        .def_readonly("correlator", &qau::OTOCResult::correlator)
        .def_readonly("commutator_squared", &qau::OTOCResult::commutator_squared)
        .def_readonly("norm_error", &qau::OTOCResult::norm_error);

    py::class_<qau::OTOCAnalyzer>(m, "OTOCAnalyzer")
        .def_static("evaluate", &qau::OTOCAnalyzer::evaluate);

    py::class_<qau::PageCurvePoint>(m, "PageCurvePoint")
        .def_readonly("evaporation_step", &qau::PageCurvePoint::evaporation_step)
        .def_readonly("black_hole_qubits", &qau::PageCurvePoint::black_hole_qubits)
        .def_readonly("radiation_qubits", &qau::PageCurvePoint::radiation_qubits)
        .def_readonly("radiation_entropy", &qau::PageCurvePoint::radiation_entropy)
        .def_readonly("radiation_purity", &qau::PageCurvePoint::radiation_purity)
        .def_readonly("global_norm_error", &qau::PageCurvePoint::global_norm_error);

    py::class_<qau::EvaporatingBlackHoleToyModel>(m, "EvaporatingBlackHoleToyModel")
        .def(py::init<int, std::uint64_t, int>())
        .def("prepare_seeded_initial_state", &qau::EvaporatingBlackHoleToyModel::prepare_seeded_initial_state,
             py::arg("layers") = 6)
        .def("evaporate_one_qubit", &qau::EvaporatingBlackHoleToyModel::evaporate_one_qubit)
        .def("evaporate_all", &qau::EvaporatingBlackHoleToyModel::evaporate_all)
        .def("diagnostics", &qau::EvaporatingBlackHoleToyModel::diagnostics);

    py::class_<qau::DenseEntropyDiagnostics>(m, "DenseEntropyDiagnostics")
        .def_static("von_neumann_entropy", &qau::DenseEntropyDiagnostics::von_neumann_entropy)
        .def_static("purity", &qau::DenseEntropyDiagnostics::purity);

    // Phase IV: discrete geometry and matter backreaction
    py::class_<qau::GeometryVertex>(m, "GeometryVertex")
        .def_readonly("id", &qau::GeometryVertex::id)
        .def_readonly("x", &qau::GeometryVertex::x)
        .def_readonly("y", &qau::GeometryVertex::y)
        .def_readonly("energy_density", &qau::GeometryVertex::energy_density)
        .def_readonly("boundary", &qau::GeometryVertex::boundary);

    py::class_<qau::GeometryEdge>(m, "GeometryEdge")
        .def_readonly("id", &qau::GeometryEdge::id)
        .def_readonly("a", &qau::GeometryEdge::a)
        .def_readonly("b", &qau::GeometryEdge::b)
        .def_readonly("baseline_length", &qau::GeometryEdge::baseline_length)
        .def_readonly("length", &qau::GeometryEdge::length);

    py::class_<qau::GeometryDiagnostics>(m, "GeometryDiagnostics")
        .def_readonly("step", &qau::GeometryDiagnostics::step)
        .def_readonly("total_energy", &qau::GeometryDiagnostics::total_energy)
        .def_readonly("average_edge_length", &qau::GeometryDiagnostics::average_edge_length)
        .def_readonly("mean_abs_interior_curvature", &qau::GeometryDiagnostics::mean_abs_interior_curvature)
        .def_readonly("maximum_abs_interior_curvature", &qau::GeometryDiagnostics::maximum_abs_interior_curvature)
        .def_readonly("mean_ollivier_ricci", &qau::GeometryDiagnostics::mean_ollivier_ricci)
        .def_readonly("maximum_edge_delta", &qau::GeometryDiagnostics::maximum_edge_delta)
        .def_readonly("metric_valid", &qau::GeometryDiagnostics::metric_valid);

    py::class_<qau::DiscreteGeometry>(m, "DiscreteGeometry")
        .def_static("triangular_grid", &qau::DiscreteGeometry::triangular_grid,
                    py::arg("width"), py::arg("height"), py::arg("spacing") = 1.0)
        .def("set_energy_density", &qau::DiscreteGeometry::set_energy_density)
        .def("clear_energy_density", &qau::DiscreteGeometry::clear_energy_density)
        .def("set_radial_gaussian_source", &qau::DiscreteGeometry::set_radial_gaussian_source)
        .def("total_energy", &qau::DiscreteGeometry::total_energy)
        .def("mean_abs_interior_curvature", &qau::DiscreteGeometry::mean_abs_interior_curvature)
        .def("maximum_abs_interior_curvature", &qau::DiscreteGeometry::maximum_abs_interior_curvature)
        .def("mean_ollivier_ricci", &qau::DiscreteGeometry::mean_ollivier_ricci)
        .def("backreact_step", &qau::DiscreteGeometry::backreact_step,
             py::arg("coupling"), py::arg("relaxation") = 0.15, py::arg("min_length_ratio") = 0.60)
        .def("evolve", &qau::DiscreteGeometry::evolve,
             py::arg("steps"), py::arg("coupling"), py::arg("relaxation") = 0.15,
             py::arg("min_length_ratio") = 0.60)
        .def("metric_is_valid", &qau::DiscreteGeometry::metric_is_valid,
             py::arg("tolerance") = 1e-12)
        .def_property_readonly("vertices", [](const qau::DiscreteGeometry& geometry) {
            return geometry.vertices();
        })
        .def_property_readonly("edges", [](const qau::DiscreteGeometry& geometry) {
            return geometry.edges();
        })
        .def_property_readonly("triangles", [](const qau::DiscreteGeometry& geometry) {
            return geometry.triangles();
        });
}
