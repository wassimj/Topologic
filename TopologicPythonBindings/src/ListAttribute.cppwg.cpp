#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "ListAttribute.cppwg.hpp"

namespace py = pybind11;
//typedef TopologicCore::ListAttribute ListAttribute;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
typedef void * voidPtr;

class ListAttribute_Overloads : public ListAttribute{
    public:
    using ListAttribute::ListAttribute;
    void * Value() override {
        PYBIND11_OVERLOAD(
            voidPtr,
            ListAttribute,
            Value,
            );
    }

};
void register_ListAttribute_class(py::module &m){
py::class_<ListAttribute , ListAttribute_Overloads , std::shared_ptr<ListAttribute >  , Attribute  >(m, "ListAttribute")
        .def(py::init<::std::list<std::shared_ptr<TopologicCore::Attribute>, std::allocator<std::shared_ptr<TopologicCore::Attribute>>> const & >(), py::arg("rkAttributes"))
        .def(
            "Value", 
            (void *(ListAttribute::*)()) &ListAttribute::Value, 
            " "  )
        .def(
            "ListValue", 
            (::std::list<std::shared_ptr<TopologicCore::Attribute>, std::allocator<std::shared_ptr<TopologicCore::Attribute>>> const &(ListAttribute::*)() const ) &ListAttribute::ListValue, 
            " "  )
    ;
}
