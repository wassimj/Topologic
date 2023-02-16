#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "TopologicalQuery.cppwg.hpp"

namespace py = pybind11;
//typedef TopologicCore::TopologicCore::TopologicalQuery TopologicalQuery;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

void register_TopologicalQuery_class(py::module &m){
py::class_<TopologicalQuery  , std::shared_ptr<TopologicalQuery >   >(m, "TopologicalQuery")
        .def(py::init< >())
    ;
}
