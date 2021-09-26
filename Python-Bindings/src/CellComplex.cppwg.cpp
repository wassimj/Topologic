#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "CellComplex.cppwg.hpp"

namespace py = pybind11;
//typedef TopologicCore::CellComplex CellComplex;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
typedef ::TopoDS_Shape & _TopoDS_ShapeRef;
typedef ::TopoDS_Shape const & _TopoDS_ShapeconstRef;
typedef ::TopoDS_CompSolid & _TopoDS_CompSolidRef;
typedef ::TopoDS_CompSolid const & _TopoDS_CompSolidconstRef;
typedef ::std::shared_ptr<TopologicCore::Vertex> _std_shared_ptr_lt_TopologicCore_Vertex_gt_;
typedef ::TopologicCore::TopologyType _TopologicCore_TopologyType;
typedef ::std::string _std_string;
typedef ::std::string _std_string;

class CellComplex_Overloads : public CellComplex{
    public:
    using CellComplex::CellComplex;
    void Vertices(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> & rVertices) const  override {
        PYBIND11_OVERLOAD(
            void,
            CellComplex,
            Vertices,
            rVertices);
    }
    bool IsManifold() const  override {
        PYBIND11_OVERLOAD(
            bool,
            CellComplex,
            IsManifold,
            );
    }
    ::TopoDS_Shape & GetOcctShape() override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShapeRef,
            CellComplex,
            GetOcctShape,
            );
    }
    ::TopoDS_Shape const & GetOcctShape() const  override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShapeconstRef,
            CellComplex,
            GetOcctShape,
            );
    }
    ::TopoDS_CompSolid & GetOcctCompSolid() override {
        PYBIND11_OVERLOAD(
            _TopoDS_CompSolidRef,
            CellComplex,
            GetOcctCompSolid,
            );
    }
    ::TopoDS_CompSolid const & GetOcctCompSolid() const  override {
        PYBIND11_OVERLOAD(
            _TopoDS_CompSolidconstRef,
            CellComplex,
            GetOcctCompSolid,
            );
    }
    void SetOcctShape(::TopoDS_Shape const & rkOcctShape) override {
        PYBIND11_OVERLOAD(
            void,
            CellComplex,
            SetOcctShape,
            rkOcctShape);
    }
    ::std::shared_ptr<TopologicCore::Vertex> CenterOfMass() const  override {
        PYBIND11_OVERLOAD(
            _std_shared_ptr_lt_TopologicCore_Vertex_gt_,
            CellComplex,
            CenterOfMass,
            );
    }
    void Geometry(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> & rOcctGeometries) const  override {
        PYBIND11_OVERLOAD(
            void,
            CellComplex,
            Geometry,
            rOcctGeometries);
    }
    ::TopologicCore::TopologyType GetType() const  override {
        PYBIND11_OVERLOAD(
            _TopologicCore_TopologyType,
            CellComplex,
            GetType,
            );
    }
    ::std::string GetTypeAsString() const  override {
        PYBIND11_OVERLOAD(
            _std_string,
            CellComplex,
            GetTypeAsString,
            );
    }
    ::std::string GetClassGUID() const  override {
        PYBIND11_OVERLOAD(
            _std_string,
            CellComplex,
            GetClassGUID,
            );
    }
    bool IsContainerType() override {
        PYBIND11_OVERLOAD(
            bool,
            CellComplex,
            IsContainerType,
            );
    }

};
void register_CellComplex_class(py::module &m){
py::class_<CellComplex , CellComplex_Overloads , std::shared_ptr<CellComplex >  , Topology  >(m, "CellComplex")
        .def(py::init<::TopoDS_CompSolid const &, ::std::string const & >(), py::arg("rkOcctCompSolid"), py::arg("rkGuid") = "")
        .def(
            "Cells", 
            (void(CellComplex::*)(::std::list<std::shared_ptr<TopologicCore::Cell>, std::allocator<std::shared_ptr<TopologicCore::Cell>>> &) const ) &CellComplex::Cells, 
            " " , py::arg("rCells") )
        .def(
            "Faces", 
            (void(CellComplex::*)(::std::list<std::shared_ptr<TopologicCore::Face>, std::allocator<std::shared_ptr<TopologicCore::Face>>> &) const ) &CellComplex::Faces, 
            " " , py::arg("rFaces") )
        .def(
            "Shells", 
            (void(CellComplex::*)(::std::list<std::shared_ptr<TopologicCore::Shell>, std::allocator<std::shared_ptr<TopologicCore::Shell>>> &) const ) &CellComplex::Shells, 
            " " , py::arg("rShells") )
        .def(
            "Edges", 
            (void(CellComplex::*)(::std::list<std::shared_ptr<TopologicCore::Edge>, std::allocator<std::shared_ptr<TopologicCore::Edge>>> &) const ) &CellComplex::Edges, 
            " " , py::arg("rEdges") )
        .def(
            "Vertices", 
            (void(CellComplex::*)(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> &) const ) &CellComplex::Vertices, 
            " " , py::arg("rVertices") )
        .def(
            "Wires", 
            (void(CellComplex::*)(::std::list<std::shared_ptr<TopologicCore::Wire>, std::allocator<std::shared_ptr<TopologicCore::Wire>>> &) const ) &CellComplex::Wires, 
            " " , py::arg("rWires") )
        .def_static(
            "ByCells", 
            (::std::shared_ptr<TopologicCore::CellComplex>(*)(::std::list<std::shared_ptr<TopologicCore::Cell>, std::allocator<std::shared_ptr<TopologicCore::Cell>>> const &)) &CellComplex::ByCells, 
            " " , py::arg("rkCells") )
        .def_static(
            "ByOcctSolids", 
            (::TopoDS_CompSolid(*)(::TopTools_ListOfShape const &)) &CellComplex::ByOcctSolids, 
            " " , py::arg("rkOcctSolids") )
        .def_static(
            "ByFaces", 
            (::std::shared_ptr<TopologicCore::CellComplex>(*)(::std::list<std::shared_ptr<TopologicCore::Face>, std::allocator<std::shared_ptr<TopologicCore::Face>>> const &, double const)) &CellComplex::ByFaces, 
            " " , py::arg("rkFaces"), py::arg("kTolerance") )
        .def(
            "ExternalBoundary", 
            (::std::shared_ptr<TopologicCore::Cell>(CellComplex::*)() const ) &CellComplex::ExternalBoundary, 
            " "  )
        .def(
            "InternalBoundaries", 
            (void(CellComplex::*)(::std::list<std::shared_ptr<TopologicCore::Face>, std::allocator<std::shared_ptr<TopologicCore::Face>>> &) const ) &CellComplex::InternalBoundaries, 
            " " , py::arg("rInternalFaces") )
        .def(
            "IsManifold", 
            (bool(CellComplex::*)() const ) &CellComplex::IsManifold, 
            " "  )
        .def(
            "NonManifoldFaces", 
            (void(CellComplex::*)(::std::list<std::shared_ptr<TopologicCore::Face>, std::allocator<std::shared_ptr<TopologicCore::Face>>> &) const ) &CellComplex::NonManifoldFaces, 
            " " , py::arg("rNonManifoldFaces") )
        .def(
            "GetOcctShape", 
            (::TopoDS_Shape &(CellComplex::*)()) &CellComplex::GetOcctShape, 
            " "  )
        .def(
            "GetOcctShape", 
            (::TopoDS_Shape const &(CellComplex::*)() const ) &CellComplex::GetOcctShape, 
            " "  )
        .def(
            "GetOcctCompSolid", 
            (::TopoDS_CompSolid &(CellComplex::*)()) &CellComplex::GetOcctCompSolid, 
            " "  )
        .def(
            "GetOcctCompSolid", 
            (::TopoDS_CompSolid const &(CellComplex::*)() const ) &CellComplex::GetOcctCompSolid, 
            " "  )
        .def(
            "SetOcctShape", 
            (void(CellComplex::*)(::TopoDS_Shape const &)) &CellComplex::SetOcctShape, 
            " " , py::arg("rkOcctShape") )
        .def(
            "SetOcctCompSolid", 
            (void(CellComplex::*)(::TopoDS_CompSolid const &)) &CellComplex::SetOcctCompSolid, 
            " " , py::arg("rkOcctCompSolid") )
        .def(
            "CenterOfMass", 
            (::std::shared_ptr<TopologicCore::Vertex>(CellComplex::*)() const ) &CellComplex::CenterOfMass, 
            " "  )
        .def(
            "Geometry", 
            (void(CellComplex::*)(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> &) const ) &CellComplex::Geometry, 
            " " , py::arg("rOcctGeometries") )
        .def(
            "GetType", 
            (::TopologicCore::TopologyType(CellComplex::*)() const ) &CellComplex::GetType, 
            " "  )
        .def(
            "GetTypeAsString", 
            (::std::string(CellComplex::*)() const ) &CellComplex::GetTypeAsString, 
            " "  )
        .def(
            "GetClassGUID", 
            (::std::string(CellComplex::*)() const ) &CellComplex::GetClassGUID, 
            " "  )
        .def_static(
            "Type", 
            (int(*)()) &CellComplex::Type, 
            " "  )
        .def(
            "IsContainerType", 
            (bool(CellComplex::*)()) &CellComplex::IsContainerType, 
            " "  )
        .def_static(
            "OcctShapeFix", 
            (::TopoDS_CompSolid(*)(::TopoDS_CompSolid const &)) &CellComplex::OcctShapeFix, 
            " " , py::arg("rkOcctInputCompSolid") )
    ;
}
