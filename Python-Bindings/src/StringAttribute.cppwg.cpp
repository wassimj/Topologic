#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "StringAttribute.cppwg.hpp"

namespace py = pybind11;
//typedef TopologicCore::StringAttribute StringAttribute;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
typedef void * voidPtr;

class StringAttribute_Overloads : public StringAttribute{
    public:
    using StringAttribute::StringAttribute;
    void * Value() override {
        PYBIND11_OVERLOAD(
            voidPtr,
            StringAttribute,
            Value,
            );
    }

};
void register_StringAttribute_class(py::module &m){
py::class_<StringAttribute , StringAttribute_Overloads , std::shared_ptr<StringAttribute >  , Attribute  >(m, "StringAttribute")
        .def(py::init<::std::string const & >(), py::arg("kValue"))
        .def(
            "Value", 
            (void *(StringAttribute::*)()) &StringAttribute::Value, 
            " "  )
    ;
}
