#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "EdgeUtility.Binding.h"

namespace py = pybind11;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

void register_EdgeUtility_class(py::module& m) {
    py::class_<EdgeUtility, std::shared_ptr<EdgeUtility >>(m, "EdgeUtility")
    .def(py::init<>()) // Because there is a default c'tor 
    .def_static(
        "ByVertices",
        [](const std::list<TopologicCore::Vertex::Ptr>& rkVertices)
        {
            return EdgeUtility::ByVertices(rkVertices);
        },
        " ", py::arg("rkVertices"))
    .def_static(
        "ByCircle",
        [](const TopologicCore::Vertex::Ptr& kpCenterPoint, const double kRadius,
            const double kXAxisX, const double kXAxisY, const double kXAxisZ,
            const double kNormalX, const double kNormalY, const double kNormalZ)
        {
            return EdgeUtility::ByCircle(kpCenterPoint, kRadius, kXAxisX, kXAxisY,
                kXAxisZ, kNormalX, kNormalY, kNormalZ);
        },
        " ", py::arg("kpCenterPoint"), py::arg("kRadius"), py::arg("kXAxisX"), py::arg("kXAxisY"),
            py::arg("kXAxisZ"), py::arg("kNormalX"), py::arg("kNormalY"), py::arg("kNormalZ"))
    .def_static(
        "ByEllipse",
        [](const TopologicCore::Vertex::Ptr& kpCenterPoint, 
            const double kMajorRadius, const double kMinorRadius,
            const double kXAxisX, const double kXAxisY, const double kXAxisZ,
            const double kNormalX, const double kNormalY, const double kNormalZ)
        {
            return EdgeUtility::ByEllipse(kpCenterPoint, kMajorRadius, kMinorRadius, kXAxisX, kXAxisY,
                kXAxisZ, kNormalX, kNormalY, kNormalZ);
        },
        " ", py::arg("kpCenterPoint"), py::arg("kMajorRadius"), py::arg("kMinorRadius"), 
            py::arg("kXAxisX"), py::arg("kXAxisY"),
            py::arg("kXAxisZ"), py::arg("kNormalX"), py::arg("kNormalY"), py::arg("kNormalZ"))
    .def_static(
        "ByNurbsCurve",
        [](const std::list<TopologicCore::Vertex::Ptr>& rkControlPoints,
            const std::list<double>& rkKnots,
            const std::list<double>& rkWeights,
            const int kDegree,
            const bool kIsPeriodic,
            const bool kIsRational)
        {
            return EdgeUtility::ByNurbsCurve(rkControlPoints, rkKnots, rkWeights, kDegree,
                kIsPeriodic, kIsRational);
        },
        " ", py::arg("rkControlPoints"), py::arg("rkKnots"), py::arg("rkWeights"), py::arg("kDegree"),
            py::arg("kIsPeriodic"), py::arg("kIsRational"))
    .def_static(
        "ParameterAtPoint",
        [](const TopologicCore::Edge::Ptr& kpEdge, const TopologicCore::Vertex::Ptr& kpVertex)
        {
            return EdgeUtility::ParameterAtPoint(kpEdge, kpVertex);
        },
        " ", py::arg("kpEdge"), py::arg("kpVertex"))
    .def_static(
        "PointAtParameter",
        [](const TopologicCore::Edge::Ptr& kpEdge, const double kParameter)
        {
            return EdgeUtility::PointAtParameter(kpEdge, kParameter);
        },
        " ", py::arg("kpEdge"), py::arg("kParameter"))
    .def_static(
        "Length",
        [](const TopologicCore::Edge::Ptr& kpEdge)
        {
            return EdgeUtility::Length(kpEdge);
        },
        " ", py::arg("kpEdge"))
    .def_static(
        "AdjacentWires",
        [](const TopologicCore::Edge::Ptr& kpEdge,
            const TopologicCore::Topology::Ptr& kpParentTopology,
            py::list& rCoreAdjacentWires)
        {
            std::list<TopologicCore::Wire::Ptr> rCoreAdjacentWiresLocal;
            EdgeUtility::AdjacentWires(kpEdge.get(), kpParentTopology, rCoreAdjacentWiresLocal);
            for (auto& x : rCoreAdjacentWiresLocal)
                rCoreAdjacentWires.append(x);
            return;
        },
        " ", py::arg("kpEdge"), py::arg("kpParentTopology"), py::arg("rCoreAdjacentWires"))
    .def_static(
        "AdjacentWires",
        [](TopologicCore::Edge const* const kpkEdge,
            const TopologicCore::Topology::Ptr& kpParentTopology,
            py::list& rCoreAdjacentWires)
        {
            std::list<TopologicCore::Wire::Ptr> rCoreAdjacentWiresLocal;
            EdgeUtility::AdjacentWires(kpkEdge, kpParentTopology, rCoreAdjacentWiresLocal);
            for (auto& x : rCoreAdjacentWiresLocal)
                rCoreAdjacentWires.append(x);
            return;
        },
        " ", py::arg("kpkEdge"), py::arg("kpParentTopology"), py::arg("rCoreAdjacentWires"))
    .def_static(
        "AdjacentFaces",
        [](const TopologicCore::Edge::Ptr& kpEdge, 
            const TopologicCore::Topology::Ptr& kpParentTopology,
            py::list& rCoreAdjacentFaces)
        {
            std::list<TopologicCore::Face::Ptr> rCoreAdjacentFacesLocal;
            EdgeUtility::AdjacentFaces(kpEdge, kpParentTopology, rCoreAdjacentFacesLocal);
            for (auto& x : rCoreAdjacentFacesLocal)
                rCoreAdjacentFaces.append(x);
            return;
        },
        " ", py::arg("kpkEdge"), py::arg("kpParentTopology"), py::arg("rCoreAdjacentFaces"))
    .def_static(
        "AngleBetween",
        [](const TopologicCore::Edge::Ptr& kpEdge1,
           const TopologicCore::Edge::Ptr& kpEdge2
           )
        {
            return EdgeUtility::AngleBetween(kpEdge1, kpEdge2);
        },
        " ", py::arg("kpEdge1"), py::arg("kpEdge2"))
        ;
}