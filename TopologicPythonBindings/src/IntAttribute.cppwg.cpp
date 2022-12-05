#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "IntAttribute.cppwg.hpp"

namespace py = pybind11;
//typedef TopologicCore::IntAttribute IntAttribute;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
typedef void * voidPtr;

class IntAttribute_Overloads : public IntAttribute{
    public:
    using IntAttribute::IntAttribute;
    void * Value() override {
        PYBIND11_OVERLOAD(
            voidPtr,
            IntAttribute,
            Value,
            );
    }

};
void register_IntAttribute_class(py::module &m){
py::class_<IntAttribute , IntAttribute_Overloads , std::shared_ptr<IntAttribute >  , Attribute  >(m, "IntAttribute")
        .def(py::init<long long int const >(), py::arg("kValue"))
        .def(
            "Value", 
            (void *(IntAttribute::*)()) &IntAttribute::Value, 
            " "  )
        .def(
            "IntValue", 
            (long long int(IntAttribute::*)()) &IntAttribute::IntValue, 
            " "  )
    ;
}
