#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "Cluster.cppwg.hpp"

namespace py = pybind11;
//typedef TopologicCore::Cluster Cluster;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
typedef ::TopoDS_Shape & _TopoDS_ShapeRef;
typedef ::TopoDS_Shape const & _TopoDS_ShapeconstRef;
typedef ::TopoDS_Compound & _TopoDS_CompoundRef;
typedef ::TopoDS_Compound const & _TopoDS_CompoundconstRef;
typedef ::std::shared_ptr<TopologicCore::Vertex> _std_shared_ptr_lt_TopologicCore_Vertex_gt_;
typedef ::TopologicCore::TopologyType _TopologicCore_TopologyType;
typedef ::std::string _std_string;
typedef ::std::string _std_string;

class Cluster_Overloads : public Cluster{
    public:
    using Cluster::Cluster;
    ::TopoDS_Shape & GetOcctShape() override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShapeRef,
            Cluster,
            GetOcctShape,
            );
    }
    ::TopoDS_Shape const & GetOcctShape() const  override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShapeconstRef,
            Cluster,
            GetOcctShape,
            );
    }
    ::TopoDS_Compound & GetOcctCompound() override {
        PYBIND11_OVERLOAD(
            _TopoDS_CompoundRef,
            Cluster,
            GetOcctCompound,
            );
    }
    ::TopoDS_Compound const & GetOcctCompound() const  override {
        PYBIND11_OVERLOAD(
            _TopoDS_CompoundconstRef,
            Cluster,
            GetOcctCompound,
            );
    }
    void SetOcctShape(::TopoDS_Shape const & rkOcctShape) override {
        PYBIND11_OVERLOAD(
            void,
            Cluster,
            SetOcctShape,
            rkOcctShape);
    }
    void Geometry(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> & rOcctGeometries) const  override {
        PYBIND11_OVERLOAD(
            void,
            Cluster,
            Geometry,
            rOcctGeometries);
    }
    void Vertices(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> & rVertices) const  override {
        PYBIND11_OVERLOAD(
            void,
            Cluster,
            Vertices,
            rVertices);
    }
    ::std::shared_ptr<TopologicCore::Vertex> CenterOfMass() const  override {
        PYBIND11_OVERLOAD(
            _std_shared_ptr_lt_TopologicCore_Vertex_gt_,
            Cluster,
            CenterOfMass,
            );
    }
    bool IsManifold() const  override {
        PYBIND11_OVERLOAD(
            bool,
            Cluster,
            IsManifold,
            );
    }
    ::TopologicCore::TopologyType GetType() const  override {
        PYBIND11_OVERLOAD(
            _TopologicCore_TopologyType,
            Cluster,
            GetType,
            );
    }
    ::std::string GetTypeAsString() const  override {
        PYBIND11_OVERLOAD(
            _std_string,
            Cluster,
            GetTypeAsString,
            );
    }
    ::std::string GetClassGUID() const  override {
        PYBIND11_OVERLOAD(
            _std_string,
            Cluster,
            GetClassGUID,
            );
    }
    bool IsContainerType() override {
        PYBIND11_OVERLOAD(
            bool,
            Cluster,
            IsContainerType,
            );
    }

};
void register_Cluster_class(py::module &m){
py::class_<Cluster , Cluster_Overloads , std::shared_ptr<Cluster >  , Topology  >(m, "Cluster")
        .def(py::init<::TopoDS_Compound const &, ::std::string const & >(), py::arg("rkOcctCompound"), py::arg("rkGuid") = "")
        .def_static(
            "ByTopologies", 
            (::std::shared_ptr<TopologicCore::Cluster>(*)(::std::list<std::shared_ptr<TopologicCore::Topology>, std::allocator<std::shared_ptr<TopologicCore::Topology>>> const &)) &Cluster::ByTopologies, 
            " " , py::arg("rkTopologies") )
        .def_static(
            "ByOcctTopologies", 
            (::TopoDS_Compound(*)(::TopTools_MapOfShape const &)) &Cluster::ByOcctTopologies, 
            " " , py::arg("rkOcctShapes") )
        .def(
            "AddTopology", 
            (bool(Cluster::*)(::TopologicCore::Topology const * const)) &Cluster::AddTopology, 
            " " , py::arg("kpkTopology") )
        .def(
            "RemoveTopology", 
            (bool(Cluster::*)(::TopologicCore::Topology const * const)) &Cluster::RemoveTopology, 
            " " , py::arg("kpkTopology") )
        .def(
            "GetOcctShape", 
            (::TopoDS_Shape &(Cluster::*)()) &Cluster::GetOcctShape, 
            " "  )
        .def(
            "GetOcctShape", 
            (::TopoDS_Shape const &(Cluster::*)() const ) &Cluster::GetOcctShape, 
            " "  )
        .def(
            "GetOcctCompound", 
            (::TopoDS_Compound &(Cluster::*)()) &Cluster::GetOcctCompound, 
            " "  )
        .def(
            "GetOcctCompound", 
            (::TopoDS_Compound const &(Cluster::*)() const ) &Cluster::GetOcctCompound, 
            " "  )
        .def(
            "SetOcctShape", 
            (void(Cluster::*)(::TopoDS_Shape const &)) &Cluster::SetOcctShape, 
            " " , py::arg("rkOcctShape") )
        .def(
            "SetOcctCompound", 
            (void(Cluster::*)(::TopoDS_Compound const &)) &Cluster::SetOcctCompound, 
            " " , py::arg("rkOcctCompound") )
        .def(
            "Geometry", 
            (void(Cluster::*)(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> &) const ) &Cluster::Geometry, 
            " " , py::arg("rOcctGeometries") )
        /*.def(
            "Shells", 
            (void(Cluster::*)(::std::list<std::shared_ptr<TopologicCore::Shell>, std::allocator<std::shared_ptr<TopologicCore::Shell>>> &) const ) &Cluster::Shells, 
            " " , py::arg("rShells") )
        .def(
            "Edges", 
            (void(Cluster::*)(::std::list<std::shared_ptr<TopologicCore::Edge>, std::allocator<std::shared_ptr<TopologicCore::Edge>>> &) const ) &Cluster::Edges, 
            " " , py::arg("rEdges") )
        .def(
            "Faces", 
            (void(Cluster::*)(::std::list<std::shared_ptr<TopologicCore::Face>, std::allocator<std::shared_ptr<TopologicCore::Face>>> &) const ) &Cluster::Faces, 
            " " , py::arg("rFaces") )
        .def(
            "Vertices", 
            (void(Cluster::*)(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> &) const ) &Cluster::Vertices, 
            " " , py::arg("rVertices") )
        .def(
            "Wires", 
            (void(Cluster::*)(::std::list<std::shared_ptr<TopologicCore::Wire>, std::allocator<std::shared_ptr<TopologicCore::Wire>>> &) const ) &Cluster::Wires, 
            " " , py::arg("rWires") )
        .def(
            "Cells", 
            (void(Cluster::*)(::std::list<std::shared_ptr<TopologicCore::Cell>, std::allocator<std::shared_ptr<TopologicCore::Cell>>> &) const ) &Cluster::Cells, 
            " " , py::arg("rCells") )
        .def(
            "CellComplexes", 
            (void(Cluster::*)(::std::list<std::shared_ptr<TopologicCore::CellComplex>, std::allocator<std::shared_ptr<TopologicCore::CellComplex>>> &) const ) &Cluster::CellComplexes, 
            " " , py::arg("rCellComplexes") )*/

        .def(
            "CellComplexes",
            [](const Cluster& obj, py::list& rCellComplexes) {
                std::list<CellComplex::Ptr> local;
                obj.CellComplexes(local);
                for (auto& x : local)
                    rCellComplexes.append(x);
            },
            " ", py::arg("rCellComplexes"))

        .def(
            "Shells",
            [](const Cluster& obj, py::list& rShells) {
                std::list<Shell::Ptr> local;
                obj.Shells(local);
                for (auto& x : local)
                    rShells.append(x);
            },
            " ", py::arg("rShells"))

        .def(
            "Edges",
            [](const Cluster& obj, py::list& rEdges) {
                std::list<Edge::Ptr> local;
                obj.Edges(local);
                for (auto& x : local)
                    rEdges.append(x);
            },
            " ", py::arg("rEdges"))

        .def(
            "Faces",
            [](const Cluster& obj, py::list& rFaces) {
                std::list<Face::Ptr> local;
                obj.Faces(local);
                for (auto& x : local)
                    rFaces.append(x);
            },
            " ", py::arg("rFaces"))

        .def(
            "Vertices",
            [](const Cluster& obj, py::list& rVertices) {
                std::list<Vertex::Ptr> local;
                obj.Vertices(local);
                for (auto& x : local)
                    rVertices.append(x);
            },
            " ", py::arg("rVertices"))

        .def(
            "Wires",
            [](const Cluster& obj, py::list& rWires) {
                std::list<Wire::Ptr> local;
                obj.Wires(local);
                for (auto& x : local)
                    rWires.append(x);
            },
            " ", py::arg("rWires"))

        .def(
            "CenterOfMass", 
            (::std::shared_ptr<TopologicCore::Vertex>(Cluster::*)() const ) &Cluster::CenterOfMass, 
            " "  )
        .def(
            "IsManifold", 
            (bool(Cluster::*)() const ) &Cluster::IsManifold, 
            " "  )
        .def(
            "GetType", 
            (::TopologicCore::TopologyType(Cluster::*)() const ) &Cluster::GetType, 
            " "  )
        .def(
            "GetTypeAsString", 
            (::std::string(Cluster::*)() const ) &Cluster::GetTypeAsString, 
            " "  )
        .def(
            "GetClassGUID", 
            (::std::string(Cluster::*)() const ) &Cluster::GetClassGUID, 
            " "  )
        .def_static(
            "Type", 
            (int(*)()) &Cluster::Type, 
            " "  )
        .def(
            "IsContainerType", 
            (bool(Cluster::*)()) &Cluster::IsContainerType, 
            " "  )
    ;
}
