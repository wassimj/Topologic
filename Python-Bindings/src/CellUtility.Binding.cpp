#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "CellUtility.Binding.h"

namespace py = pybind11;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

void register_CellUtility_class(py::module& m) {
    py::enum_<CellContainmentState>(m, "CellContainmentState")
        .value("INSIDE", INSIDE)
        .value("ON_BOUNDARY", ON_BOUNDARY)
        .value("OUTSIDE", OUTSIDE)
        .value("UNKNOWN", UNKNOWN)
        .export_values();
    py::class_<CellUtility, std::shared_ptr<CellUtility >>(m, "CellUtility")
        .def(py::init<>()) // Because there is a default c'tor 
        .def_static(
            "ByLoft",
            [](const std::list<TopologicCore::Wire::Ptr>& rkWires)
            {
                return CellUtility::ByLoft(rkWires);
            },
            " ", py::arg("rkWires"))
        .def_static(
            "ByCuboid",
            [](const double kXCentroid, const double kYCentroid, const double kZCentroid,
                const double kXDimension, const double kYDimension, const double kZDimension,
                const double kXNormal, const double kYNormal, const double kZNormal,
                const double kXAxisX, const double kYAxisX, const double kZAxisX,
                const double kXAxisY, const double kYAxisY, const double kZAxisY)
            {
                return CellUtility::ByCuboid( kXCentroid,  kYCentroid,  kZCentroid,
                     kXDimension,  kYDimension,  kZDimension,
                     kXNormal,  kYNormal,  kZNormal,
                     kXAxisX,  kYAxisX,  kZAxisX,
                     kXAxisY,  kYAxisY,  kZAxisY);
            },
            " ", py::arg("kXCentroid"), py::arg("kYCentroid"), py::arg("kZCentroid"),
                py::arg("kXDimension"), py::arg("kYDimension"), py::arg("kZDimension"),
                py::arg("kXNormal"), py::arg("kYNormal"), py::arg("kZNormal"),
                py::arg("kXAxisX"), py::arg("kYAxisX"), py::arg("kZAxisX"),
                py::arg("kXAxisY"), py::arg("kYAxisY"), py::arg("kZAxisY"))
        .def_static(
            "BySphere",
            [](const double kCenterX, const double kCenterY, const double kCenterZ, const double kRadius)
            {
                return CellUtility::BySphere(kCenterX, kCenterY, kCenterZ, kRadius);
            },
            " ", py::arg("kCenterX"), py::arg("kCenterY"), py::arg("kCenterZ"), py::arg("kRadius"))
        .def_static(
            "ByTwoCorners",
            [](const std::shared_ptr<TopologicCore::Vertex>& kpMinVertex, const std::shared_ptr<TopologicCore::Vertex>& kpMaxVertex)
            {
                return CellUtility::ByTwoCorners(kpMinVertex, kpMaxVertex);
            },
            " ", py::arg("kpMinVertex"), py::arg("kpMaxVertex"))
        .def_static(
            "ByCylinder",
            [](const double kReferencePointX, const double kReferencePointY, const double kReferencePointZ,
                const double kNormalX, const double kNormalY, const double kNormalZ,
                const double kRadius, const double kHeight)
            {
                return CellUtility::ByCylinder( kReferencePointX,  kReferencePointY,  kReferencePointZ,
                     kNormalX,  kNormalY,  kNormalZ,
                     kRadius,  kHeight);
            },
            " ", py::arg("kReferencePointX"), py::arg("kReferencePointY"),
                py::arg("kReferencePointZ"), py::arg("kNormalX"),
                py::arg("kNormalY"), py::arg("kNormalZ"),
                py::arg("kRadius"), py::arg("kHeight"))
        .def_static(
            "ByCone",
            [](const double kReferencePointX, const double kReferencePointY, const double kReferencePointZ,
                const double kNormalX, const double kNormalY, const double kNormalZ,
                const double kXDirectionX, const double kXDirectionY, const double kDirectionZ,
                const double kRadius1, const double kRadius2, const double kHeight)
            {
                return CellUtility::ByCone( kReferencePointX,  kReferencePointY,  kReferencePointZ,
                     kNormalX,  kNormalY,  kNormalZ,
                     kXDirectionX,  kXDirectionY,  kDirectionZ,
                     kRadius1,  kRadius2,  kHeight);
            },
            " ", py::arg("kReferencePointX"), py::arg("kReferencePointY"),
                py::arg("kReferencePointZ"), py::arg("kNormalX"),
                py::arg("kNormalY"), py::arg("kNormalZ"), py::arg("kXDirectionX"),
                py::arg("kXDirectionY"), py::arg("kDirectionZ"),
                py::arg("kRadius1"), py::arg("kRadius2"), py::arg("kHeight"))
        .def_static(
            "InternalVertex",
            [](const TopologicCore::Cell::Ptr kpCell,
                const double kTolerance = 0.0001)
            {
                return CellUtility::InternalVertex(kpCell, kTolerance);
            },
            " ", py::arg("kpCell"), py::arg("kTolerance"))
        .def_static(
            "InternalVertex",
            [](const TopoDS_Solid& rkOcctSolid, 
                const double kTolerance = 0.0001)
            {
                return CellUtility::InternalVertex(rkOcctSolid, kTolerance);
            },
            " ", py::arg("rkOcctSolid"), py::arg("kTolerance"))
        .def_static(
            "Volume",
            [](const TopologicCore::Cell::Ptr& kpCell)
            {
                return CellUtility::Volume(kpCell);
            },
            " ", py::arg("kpCell"))
        .def_static(
            "Contains",
            [](const TopologicCore::Cell::Ptr& kpCell, const TopologicCore::Vertex::Ptr& kpVertex, 
                const double kTolerance = 0.0001)
            {
                return CellUtility::Contains(kpCell, kpVertex, kTolerance);
            },
            " ", py::arg("kpCell"), py::arg("kpVertex"), py::arg("kTolerance"))
        .def_static(
            "GetMinMax",
            [](const TopologicCore::Cell::Ptr& kpCell, double& rMinX, double& rMaxX, 
                double& rMinY, double& rMaxY, double& rMinZ, double& rMaxZ)
            {
                return CellUtility::GetMinMax(kpCell,  rMinX,  rMaxX,
                     rMinY,  rMaxY,  rMinZ,  rMaxZ);
            },
            " ", py::arg("kpCell"), py::arg("rMinX"), py::arg("rMaxX"),
                py::arg("rMinY"), py::arg("rMaxY"),
                py::arg("rMinZ"), py::arg("rMaxZ"))
        ;
}