#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "DoubleAttribute.cppwg.hpp"

namespace py = pybind11;
//typedef TopologicCore::DoubleAttribute DoubleAttribute;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
typedef void * voidPtr;

class DoubleAttribute_Overloads : public DoubleAttribute{
    public:
    using DoubleAttribute::DoubleAttribute;
    void * Value() override {
        PYBIND11_OVERLOAD(
            voidPtr,
            DoubleAttribute,
            Value,
            );
    }

};
void register_DoubleAttribute_class(py::module &m){
py::class_<DoubleAttribute , DoubleAttribute_Overloads , std::shared_ptr<DoubleAttribute >  , Attribute  >(m, "DoubleAttribute")
        .def(py::init<double const >(), py::arg("kValue"))
        .def(
            "Value", 
            (void *(DoubleAttribute::*)()) &DoubleAttribute::Value, 
            " "  )
        .def(
            "DoubleValue", 
            (double(DoubleAttribute::*)()) &DoubleAttribute::DoubleValue, 
            " "  )
    ;
}
