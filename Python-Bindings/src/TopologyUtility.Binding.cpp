#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "TopoligyUtility.Binding.h"

namespace py = pybind11;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

void register_TopologyUtility_class(py::module& m) {
    py::class_<TopologyUtility, std::shared_ptr<TopologyUtility >>(m, "TopologyUtility")
        .def(py::init<>()) // Because there is a default c'tor 
        .def_static(
            "Translate",
            [](const TopologicCore::Topology::Ptr& kpTopology, const double x = 0.0,
                const double y = 0.0, const double z = 0.0)
            {
                return TopologyUtility::Translate(kpTopology, x, y, z);
            },
            " ", py::arg("kpTopology"), py::arg("x"), py::arg("y"), py::arg("z"))
        .def_static(
            "Rotate",
            [](const TopologicCore::Topology::Ptr& kpTopology,
                const TopologicCore::Vertex::Ptr& kpOrigin,
                const double kDirectionX = 0.0, const double kDirectionY = 0.0, const double kDirectionZ = 1.0,
                const double kDegree = 0.0)
            {
                return TopologyUtility::Rotate(kpTopology, kpOrigin,
                    kDirectionX, kDirectionY, kDirectionZ, kDegree);
            },
            " ", py::arg("kpTopology"), py::arg("kpOrigin"), py::arg("kDirectionX"), py::arg("kDirectionY"),
                py::arg("kDirectionZ"), py::arg("kDegree"))
        .def_static(
            "Transform",
            [](const TopologicCore::Topology::Ptr& kpTopology,
                const double kTranslationX = 0.0, const double kTranslationY = 0.0, const double kTranslationZ = 0.0,
                const double kRotation11 = 1.0, const double kRotation12 = 0.0, const double kRotation13 = 0.0,
                const double kRotation21 = 0.0, const double kRotation22 = 1.0, const double kRotation23 = 0.0,
                const double kRotation31 = 0.0, const double kRotation32 = 0.0, const double kRotation33 = 1.0)
            {
                return TopologyUtility::Transform(kpTopology,
                    kTranslationX, kTranslationY, kTranslationZ,
                    kRotation11, kRotation12, kRotation13,
                    kRotation21, kRotation22, kRotation23,
                    kRotation31, kRotation32, kRotation33);
            },
            " ", py::arg("kpTopology"), py::arg("kTranslationX"), py::arg("kTranslationY"), py::arg("kTranslationZ"),
                py::arg("kRotation11"), py::arg("kRotation12"), py::arg("kRotation13"),
                py::arg("kRotation21"), py::arg("kRotation22"), py::arg("kRotation23"),
                py::arg("kRotation31"), py::arg("kRotation32"), py::arg("kRotation33"))
        .def_static(
            "Scale",
            [](const TopologicCore::Topology::Ptr& kpTopology, const TopologicCore::Vertex::Ptr& kpOrigin,
                const double kXFactor = 1.0, const double kYFactor = 1.0, const double kZFactor = 1.0)
            {
                return TopologyUtility::Scale(kpTopology,
                    kpOrigin, kXFactor, kYFactor,
                    kZFactor);
            },
            " ", py::arg("kpTopology"), py::arg("kpOrigin"), py::arg("kXFactor"), py::arg("kYFactor"),
                py::arg("kZFactor"))
        .def_static(
            "DegreeToRadian",
            [](const double kDegree)
            {
                return TopologyUtility::DegreeToRadian(kDegree);
            },
            " ", py::arg("kDegree"))
        .def_static(
            "RadianToDegree",
            [](const double kRadian)
            {
                return TopologyUtility::RadianToDegree(kRadian);
            },
            " ", py::arg("kRadian"))
        .def_static(
            "AdjacentTopologies",
            [](const TopologicCore::Topology::Ptr& kpCoreTopology,
                const TopologicCore::Topology::Ptr& kpCoreParentTopology,
                const int kTypeFilter,
                py::list& rCoreAdjacentTopologies)
            {
                std::list<TopologicCore::Topology::Ptr> rCoreAdjacentTopologiesLocal;
                TopologyUtility::AdjacentTopologies(kpCoreTopology, kpCoreParentTopology, kTypeFilter,
                    rCoreAdjacentTopologiesLocal);
                for (auto& x : rCoreAdjacentTopologiesLocal)
                    rCoreAdjacentTopologies.append(x);
            },
            " ", py::arg("kpCoreTopology"), py::arg("kpCoreParentTopology"), py::arg("kTypeFilter"), 
                py::arg("rCoreAdjacentTopologies"))
                ;
}