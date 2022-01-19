#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "Edge.cppwg.hpp"

namespace py = pybind11;
//typedef TopologicCore::Edge Edge;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
typedef ::TopoDS_Shape & _TopoDS_ShapeRef;
typedef ::TopoDS_Shape const & _TopoDS_ShapeconstRef;
typedef ::TopoDS_Edge & _TopoDS_EdgeRef;
typedef ::TopoDS_Edge const & _TopoDS_EdgeconstRef;
typedef ::std::shared_ptr<TopologicCore::Vertex> _std_shared_ptr_lt_TopologicCore_Vertex_gt_;
typedef ::TopologicCore::TopologyType _TopologicCore_TopologyType;
typedef ::std::string _std_string;
typedef ::std::string _std_string;

class Edge_Overloads : public Edge{
    public:
    using Edge::Edge;
    /*void Vertices(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>>& rVertices) const  override {
        PYBIND11_OVERLOAD(
            void,
            Edge,
            Vertices,
            rVertices);
    }*/
    bool IsManifold(::TopologicCore::Topology::Ptr const& kpHostTopology) const  override {
        PYBIND11_OVERLOAD(
            bool,
            Edge,
            IsManifold,
            kpHostTopology);
    }
    /*bool IsManifold(::TopologicCore::Topology::Ptr const& rkParentTopology) const  override {
        PYBIND11_OVERLOAD(
            bool,
            Edge,
            IsManifold,
            rkParentTopology);
    } */
    void Geometry(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> & rOcctGeometries) const  override {
        PYBIND11_OVERLOAD(
            void,
            Edge,
            Geometry,
            rOcctGeometries);
    }
    ::TopoDS_Shape & GetOcctShape() override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShapeRef,
            Edge,
            GetOcctShape,
            );
    }
    ::TopoDS_Shape const & GetOcctShape() const  override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShapeconstRef,
            Edge,
            GetOcctShape,
            );
    }
    ::TopoDS_Edge & GetOcctEdge() override {
        PYBIND11_OVERLOAD(
            _TopoDS_EdgeRef,
            Edge,
            GetOcctEdge,
            );
    }
    ::TopoDS_Edge const & GetOcctEdge() const  override {
        PYBIND11_OVERLOAD(
            _TopoDS_EdgeconstRef,
            Edge,
            GetOcctEdge,
            );
    }
    void SetOcctShape(::TopoDS_Shape const & rkOcctShape) override {
        PYBIND11_OVERLOAD(
            void,
            Edge,
            SetOcctShape,
            rkOcctShape);
    }
    ::std::shared_ptr<TopologicCore::Vertex> CenterOfMass() const  override {
        PYBIND11_OVERLOAD(
            _std_shared_ptr_lt_TopologicCore_Vertex_gt_,
            Edge,
            CenterOfMass,
            );
    }
    ::TopologicCore::TopologyType GetType() const  override {
        PYBIND11_OVERLOAD(
            _TopologicCore_TopologyType,
            Edge,
            GetType,
            );
    }
    ::std::string GetTypeAsString() const  override {
        PYBIND11_OVERLOAD(
            _std_string,
            Edge,
            GetTypeAsString,
            );
    }
    ::std::string GetClassGUID() const  override {
        PYBIND11_OVERLOAD(
            _std_string,
            Edge,
            GetClassGUID,
            );
    }
    bool IsContainerType() override {
        PYBIND11_OVERLOAD(
            bool,
            Edge,
            IsContainerType,
            );
    }

};
void register_Edge_class(py::module &m){
py::class_<Edge , Edge_Overloads , std::shared_ptr<Edge >  , Topology  >(m, "Edge")
        .def(py::init<::TopoDS_Edge const &, ::std::string const & >(), py::arg("rkOcctEdge"), py::arg("rkGuid") = "")
        .def(
            "AdjacentEdges",
            [](const Edge& obj, const TopologicCore::Topology::Ptr& kpHostTopology, py::list& rAdjacentEdges) {
                std::list<Edge::Ptr> local;
                obj.AdjacentEdges(kpHostTopology, local);
                for (auto& x : local)
                rAdjacentEdges.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rAdjacentEdges"))
        .def(
            "StartVertex", 
            (::std::shared_ptr<TopologicCore::Vertex>(Edge::*)() const ) &Edge::StartVertex, 
            " "  )
        .def(
            "EndVertex", 
            (::std::shared_ptr<TopologicCore::Vertex>(Edge::*)() const ) &Edge::EndVertex, 
            " "  )
        .def(
            "Vertices",
            [](const Edge& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rVertices) {
                std::list<Vertex::Ptr> local;
                obj.Vertices(kpHostTopology, local);
                for (auto& x : local)
                    rVertices.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rVertices"))
        .def(
            "Edges",
            [](const Edge& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rEdges) {
                std::list<Edge::Ptr> local;
                obj.Edges(kpHostTopology, local);
                for (auto& x : local)
                    rEdges.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rEdges"))

        .def(
            "Wires",
            [](const Edge& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rWires) {
                std::list<Wire::Ptr> local;
                obj.Wires(kpHostTopology, local);
                for (auto& x : local)
                    rWires.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rWires"))
        .def(
            "Faces",
            [](const Edge& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rFaces) {
                std::list<Face::Ptr> local;
                obj.Faces(kpHostTopology, local);
                for (auto& x : local)
                    rFaces.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rCells"))
        .def(
            "Shells",
            [](const Edge& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rShells) {
                std::list<Shell::Ptr> local;
                obj.Shells(kpHostTopology, local);
                for (auto& x : local)
                    rShells.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rShells"))
        .def(
            "Cells",
            [](const Edge& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rCells) {
                std::list<Cell::Ptr> local;
                obj.Cells(kpHostTopology, local);
                for (auto& x : local)
                    rCells.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rCells"))
        .def(
            "CellComplexes",
            [](const Edge& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rCellComplexes) {
                std::list<CellComplex::Ptr> local;
                obj.CellComplexes(kpHostTopology, local);
                for (auto& x : local)
                    rCellComplexes.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rCellComplexes"))
        .def_static(
            "ByCurve", 
            (::TopologicCore::Edge::Ptr(*)(::TColgp_Array1OfPnt const &, ::TColStd_Array1OfReal const &, ::TColStd_Array1OfReal const &, ::TColStd_Array1OfInteger const &, int const, bool const, bool const)) &Edge::ByCurve, 
            " " , py::arg("rkOcctPoles"), py::arg("rkOcctWeights"), py::arg("rkOcctKnots"), py::arg("rkOcctMultiplicities"), py::arg("kDegree"), py::arg("kIsPeriodic") = false, py::arg("kIsRational") = true )
        .def_static(
            "ByCurve", 
            (::TopologicCore::Edge::Ptr(*)(::opencascade::handle<Geom_Curve>, double const, double const)) &Edge::ByCurve, 
            " " , py::arg("pOcctCurve"), py::arg("rkFirstParameter") = 0., py::arg("rkLastParameter") = 1. )
        .def_static(
            "ByStartVertexEndVertex", 
            (::TopologicCore::Edge::Ptr(*)(::std::shared_ptr<TopologicCore::Vertex> const &, ::std::shared_ptr<TopologicCore::Vertex> const &, bool const)) &Edge::ByStartVertexEndVertex, 
            " " , py::arg("kpStartVertex"), py::arg("kpEndVertex"), py::arg("kCopyAttributes") = false )
        .def(
            "SharedVertices",
            [](const Edge& obj, ::std::shared_ptr<TopologicCore::Edge> const & kpAnotherEdge, py::list& rSharedVertices) {
                std::list<Vertex::Ptr> local;
                obj.SharedVertices(kpAnotherEdge, local);
                for (auto& x : local)
                    rSharedVertices.append(x);
            },
            " ", py::arg("kpAnotherEdge"), py::arg("rSharedVertices"))
        /*.def(
            "IsManifold", 
            (bool(Edge::*)() const ) &Edge::IsManifold, 
            " "  )*/
        .def(
            "IsManifold", 
            (bool(Edge::*)(::TopologicCore::Topology::Ptr const &) const ) &Edge::IsManifold, 
            " " , py::arg("kpHostTopology") )
        .def(
            "Geometry", 
            (void(Edge::*)(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> &) const ) &Edge::Geometry, 
            " " , py::arg("rOcctGeometries") )
        .def(
            "GetOcctShape", 
            (::TopoDS_Shape &(Edge::*)()) &Edge::GetOcctShape, 
            " "  )
        .def(
            "GetOcctShape", 
            (::TopoDS_Shape const &(Edge::*)() const ) &Edge::GetOcctShape, 
            " "  )
        .def(
            "GetOcctEdge", 
            (::TopoDS_Edge &(Edge::*)()) &Edge::GetOcctEdge, 
            " "  )
        .def(
            "GetOcctEdge", 
            (::TopoDS_Edge const &(Edge::*)() const ) &Edge::GetOcctEdge, 
            " "  )
        .def(
            "SetOcctShape", 
            (void(Edge::*)(::TopoDS_Shape const &)) &Edge::SetOcctShape, 
            " " , py::arg("rkOcctShape") )
        .def(
            "SetOcctEdge", 
            (void(Edge::*)(::TopoDS_Edge const &)) &Edge::SetOcctEdge, 
            " " , py::arg("rkOcctEdge") )
        .def(
            "Curve", 
            (::opencascade::handle<Geom_Curve>(Edge::*)() const ) &Edge::Curve, 
            " "  )
        .def(
            "Curve", 
            (::opencascade::handle<Geom_Curve>(Edge::*)(double &, double &) const ) &Edge::Curve, 
            " " , py::arg("rFirstParameter"), py::arg("rSecondParameter") )
        .def(
            "CenterOfMass", 
            (::std::shared_ptr<TopologicCore::Vertex>(Edge::*)() const ) &Edge::CenterOfMass, 
            " "  )
        .def(
            "GetType", 
            (::TopologicCore::TopologyType(Edge::*)() const ) &Edge::GetType, 
            " "  )
        .def(
            "GetTypeAsString", 
            (::std::string(Edge::*)() const ) &Edge::GetTypeAsString, 
            " "  )
        .def(
            "GetClassGUID", 
            (::std::string(Edge::*)() const ) &Edge::GetClassGUID, 
            " "  )
        .def_static(
            "Throw", 
            (void(*)(::BRepBuilderAPI_EdgeError const)) &Edge::Throw, 
            " " , py::arg("occtEdgeError") )
        .def_static(
            "NormalizeParameter", 
            (double(*)(double const, double const, double const)) &Edge::NormalizeParameter, 
            " " , py::arg("kOcctFirstParameter"), py::arg("kOcctLastParameter"), py::arg("kNonNormalizedParameter") )
        .def_static(
            "NonNormalizeParameter", 
            (double(*)(double const, double const, double const)) &Edge::NonNormalizeParameter, 
            " " , py::arg("kOcctFirstParameter"), py::arg("kOcctLastParameter"), py::arg("kNormalizedParameter") )
        .def_static(
            "Type", 
            (int(*)()) &Edge::Type, 
            " "  )
        .def(
            "IsContainerType", 
            (bool(Edge::*)()) &Edge::IsContainerType, 
            " "  )
        .def_static(
            "OcctShapeFix", 
            (::TopoDS_Edge(*)(::TopoDS_Edge const &)) &Edge::OcctShapeFix, 
            " " , py::arg("rkOcctInputEdge") )
    ;
}
