#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "Wire.cppwg.hpp"

class BRepBuilderAPI_MakeWire;
namespace py = pybind11;
//typedef TopologicCore::Wire Wire;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
typedef ::TopoDS_Shape & _TopoDS_ShapeRef;
typedef ::TopoDS_Shape const & _TopoDS_ShapeconstRef;
typedef ::TopoDS_Wire & _TopoDS_WireRef;
typedef ::TopoDS_Wire const & _TopoDS_WireconstRef;
typedef ::std::shared_ptr<TopologicCore::Vertex> _std_shared_ptr_lt_TopologicCore_Vertex_gt_;
typedef ::TopologicCore::TopologyType _TopologicCore_TopologyType;
typedef ::std::string _std_string;
typedef ::std::string _std_string;

class Wire_Overloads : public Wire{
    public:
    using Wire::Wire;
    /*void Vertices(::std::list<std::shared_ptr<TopologicCore::Vertex>, ::TopologicCore::Topology::Ptr const& kpHostTopology, std::allocator<std::shared_ptr<TopologicCore::Vertex>>>& rVertices) const  override {
        PYBIND11_OVERLOAD(
            void,
            Wire,
            Vertices,
            kpHostTopology,
            rVertices);
    }*/
    /*void Edges(::std::list<std::shared_ptr<TopologicCore::Edge>, ::TopologicCore::Topology::Ptr const& kpHostTopology, std::allocator<std::shared_ptr<TopologicCore::Edge>>>& rEdges) const  override {
        PYBIND11_OVERLOAD(
            void,
            Wire,
            Edges,
            kpHostTopology,
            rEdges);
    }*/
    bool IsManifold(::TopologicCore::Topology::Ptr const & kpHostTopology) const  override {
        PYBIND11_OVERLOAD(
            bool,
            Wire,
            IsManifold,
            kpHostTopology);
    }
    void Geometry(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> & rOcctGeometries) const  override {
        PYBIND11_OVERLOAD(
            void,
            Wire,
            Geometry,
            rOcctGeometries);
    }
    ::TopoDS_Shape & GetOcctShape() override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShapeRef,
            Wire,
            GetOcctShape,
            );
    }
    ::TopoDS_Shape const & GetOcctShape() const  override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShapeconstRef,
            Wire,
            GetOcctShape,
            );
    }
    void SetOcctShape(::TopoDS_Shape const & rkOcctShape) override {
        PYBIND11_OVERLOAD(
            void,
            Wire,
            SetOcctShape,
            rkOcctShape);
    }
    ::TopoDS_Wire & GetOcctWire() override {
        PYBIND11_OVERLOAD(
            _TopoDS_WireRef,
            Wire,
            GetOcctWire,
            );
    }
    ::TopoDS_Wire const & GetOcctWire() const  override {
        PYBIND11_OVERLOAD(
            _TopoDS_WireconstRef,
            Wire,
            GetOcctWire,
            );
    }
    ::std::shared_ptr<TopologicCore::Vertex> CenterOfMass() const  override {
        PYBIND11_OVERLOAD(
            _std_shared_ptr_lt_TopologicCore_Vertex_gt_,
            Wire,
            CenterOfMass,
            );
    }
    ::TopologicCore::TopologyType GetType() const  override {
        PYBIND11_OVERLOAD(
            _TopologicCore_TopologyType,
            Wire,
            GetType,
            );
    }
    ::std::string GetTypeAsString() const  override {
        PYBIND11_OVERLOAD(
            _std_string,
            Wire,
            GetTypeAsString,
            );
    }
    ::std::string GetClassGUID() const  override {
        PYBIND11_OVERLOAD(
            _std_string,
            Wire,
            GetClassGUID,
            );
    }
    bool IsContainerType() override {
        PYBIND11_OVERLOAD(
            bool,
            Wire,
            IsContainerType,
            );
    }
};

void register_Wire_class(py::module &m){
py::class_<Wire , Wire_Overloads , std::shared_ptr<Wire >  , Topology  >(m, "Wire")
        .def(py::init<::TopoDS_Wire const &, ::std::string const & >(), py::arg("rkOcctWire"), py::arg("rkGuid") = "")
        .def(
            "Edges",
            [](const Wire& obj, const TopologicCore::Topology::Ptr& kpHostTopology, py::list& rEdges) {
                std::list<Edge::Ptr> local;
                obj.Edges(kpHostTopology, local);
                for (auto& x : local)
                    rEdges.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rEdges"))
        /*.def(
            "Faces", 
            (void(Wire::*)(::std::list<std::shared_ptr<TopologicCore::Face>, std::allocator<std::shared_ptr<TopologicCore::Face>>> &) const ) &Wire::Faces, 
            " " , py::arg("rFaces") )*/
        /*.def(
            "Faces",
            [](const Wire& obj, py::list& rFaces) {
                std::list<Face::Ptr> local;
                obj.Faces(local);
                for (auto& x : local)
                    rFaces.append(x);
            },
            " ", py::arg("rFaces"))*/
        .def(
            "IsClosed", 
            (bool(Wire::*)() const ) &Wire::IsClosed, 
            " "  )
        .def(
            "Vertices",
            [](const Wire& obj, const TopologicCore::Topology::Ptr& kpHostTopology, py::list& rVertices) {
                std::list<Vertex::Ptr> local;
                obj.Vertices(kpHostTopology, local);
                for (auto& x : local)
                    rVertices.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rVertices"))
        .def_static(
            "ByEdges", 
            (::std::shared_ptr<TopologicCore::Wire>(*)(::std::list<std::shared_ptr<TopologicCore::Edge>, std::allocator<std::shared_ptr<TopologicCore::Edge>>> const &, const bool)) &Wire::ByEdges, 
            " " , py::arg("rkEdges"), py::arg("kCopyAttributes") = false)
        .def_static(
            "ByOcctEdges", 
            (::TopoDS_Wire(*)(::TopTools_ListOfShape const &)) &Wire::ByOcctEdges, 
            " " , py::arg("rkOcctEdges") )
        .def(
            "IsManifold", 
            (bool(Wire::*)(::TopologicCore::Topology::Ptr const &) const ) &Wire::IsManifold,
            " ", py::arg("kpHostTopology"))
        .def(
            "NumberOfBranches", 
            (int(Wire::*)() const ) &Wire::NumberOfBranches, 
            " "  )
        .def(
            "Geometry", 
            (void(Wire::*)(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> &) const ) &Wire::Geometry, 
            " " , py::arg("rOcctGeometries") )
        .def(
            "GetOcctShape", 
            (::TopoDS_Shape &(Wire::*)()) &Wire::GetOcctShape, 
            " "  )
        .def(
            "GetOcctShape", 
            (::TopoDS_Shape const &(Wire::*)() const ) &Wire::GetOcctShape, 
            " "  )
        .def(
            "SetOcctShape", 
            (void(Wire::*)(::TopoDS_Shape const &)) &Wire::SetOcctShape, 
            " " , py::arg("rkOcctShape") )
        .def(
            "SetOcctWire", 
            (void(Wire::*)(::TopoDS_Wire const &)) &Wire::SetOcctWire, 
            " " , py::arg("rkOcctWire") )
        .def(
            "GetOcctWire", 
            (::TopoDS_Wire &(Wire::*)()) &Wire::GetOcctWire, 
            " "  )
        .def(
            "GetOcctWire", 
            (::TopoDS_Wire const &(Wire::*)() const ) &Wire::GetOcctWire, 
            " "  )
        .def(
            "CenterOfMass", 
            (::std::shared_ptr<TopologicCore::Vertex>(Wire::*)() const ) &Wire::CenterOfMass, 
            " "  )
        .def(
            "GetType", 
            (::TopologicCore::TopologyType(Wire::*)() const ) &Wire::GetType, 
            " "  )
        .def(
            "GetTypeAsString", 
            (::std::string(Wire::*)() const ) &Wire::GetTypeAsString, 
            " "  )
        .def(
            "GetClassGUID", 
            (::std::string(Wire::*)() const ) &Wire::GetClassGUID, 
            " "  )
        .def_static(
            "Type", 
            (int(*)()) &Wire::Type, 
            " "  )
        .def(
            "IsContainerType", 
            (bool(Wire::*)()) &Wire::IsContainerType, 
            " "  )
    ;
}
