#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "ShellUtility.Binding.h"

namespace py = pybind11;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

void register_ShellUtility_class(py::module& m) {
    py::class_<ShellUtility, std::shared_ptr<ShellUtility >>(m, "ShellUtility")
        .def(py::init<>()) // Because there is a default c'tor 
        .def_static(
            "ByLoft",
            [](const std::list<TopologicCore::Wire::Ptr>& rkWires)
            {
                return ShellUtility::ByLoft(rkWires);
            },
            " ", py::arg("ByLoft"))
        ;
}