#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "Aperture.cppwg.hpp"

namespace py = pybind11;

//using namespace TopologicCore;

//typedef TopologicCore::Aperture Aperture;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
typedef ::std::shared_ptr<TopologicCore::Vertex> _std_shared_ptr_lt_TopologicCore_Vertex_gt_;
typedef ::std::string _std_string;
typedef ::TopoDS_Shape & _TopoDS_ShapeRef;
typedef ::TopoDS_Shape const & _TopoDS_ShapeconstRef;
typedef ::TopologicCore::TopologyType _TopologicCore_TopologyType;
typedef ::std::string _std_string;
typedef ::TopoDS_Shape _TopoDS_Shape;

class Aperture_Overloads : public Aperture{
    public:
    using Aperture::Aperture;
    void Geometry(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> & rOcctGeometries) const  override {
        PYBIND11_OVERLOAD(
            void,
            Aperture,
            Geometry,
            rOcctGeometries);
    }
    ::std::shared_ptr<TopologicCore::Vertex> CenterOfMass() const  override {
        PYBIND11_OVERLOAD(
            _std_shared_ptr_lt_TopologicCore_Vertex_gt_,
            Aperture,
            CenterOfMass,
            );
    }
    bool IsManifold(const Topology::Ptr& kpHostTopology) const  override {
        PYBIND11_OVERLOAD(
            bool,
            Aperture,
            IsManifold,
            kpHostTopology);
    }
    ::std::string GetTypeAsString() const  override {
        PYBIND11_OVERLOAD(
            _std_string,
            Aperture,
            GetTypeAsString,
            );
    }
    ::TopoDS_Shape & GetOcctShape() override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShapeRef,
            Aperture,
            GetOcctShape,
            );
    }
    void SetOcctShape(::TopoDS_Shape const & rkOcctShape) override {
        PYBIND11_OVERLOAD(
            void,
            Aperture,
            SetOcctShape,
            rkOcctShape);
    }
    ::TopoDS_Shape const & GetOcctShape() const  override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShapeconstRef,
            Aperture,
            GetOcctShape,
            );
    }
    ::TopologicCore::TopologyType GetType() const  override {
        PYBIND11_OVERLOAD(
            _TopologicCore_TopologyType,
            Aperture,
            GetType,
            );
    }
    ::std::string GetClassGUID() const  override {
        PYBIND11_OVERLOAD(
            _std_string,
            Aperture,
            GetClassGUID,
            );
    }
    bool IsContainerType() override {
        PYBIND11_OVERLOAD(
            bool,
            Aperture,
            IsContainerType,
            );
    }
    ::TopoDS_Shape OcctShapeFix(::TopoDS_Shape const & rkOcctInputShape) override {
        PYBIND11_OVERLOAD(
            _TopoDS_Shape,
            Aperture,
            OcctShapeFix,
            rkOcctInputShape);
    }

};
void register_Aperture_class(py::module &m){
py::class_<Aperture , Aperture_Overloads , std::shared_ptr<Aperture >  , Topology  >(m, "Aperture")
        .def(py::init<::TopologicCore::Topology::Ptr const &, ::std::shared_ptr<TopologicCore::Context> const &, ::std::string const & >(), py::arg("kpTopology"), py::arg("kpContext"), py::arg("rkGuid") = "")
        .def_static(
            "ByTopologyContext", 
            (::std::shared_ptr<TopologicCore::Aperture>(*)(::TopologicCore::Topology::Ptr const &, ::std::shared_ptr<TopologicCore::Context> const &)) &Aperture::ByTopologyContext, 
            " " , py::arg("kpTopology"), py::arg("kpContext") )
        .def_static(
            "ByTopologyContext", 
            (::std::shared_ptr<TopologicCore::Aperture>(*)(::TopologicCore::Topology::Ptr const &, ::TopologicCore::Topology::Ptr const &)) &Aperture::ByTopologyContext, 
            " " , py::arg("kpTopology"), py::arg("kpContextTopology") )
        .def(
            "Topology", 
            (::std::shared_ptr<TopologicCore::Topology>(Aperture::*)() const ) &Aperture::Topology, 
            " "  )
        .def(
            "GetMainContext", 
            (::std::shared_ptr<TopologicCore::Context> &(Aperture::*)()) &Aperture::GetMainContext, 
            " "  )
        .def(
            "GetMainContext", 
            (::std::shared_ptr<TopologicCore::Context> const &(Aperture::*)() const ) &Aperture::GetMainContext, 
            " "  )
        .def(
            "Geometry", 
            (void(Aperture::*)(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> &) const ) &Aperture::Geometry, 
            " " , py::arg("rOcctGeometries") )
        .def(
            "CenterOfMass", 
            (::std::shared_ptr<TopologicCore::Vertex>(Aperture::*)() const ) &Aperture::CenterOfMass, 
            " "  )
        .def(
            "IsManifold", 
            (bool(Aperture::*)(const Topology::Ptr &) const ) &Aperture::IsManifold,
            " " , py::arg("kpHostTopology"))
        .def(
            "GetTypeAsString", 
            (::std::string(Aperture::*)() const ) &Aperture::GetTypeAsString, 
            " "  )
        .def(
            "GetOcctShape", 
            (::TopoDS_Shape &(Aperture::*)()) &Aperture::GetOcctShape, 
            " "  )
        .def(
            "SetOcctShape", 
            (void(Aperture::*)(::TopoDS_Shape const &)) &Aperture::SetOcctShape, 
            " " , py::arg("rkOcctShape") )
        .def(
            "GetOcctShape", 
            (::TopoDS_Shape const &(Aperture::*)() const ) &Aperture::GetOcctShape, 
            " "  )
        .def(
            "GetType", 
            (::TopologicCore::TopologyType(Aperture::*)() const ) &Aperture::GetType, 
            " "  )
        .def(
            "GetClassGUID", 
            (::std::string(Aperture::*)() const ) &Aperture::GetClassGUID, 
            " "  )
        .def(
            "IsContainerType", 
            (bool(Aperture::*)()) &Aperture::IsContainerType, 
            " "  )
    ;
}
