#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "Dictionary.cppwg.hpp"

namespace py = pybind11;
//typedef TopologicCore::Dictionary Dictionary;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

void register_Dictionary_class(py::module &m){
py::class_<Dictionary  , std::shared_ptr<Dictionary >   >(m, "Dictionary")
        .def(py::init< >())
        .def(
            "Add", 
            (void(Dictionary::*)(::std::string const &, ::TopologicCore::Attribute::Ptr const &)) &Dictionary::Add, 
            " " , py::arg("key"), py::arg("value") )
        .def(
            "Add", 
            (void(Dictionary::*)(::std::pair<std::basic_string<char>, std::shared_ptr<TopologicCore::Attribute>> const &)) &Dictionary::Add, 
            " " , py::arg("pair") )
        .def(
            "Clear", 
            (void(Dictionary::*)()) &Dictionary::Clear, 
            " "  )
        .def(
            "ContainsKey", 
            (bool(Dictionary::*)(::std::string const &) const ) &Dictionary::ContainsKey, 
            " " , py::arg("key") )
        .def(
            "Contains", 
            (bool(Dictionary::*)(::std::pair<std::basic_string<char>, std::shared_ptr<TopologicCore::Attribute>> const &) const ) &Dictionary::Contains, 
            " " , py::arg("pair") )
        .def(
            "Remove", 
            (bool(Dictionary::*)(::std::string const &)) &Dictionary::Remove, 
            " " , py::arg("key") )
        .def(
            "Remove", 
            (bool(Dictionary::*)(::std::pair<std::basic_string<char>, std::shared_ptr<TopologicCore::Attribute>> const &)) &Dictionary::Remove, 
            " " , py::arg("pair") )
        .def(
            "TryAdd", 
            (bool(Dictionary::*)(::std::string const &, ::TopologicCore::Attribute::Ptr const &)) &Dictionary::TryAdd, 
            " " , py::arg("key"), py::arg("Value") )
        .def(
            "TryGetValue", 
            (bool(Dictionary::*)(::std::string const &, ::TopologicCore::Attribute::Ptr const &)) &Dictionary::TryGetValue, 
            " " , py::arg("key"), py::arg("Value") )
        .def(
            "Count", 
            (::size_t(Dictionary::*)()) &Dictionary::Count, 
            " "  )
        .def(
            "Keys", 
            (::std::list<std::basic_string<char>, std::allocator<std::basic_string<char>>>(Dictionary::*)()) &Dictionary::Keys, 
            " "  )
        .def(
            "Values", 
            (::std::list<std::shared_ptr<TopologicCore::Attribute>, std::allocator<std::shared_ptr<TopologicCore::Attribute>>>(Dictionary::*)()) &Dictionary::Values, 
            " "  )
        // Vivek: Not been able to crack this issue: Conversion of array[] to pointer.
        //.def(
        //    "CopyTo", 
        //    (void(Dictionary::*)(::std::pair<std::basic_string<char>, std::shared_ptr<TopologicCore::Attribute>> *, int, int) const ) &Dictionary::CopyTo, 
        //    " " , py::arg("target"), py::arg("index"), py::arg("len") )
        .def(
            "GetEnumerator", 
            // Vivek: Auto -> Type is a problem
            //(::std::_Rb_tree_iterator<std::pair<const std::basic_string<char>, std::shared_ptr<TopologicCore::Attribute>>>(Dictionary::*)()) &Dictionary::GetEnumerator, 
            (Dictionary::iterator(Dictionary::*)()) & Dictionary::GetEnumerator,
            " "  )
        .def(
            "IsReadOnly", 
            (bool(Dictionary::*)()) &Dictionary::IsReadOnly, 
            " "  )
        .def(
            "IsFixedSize", 
            (bool(Dictionary::*)()) &Dictionary::IsFixedSize, 
            " "  )
        .def_static(
            "ByKeysValues", 
            (::TopologicCore::Dictionary(*)(::std::list<std::basic_string<char>, std::allocator<std::basic_string<char>>> const &, ::std::list<std::shared_ptr<TopologicCore::Attribute>, std::allocator<std::shared_ptr<TopologicCore::Attribute>>> &)) &Dictionary::ByKeysValues, 
            " " , py::arg("keys"), py::arg("values") )
        .def(
            "ValueAtKey", 
            (::TopologicCore::Attribute::Ptr(Dictionary::*)(::std::string const &)) &Dictionary::ValueAtKey, 
            " " , py::arg("key") )
    ;
}
