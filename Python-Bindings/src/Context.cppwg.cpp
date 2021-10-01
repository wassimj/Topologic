#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "Context.cppwg.hpp"

namespace py = pybind11;
typedef TopologicCore::Context Context;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

void register_Context_class(py::module &m){
py::class_<Context  , std::shared_ptr<Context >  , TopologicalQuery  >(m, "Context")
        .def(py::init<::std::shared_ptr<TopologicCore::Topology> const &, double const, double const, double const >(), py::arg("kpTopology"), py::arg("kU"), py::arg("kV"), py::arg("kW"))
        .def_static(
            "ByTopologyParameters", 
            (::std::shared_ptr<TopologicCore::Context>(*)
                (::std::shared_ptr<TopologicCore::Topology> const &, 
                    double const, double const, double const)) &Context::ByTopologyParameters, 
            " " , py::arg("kpTopology"), py::arg("kU"), py::arg("kV"), py::arg("kW") )
        .def(
            "Topology", 
            (::std::shared_ptr<TopologicCore::Topology>(Context::*)() const ) &Context::Topology, 
            " "  )
        .def(
            "U", 
            (double(Context::*)() const ) &Context::U, 
            " "  )
        .def(
            "V", 
            (double(Context::*)() const ) &Context::V, 
            " "  )
        .def(
            "W", 
            (double(Context::*)() const ) &Context::W, 
            " "  )
    ;
}
