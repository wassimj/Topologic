#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "VertexUtility.Binding.h"

namespace py = pybind11;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

void register_VertexUtility_class(py::module& m) {
    py::class_<VertexUtility, std::shared_ptr<VertexUtility >>(m, "VertexUtility")
        .def(py::init<>()) // Because there is a default c'tor 
        .def_static(
            "AdjacentEdges",
            [](const TopologicCore::Vertex::Ptr& kpVertex, const TopologicCore::Topology::Ptr& kpParentTopology, py::list& rCoreAdjacentEdges)
            {
                std::list<TopologicCore::Edge::Ptr> rCoreAdjacentEdgesLocal;
                VertexUtility::AdjacentEdges(
                    kpVertex, kpParentTopology.get(), rCoreAdjacentEdgesLocal);
                for (auto& x : rCoreAdjacentEdgesLocal)
                    rCoreAdjacentEdges.append(x);
            },
            " ", py::arg("kpVertex"), py::arg("kpParentTopology"), py::arg("rCoreAdjacentEdges"))
        .def_static(
            "AdjacentEdges",
            [](const TopologicCore::Vertex::Ptr& kpVertex, TopologicCore::Topology const* const kpkParentTopology, py::list& rCoreAdjacentEdges)
            {
                std::list<TopologicCore::Edge::Ptr> rCoreAdjacentEdgesLocal;
                VertexUtility::AdjacentEdges(
                    kpVertex, kpkParentTopology, rCoreAdjacentEdgesLocal);
                for (auto& x : rCoreAdjacentEdgesLocal)
                    rCoreAdjacentEdges.append(x);
            },
            " ", py::arg("kpVertex"), py::arg("kpkParentTopology"), py::arg("rCoreAdjacentEdges"))
          .def_static(
            "Distance",
              [](const TopologicCore::Vertex::Ptr& kpVertex, const TopologicCore::Topology::Ptr& kpTopology) -> double
              {
                  return VertexUtility::Distance(kpVertex, kpTopology);
              },
              " ", py::arg("kpVertex"), py::arg("kpTopology"))
       ;
}