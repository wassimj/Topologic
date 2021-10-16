#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "WireUtility.Binding.h"

namespace py = pybind11;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

void register_WireUtility_class(py::module& m) {
    py::class_<WireUtility, std::shared_ptr<WireUtility >>(m, "WireUtility")
        .def(py::init<>()) // Because there is a default c'tor 
        .def_static(
            "AdjacentShells",
            [](const TopologicCore::Wire::Ptr& kpWire,
                const TopologicCore::Topology::Ptr& kpParentTopology,
                py::list& rCoreAdjacentShells)
            {
                std::list<TopologicCore::Shell::Ptr> rCoreAdjacentShellsLocal;
                WireUtility::AdjacentShells(kpWire, kpParentTopology, rCoreAdjacentShellsLocal);
                for (auto& x : rCoreAdjacentShellsLocal)
                    rCoreAdjacentShells.append(x);
                return;
            },
            " ", py::arg("kpWire"), py::arg("kpParentTopology"), py::arg("rCoreAdjacentShells"))
        .def_static(
            "AdjacentCells",
            [](const TopologicCore::Wire::Ptr& kpWire,
                const TopologicCore::Topology::Ptr& kpParentTopology,
                py::list& rCoreAdjacentCells)
            {
                std::list<TopologicCore::Cell::Ptr> rCoreAdjacentCellsLocal;
                WireUtility::AdjacentCells(kpWire, kpParentTopology, rCoreAdjacentCellsLocal);
                for (auto& x : rCoreAdjacentCellsLocal)
                    rCoreAdjacentCells.append(x);
                return;
            },
            " ", py::arg("kpWire"), py::arg("kpParentTopology"), py::arg("rCoreAdjacentCells"))
        .def_static(
            "RemoveCollinearEdges",
            [](const TopologicCore::Wire::Ptr& kpWire,
                const double kTolerance)
            {
                WireUtility::RemoveCollinearEdges(kpWire, kTolerance);
            },
            " ", py::arg("kpWire"), py::arg("kTolerance"))
         ;
}