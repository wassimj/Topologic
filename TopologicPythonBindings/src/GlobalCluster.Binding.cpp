#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "GlobalCluster.Binding.h"

namespace py = pybind11;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

void register_GlobalCluster_class(py::module& m) {
    py::class_<GlobalCluster, std::shared_ptr<GlobalCluster >>(m, "GlobalCluster")
        .def(py::init<>()) // Singleton class, but with a public c'tor :)
        .def_static(
            "GetInstance",
            []()
            {
                return GlobalCluster::GetInstance();
            })
        .def(
            "AddTopology",
            [](GlobalCluster& obj, const std::shared_ptr<Topology>& rkTopology)
            {
                return obj.AddTopology(rkTopology);
            }, "", py::arg("rkTopology"))
        .def(
            "AddTopology",
            [](GlobalCluster& obj, const TopoDS_Shape& rkOcctShape)
            {
                return obj.AddTopology(rkOcctShape);
            }, "", py::arg("rkOcctShape"))
        .def(
            "GetCluster",
            [](GlobalCluster& obj)
            {
                return obj.GetCluster();
            })
        .def(
            "RemoveTopology",
            [](GlobalCluster& obj, const std::shared_ptr<Topology>& rkTopology)
            {
                return obj.RemoveTopology(rkTopology);
            }, "", py::arg("rkTopology"))
        .def(
            "RemoveTopology",
            [](GlobalCluster& obj, const TopoDS_Shape& rkOcctShape)
            {
                return obj.RemoveTopology(rkOcctShape);
            }, "", py::arg("rkOcctShape"))
        .def(
            "Clear",
            [](GlobalCluster& obj)
            {
                return obj.Clear();
            })
        .def(
            "GetOcctCompound",
            [](GlobalCluster& obj)
            {
                return obj.GetOcctCompound();
            })
        .def(
            "GetOcctCompound",
            [](const GlobalCluster& obj)
            {
                return obj.GetOcctCompound();
            })
        .def(
            "SubTopologies",
            [](const GlobalCluster& obj, py::list rSubTopologies)
            {
                std::list<std::shared_ptr<Topology>> rSubTopologiesLocal;
                obj.SubTopologies(rSubTopologiesLocal);
                for (auto& x : rSubTopologiesLocal)
                    rSubTopologies.append(x);
            })
        ;
}