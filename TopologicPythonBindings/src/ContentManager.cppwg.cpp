#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "ContentManager.cppwg.hpp"

namespace py = pybind11;
//typedef TopologicCore::ContentManager ContentManager;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

void register_ContentManager_class(py::module &m){
py::class_<ContentManager  , std::shared_ptr<ContentManager >   >(m, "ContentManager")
        .def(py::init< >())
        .def_static(
            "GetInstance", 
            (::TopologicCore::ContentManager &(*)()) &ContentManager::GetInstance, 
            " "  )
        .def(
            "Add", 
            (void(ContentManager::*)(::TopoDS_Shape const &, ::std::shared_ptr<TopologicCore::Topology> const &)) &ContentManager::Add, 
            " " , py::arg("rkOcctShape"), py::arg("kpContentTopology") )
        .def(
            "Remove", 
            (void(ContentManager::*)(::TopoDS_Shape const &, ::TopoDS_Shape const &)) &ContentManager::Remove, 
            " " , py::arg("rkOcctShape"), py::arg("rkOcctContentTopology") )
        .def(
            "Find", 
            (bool(ContentManager::*)(::TopoDS_Shape const &, ::std::list<std::shared_ptr<TopologicCore::Topology>, std::allocator<std::shared_ptr<TopologicCore::Topology>>> &)) &ContentManager::Find, 
            " " , py::arg("rkOcctShape"), py::arg("rContents") )
        .def(
            "HasContent", 
            (bool(ContentManager::*)(::TopoDS_Shape const &, ::TopoDS_Shape const &)) &ContentManager::HasContent, 
            " " , py::arg("rkOcctShape"), py::arg("rkOcctContentTopology") )
        .def(
            "ClearOne", 
            (void(ContentManager::*)(::TopoDS_Shape const &)) &ContentManager::ClearOne, 
            " " , py::arg("rkOcctShape") )
        .def(
            "ClearAll", 
            (void(ContentManager::*)()) &ContentManager::ClearAll, 
            " "  )
    ;
}
