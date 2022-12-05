#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "Cell.cppwg.hpp"

namespace py = pybind11;
//typedef TopologicCore::Cell Cell;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
typedef ::std::shared_ptr<TopologicCore::Vertex> _std_shared_ptr_lt_TopologicCore_Vertex_gt_;
typedef ::TopoDS_Shape & _TopoDS_ShapeRef;
typedef ::TopoDS_Shape const & _TopoDS_ShapeconstRef;
typedef ::TopoDS_Solid & _TopoDS_SolidRef;
typedef ::TopoDS_Solid const & _TopoDS_SolidconstRef;
typedef ::TopologicCore::TopologyType _TopologicCore_TopologyType;
typedef ::std::string _std_string;
typedef ::std::string _std_string;

class Cell_Overloads : public Cell{
    public:
    using Cell::Cell;
    /*void Vertices(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>>& rVertices) const  override {
        PYBIND11_OVERLOAD(
            void,
            Cell,
            Vertices,
            rVertices);
    }*/
    ::std::shared_ptr<TopologicCore::Vertex> CenterOfMass() const  override {
        PYBIND11_OVERLOAD(
            _std_shared_ptr_lt_TopologicCore_Vertex_gt_,
            Cell,
            CenterOfMass,
            );
    }
    bool IsManifold(::TopologicCore::Topology::Ptr const & kpHostTopology) const  override {
        PYBIND11_OVERLOAD(
            bool,
            Cell,
            IsManifold,
            kpHostTopology);
    }
    ::TopoDS_Shape & GetOcctShape() override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShapeRef,
            Cell,
            GetOcctShape,
            );
    }
    ::TopoDS_Shape const & GetOcctShape() const  override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShapeconstRef,
            Cell,
            GetOcctShape,
            );
    }
    ::TopoDS_Solid & GetOcctSolid() override {
        PYBIND11_OVERLOAD(
            _TopoDS_SolidRef,
            Cell,
            GetOcctSolid,
            );
    }
    ::TopoDS_Solid const & GetOcctSolid() const  override {
        PYBIND11_OVERLOAD(
            _TopoDS_SolidconstRef,
            Cell,
            GetOcctSolid,
            );
    }
    void SetOcctShape(::TopoDS_Shape const & rkOcctShape) override {
        PYBIND11_OVERLOAD(
            void,
            Cell,
            SetOcctShape,
            rkOcctShape);
    }
    void Geometry(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> & rOcctGeometries) const  override {
        PYBIND11_OVERLOAD(
            void,
            Cell,
            Geometry,
            rOcctGeometries);
    }
    ::TopologicCore::TopologyType GetType() const  override {
        PYBIND11_OVERLOAD(
            _TopologicCore_TopologyType,
            Cell,
            GetType,
            );
    }
    ::std::string GetTypeAsString() const  override {
        PYBIND11_OVERLOAD(
            _std_string,
            Cell,
            GetTypeAsString,
            );
    }
    ::std::string GetClassGUID() const  override {
        PYBIND11_OVERLOAD(
            _std_string,
            Cell,
            GetClassGUID,
            );
    }
    bool IsContainerType() override {
        PYBIND11_OVERLOAD(
            bool,
            Cell,
            IsContainerType,
            );
    }

};
void register_Cell_class(py::module &m){
py::class_<Cell , Cell_Overloads , std::shared_ptr<Cell >  , Topology  >(m, "Cell")
        .def(py::init<::TopoDS_Solid const &, ::std::string const & >(), py::arg("rkOcctSolid"), py::arg("rkGuid") = "")
        /*.def(
            "AdjacentCells", 
            (void(Cell::*)(::std::list<std::shared_ptr<TopologicCore::Cell>, std::allocator<std::shared_ptr<TopologicCore::Cell>>> &) const ) &Cell::AdjacentCells, 
            " " , py::arg("rAdjacentCells") )*/
        .def(
            "AdjacentCells",
            [](const Cell& obj, const TopologicCore::Topology::Ptr& kpParentTopology, py::list& rAdjacentCells) {
                std::list<Cell::Ptr> local;
                obj.AdjacentCells(kpParentTopology, local);
                for (auto& x : local)
                    rAdjacentCells.append(x);
            },
            " ", py::arg("kpParentTopology"), py::arg("rAdjacentCells"))
        .def(
         "Vertices",
            [](const Cell& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rVertices) {
                std::list<Vertex::Ptr> local;
                obj.Vertices(kpHostTopology, local);
                for (auto& x : local)
                    rVertices.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rVertices"))
        .def(
            "Edges",
            [](const Cell& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rEdges) {
                std::list<Edge::Ptr> local;
                obj.Edges(kpHostTopology, local);
                for (auto& x : local)
                    rEdges.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rEdges"))

        .def(
            "Wires",
            [](const Cell& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rWires) {
                std::list<Wire::Ptr> local;
                obj.Wires(kpHostTopology, local);
                for (auto& x : local)
                    rWires.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rWires"))
        .def(
            "Faces",
            [](const Cell& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rFaces) {
                std::list<Face::Ptr> local;
                obj.Faces(kpHostTopology, local);
                for (auto& x : local)
                    rFaces.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rCells"))
        .def(
            "Shells",
            [](const Cell& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rShells) {
                std::list<Shell::Ptr> local;
                obj.Shells(kpHostTopology, local);
                for (auto& x : local)
                    rShells.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rShells"))
        .def(
            "Cells",
            [](const Cell& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rCells) {
                std::list<Cell::Ptr> local;
                obj.Cells(kpHostTopology, local);
                for (auto& x : local)
                    rCells.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rCells"))
        .def(
            "CellComplexes",
            [](const Cell& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rCellComplexes) {
                std::list<CellComplex::Ptr> local;
                obj.CellComplexes(kpHostTopology, local);
                for (auto& x : local)
                    rCellComplexes.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rCellComplexes"))

        .def(
            "CenterOfMass", 
            (::std::shared_ptr<TopologicCore::Vertex>(Cell::*)() const ) &Cell::CenterOfMass, 
            " "  )
        .def_static(
            "ByFaces", 
            (::std::shared_ptr<TopologicCore::Cell>(*)(::std::list<std::shared_ptr<TopologicCore::Face>, std::allocator<std::shared_ptr<TopologicCore::Face>>> const &, double const, bool const)) &Cell::ByFaces, 
            " " , py::arg("rkFaces"), py::arg("kTolerance") = 0.001, py::arg("kCopyAttributes") = false)
        .def_static(
            "ByShell", 
            (::std::shared_ptr<TopologicCore::Cell>(*)(::std::shared_ptr<TopologicCore::Shell> const &, bool const)) &Cell::ByShell, 
            " " , py::arg("rkShell"), py::arg("kCopyAttributes") = false)
        /*.def(
            "SharedEdges", 
            (void(Cell::*)(::std::shared_ptr<TopologicCore::Cell> const &, ::std::list<std::shared_ptr<TopologicCore::Edge>, std::allocator<std::shared_ptr<TopologicCore::Edge>>> &) const ) &Cell::SharedEdges, 
            " " , py::arg("kpAnotherCell"), py::arg("rEdges") )*/
        .def(
            "SharedEdges",
            [](const Cell& obj, ::std::shared_ptr<TopologicCore::Cell> const& kpAnotherCell, py::list& rSharedEdges) {
                std::list<Edge::Ptr> local;
                obj.SharedEdges(kpAnotherCell, local);
                for (auto& x : local)
                    rSharedEdges.append(x);
            },
            " ", py::arg("kpAnotherCell"), py::arg("rSharedEdges"))
        /*.def(
            "SharedFaces", 
            (void(Cell::*)(::std::shared_ptr<TopologicCore::Cell> const &, ::std::list<std::shared_ptr<TopologicCore::Face>, std::allocator<std::shared_ptr<TopologicCore::Face>>> &) const ) &Cell::SharedFaces, 
            " " , py::arg("kpAnotherCell"), py::arg("rFaces") )*/
        .def(
            "SharedFaces",
            [](const Cell& obj, ::std::shared_ptr<TopologicCore::Cell> const& kpAnotherCell, py::list& rSharedFaces) {
                std::list<Face::Ptr> local;
                obj.SharedFaces(kpAnotherCell, local);
                for (auto& x : local)
                    rSharedFaces.append(x);
            },
            " ", py::arg("kpAnotherCell"), py::arg("rSharedFaces"))
        /*.def(
            "SharedVertices", 
            (void(Cell::*)(::std::shared_ptr<TopologicCore::Cell> const &, ::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> &) const ) &Cell::SharedVertices, 
            " " , py::arg("kpkAnotherCell"), py::arg("rVertices") )*/
        .def(
            "SharedVertices",
            [](const Cell& obj, ::std::shared_ptr<TopologicCore::Cell> const& kpAnotherCell, py::list& rSharedVertices) {
                std::list<Vertex::Ptr> local;
                obj.SharedVertices(kpAnotherCell, local);
                for (auto& x : local)
                    rSharedVertices.append(x);
            },
            " ", py::arg("kpAnotherCell"), py::arg("rSharedVertices"))
        .def(
            "ExternalBoundary", 
            (::std::shared_ptr<TopologicCore::Shell>(Cell::*)() const ) &Cell::ExternalBoundary, 
            " "  )
        /*.def(
            "InternalBoundaries", 
            (void(Cell::*)(::std::list<std::shared_ptr<TopologicCore::Shell>, std::allocator<std::shared_ptr<TopologicCore::Shell>>> &) const ) &Cell::InternalBoundaries, 
            " " , py::arg("rShells") )*/
        .def(
            "InternalBoundaries",
            [](const Cell& obj, py::list& rShells) {
                std::list<Shell::Ptr> local;
                obj.InternalBoundaries(local);
                for (auto& x : local)
                    rShells.append(x);
            },
            " ", py::arg("rShells"))
        .def(
            "IsManifold", 
            (bool(Cell::*)(const Topology::Ptr&) const ) &Cell::IsManifold,
            " ", py::arg("kpHostTopology"))
        .def(
            "GetOcctShape", 
            (::TopoDS_Shape &(Cell::*)()) &Cell::GetOcctShape, 
            " "  )
        .def(
            "GetOcctShape", 
            (::TopoDS_Shape const &(Cell::*)() const ) &Cell::GetOcctShape, 
            " "  )
        .def(
            "GetOcctSolid", 
            (::TopoDS_Solid &(Cell::*)()) &Cell::GetOcctSolid, 
            " "  )
        .def(
            "GetOcctSolid", 
            (::TopoDS_Solid const &(Cell::*)() const ) &Cell::GetOcctSolid, 
            " "  )
        .def(
            "SetOcctShape", 
            (void(Cell::*)(::TopoDS_Shape const &)) &Cell::SetOcctShape, 
            " " , py::arg("rkOcctShape") )
        .def(
            "SetOcctSolid", 
            (void(Cell::*)(::TopoDS_Solid const &)) &Cell::SetOcctSolid, 
            " " , py::arg("rkOcctSolid") )
        .def(
            "Geometry", 
            (void(Cell::*)(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> &) const ) &Cell::Geometry, 
            " " , py::arg("rOcctGeometries") )
        .def(
            "GetType", 
            (::TopologicCore::TopologyType(Cell::*)() const ) &Cell::GetType, 
            " "  )
        .def(
            "GetTypeAsString", 
            (::std::string(Cell::*)() const ) &Cell::GetTypeAsString, 
            " "  )
        .def(
            "GetClassGUID", 
            (::std::string(Cell::*)() const ) &Cell::GetClassGUID, 
            " "  )
        .def_static(
            "Type", 
            (int(*)()) &Cell::Type, 
            " "  )
        .def(
            "IsContainerType", 
            (bool(Cell::*)()) &Cell::IsContainerType, 
            " "  )
        .def_static(
            "OcctShapeFix", 
            (::TopoDS_Solid(*)(::TopoDS_Solid const &)) &Cell::OcctShapeFix, 
            " " , py::arg("rkOcctInputSolid") )
    ;
}
