#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "FaceUtility.Binding.h"
#include <tuple>

namespace py = pybind11;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

void register_FaceUtility_class(py::module& m) {
    py::class_<FaceUtility, std::shared_ptr<FaceUtility >>(m, "FaceUtility")
        .def(py::init<>()) // Because there is a default c'tor 
        .def_static(
            "NormalAtParameters",
            [](const TopologicCore::Face::Ptr& kpFace, const double kU, const double kV)
            {
                auto ret_val = FaceUtility::NormalAtParameters(kpFace, kU, kV);
                return std::make_tuple(ret_val.X(), ret_val.Y(), ret_val.Z());
            },
            " ", py::arg("kpFace"), py::arg("kU"), py::arg("kV"))
        .def_static(
            "Area",
            [](const TopologicCore::Face::Ptr& kpFace)
            {
                return FaceUtility::Area(kpFace);
            },
            " ", py::arg("kpFace"))
        .def_static(
            "Area",
            [](const TopoDS_Face& rkOcctFace)
            {
                return FaceUtility::Area(rkOcctFace);
            },
            " ", py::arg("rkOcctFace"))
        .def_static(
            "ByVertices",
            [](const std::list<std::list<TopologicCore::Vertex::Ptr>>& rkVertices)
            {
                return FaceUtility::ByVertices(rkVertices);
            },
            " ", py::arg("rkVertices"))
        .def_static(
            "ParametersAtVertex",
            [](const TopologicCore::Face::Ptr& kpFace, const TopologicCore::Vertex::Ptr& kpVertex)
            {
                double rU, rV;
                FaceUtility::ParametersAtVertex(kpFace, kpVertex, rU, rV);
                return std::make_tuple(rU, rV);
            },
            " ", py::arg("kpFace"), py::arg("kpVertex"))
        .def_static(
            "VertexAtParameters",
            [](const TopologicCore::Face::Ptr& kpFace, const double kU, const double kV)
            {
                return FaceUtility::VertexAtParameters(kpFace, kU, kV);
            },
            " ", py::arg("kpFace"),py::arg("kU"), py::arg("kV"))
        .def_static(
            "UVSamplePoints",
            [](const TopologicCore::Face::Ptr& kpFace,
                const std::list<double>& rkUValues,
                const std::list<double>& rkVValues,
                py::list& rSamplePoints,
                py::list& rOcctUValues,
                py::list& rOcctVValues,
                int& rNumUPoints,
                int& rNumVPoints,
                int& rNumUPanels,
                int& rNumVPanels,
                bool& rIsUClosed,
                bool& rIsVClosed)
            {
                std::list<std::list<gp_Pnt>> rSamplePointsLocal;
                std::list<double> rOcctUValuesLocal;
                std::list<double> rOcctVValuesLocal;
                FaceUtility::UVSamplePoints(kpFace, rkUValues, rkVValues, rSamplePointsLocal,
                    rOcctUValuesLocal, rOcctVValuesLocal, rNumUPoints, rNumVPoints,
                    rNumUPanels, rNumVPanels, rIsUClosed, rIsVClosed);
                for (auto& x : rSamplePointsLocal)
                    rSamplePoints.append(x);
                for (auto& x : rOcctUValuesLocal)
                    rOcctUValues.append(x);
                for (auto& x : rOcctVValuesLocal)
                    rOcctVValues.append(x);
            },
            " ", py::arg("kpFace"), py::arg("rkUValues"), py::arg("rkVValues"),
                py::arg("rSamplePoints"), py::arg("rOcctUValues"), py::arg("rOcctVValues"),
                py::arg("rNumUPoints"), py::arg("rNumVPoints"), py::arg("rNumUPanels"),
                py::arg("rNumVPanels"), py::arg("rIsUClosed"), py::arg("rIsVClosed"))
        .def_static(
            "TrimByWire",
            [](const TopologicCore::Face::Ptr& kpFace, 
                const TopologicCore::Wire::Ptr& kpWire, const bool kReverseWire)
            {
                return FaceUtility::TrimByWire(kpFace, kpWire, kReverseWire);
            },
            " ", py::arg("kpFace"), py::arg("kpWire"), py::arg("kReverseWire"))
        .def_static(
            "Triangulate",
            [](const TopologicCore::Face::Ptr& kpFace, const double kDeflection, 
                py::list& rTriangles)
            {
                std::list<TopologicCore::Face::Ptr> rTrianglesLocal;
                FaceUtility::Triangulate(kpFace, kDeflection, rTrianglesLocal);
                for (auto& x : rTrianglesLocal)
                    rTriangles.append(x);
            },
            " ", py::arg("kpFace"), py::arg("kDeflection"), py::arg("rTriangles"))
        .def_static(
            "NormalizeUV",
            [](const TopologicCore::Face::Ptr& kpFace, const double kNonNormalizedU, 
                const double kNonNormalizedV, double& rNormalizedU, double& rNormalizedV)
            {
                return FaceUtility::NormalizeUV(kpFace, kNonNormalizedU, kNonNormalizedV,
                    rNormalizedU, rNormalizedV);
            },
            " ", py::arg("kpFace"), py::arg("kNonNormalizedU"), 
                py::arg("kNonNormalizedV"), py::arg("rNormalizedU"),
                py::arg("rNormalizedV"))
        .def_static(
            "NonNormalizeUV",
            [](const TopologicCore::Face::Ptr& kpFace, const double kNormalizedU, 
                const double kNormalizedV, double& rNonNormalizedU, double& rNonNormalizedV)
            {
                return FaceUtility::NonNormalizeUV(kpFace, kNormalizedU, kNormalizedV,
                    rNonNormalizedU, rNonNormalizedV);
            },
            " ", py::arg("kpFace"), py::arg("kNormalizedU"),
                py::arg("kNormalizedV"), py::arg("rNonNormalizedU"),
                py::arg("rNonNormalizedV"))
        /*.def_static(
            "AdjacentFaces",
            [](const TopologicCore::Face::Ptr& kpFace,
                const TopologicCore::Topology::Ptr& kpParentTopology,
                py::list& rCoreAdjacentFaces)
            {
                std::list<TopologicCore::Face::Ptr> rCoreAdjacentFacesLocal;
                FaceUtility::AdjacentFaces(kpFace.get(), kpParentTopology, rCoreAdjacentFacesLocal);
                for (auto& x : rCoreAdjacentFacesLocal)
                    rCoreAdjacentFaces.append(x);
            },
            " ", py::arg("kpFace"), py::arg("kpParentTopology"), py::arg("rCoreAdjacentFaces")) */
        .def_static(
            "AdjacentShells",
            [](const TopologicCore::Face::Ptr& kpFace,
                const TopologicCore::Topology::Ptr& kpParentTopology,
                py::list& rCoreAdjacentShells)
            {
                std::list<TopologicCore::Shell::Ptr> rCoreAdjacentShellsLocal;
                FaceUtility::AdjacentShells(kpFace, kpParentTopology, rCoreAdjacentShellsLocal);
                for (auto& x : rCoreAdjacentShellsLocal)
                    rCoreAdjacentShells.append(x);
            },
            " ", py::arg("kpFace"), py::arg("kpParentTopology"),
                py::arg("rCoreAdjacentShells"))
        .def_static(
            "AdjacentCells",
            [](const TopologicCore::Face::Ptr& kpFace,
                const TopologicCore::Topology::Ptr& kpParentTopology,
                py::list& rCoreAdjacentCells)
            {
                std::list<TopologicCore::Cell::Ptr> rCoreAdjacentCellsLocal;
                FaceUtility::AdjacentCells(kpFace, kpParentTopology, rCoreAdjacentCellsLocal);
                for (auto& x : rCoreAdjacentCellsLocal)
                    rCoreAdjacentCells.append(x);
            },
            " ", py::arg("kpFace"), py::arg("kpParentTopology"),
                py::arg("rCoreAdjacentCells"))
        .def_static(
            "IsInside",
            [](const TopologicCore::Face::Ptr& kpFace, const std::shared_ptr<TopologicCore::Vertex>& kpVertex, 
                const double kTolerance)
            {               
                return FaceUtility::IsInside(kpFace, kpVertex, kTolerance);
            },
            " ", py::arg("kpFace"), py::arg("kpVertex"), py::arg("kTolerance"))
        .def_static(
            "ProjectToSurface",
            [](const TopologicCore::Face::Ptr& kpFace, const std::shared_ptr<TopologicCore::Vertex>& kpVertex)
            {
                return FaceUtility::ProjectToSurface(kpFace, kpVertex);
            },
            " ", py::arg("kpFace"), py::arg("kpVertex"))
        .def_static(
            "InternalVertex",
            [](const TopologicCore::Face::Ptr& kpFace, const double kTolerance = 0.0001)
            {
                return FaceUtility::InternalVertex(kpFace, kTolerance);
            },
            " ", py::arg("kpFace"), py::arg("kTolerance"))
        ;
}