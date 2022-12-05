#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "Attribute.cppwg.hpp"

namespace py = pybind11;
//typedef TopologicCore::Attribute Attribute;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
typedef void * voidPtr;

class Attribute_Overloads : public Attribute{
    public:
    using Attribute::Attribute;
    void * Value() override {
        PYBIND11_OVERLOAD_PURE(
            voidPtr,
            Attribute,
            Value,
            );
    }

};
void register_Attribute_class(py::module &m){
py::class_<Attribute , Attribute_Overloads , std::shared_ptr<Attribute >   >(m, "Attribute")
        .def(py::init< >())
        .def(
            "Value", 
            (void *(Attribute::*)()) &Attribute::Value, 
            " "  )
    ;
}
