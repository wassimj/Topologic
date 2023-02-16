#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "Shell.cppwg.hpp"

namespace py = pybind11;
//typedef TopologicCore::Shell Shell;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
typedef ::TopoDS_Shape & _TopoDS_ShapeRef;
typedef ::TopoDS_Shape const & _TopoDS_ShapeconstRef;
typedef ::TopoDS_Shell & _TopoDS_ShellRef;
typedef ::TopoDS_Shell const & _TopoDS_ShellconstRef;
typedef ::std::shared_ptr<TopologicCore::Vertex> _std_shared_ptr_lt_TopologicCore_Vertex_gt_;
typedef ::TopologicCore::TopologyType _TopologicCore_TopologyType;
typedef ::std::string _std_string;
typedef ::std::string _std_string;

class Shell_Overloads : public Shell{
    public:
    using Shell::Shell;
    /*void Vertices(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>>& rVertices) const  override {
        PYBIND11_OVERLOAD(
            void,
            Shell,
            Vertices,
            rVertices);
    }*/
    bool IsManifold(::TopologicCore::Topology::Ptr const & kpHostTopology) const  override {
        PYBIND11_OVERLOAD(
            bool,
            Shell,
            IsManifold,
            kpHostTopology);
    }
    ::TopoDS_Shape & GetOcctShape() override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShapeRef,
            Shell,
            GetOcctShape,
            );
    }
    ::TopoDS_Shape const & GetOcctShape() const  override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShapeconstRef,
            Shell,
            GetOcctShape,
            );
    }
    ::TopoDS_Shell & GetOcctShell() override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShellRef,
            Shell,
            GetOcctShell,
            );
    }
    ::TopoDS_Shell const & GetOcctShell() const  override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShellconstRef,
            Shell,
            GetOcctShell,
            );
    }
    void SetOcctShape(::TopoDS_Shape const & rkOcctShape) override {
        PYBIND11_OVERLOAD(
            void,
            Shell,
            SetOcctShape,
            rkOcctShape);
    }
    void Geometry(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> & rOcctGeometries) const  override {
        PYBIND11_OVERLOAD(
            void,
            Shell,
            Geometry,
            rOcctGeometries);
    }
    ::std::shared_ptr<TopologicCore::Vertex> CenterOfMass() const  override {
        PYBIND11_OVERLOAD(
            _std_shared_ptr_lt_TopologicCore_Vertex_gt_,
            Shell,
            CenterOfMass,
            );
    }
    ::TopologicCore::TopologyType GetType() const  override {
        PYBIND11_OVERLOAD(
            _TopologicCore_TopologyType,
            Shell,
            GetType,
            );
    }
    ::std::string GetTypeAsString() const  override {
        PYBIND11_OVERLOAD(
            _std_string,
            Shell,
            GetTypeAsString,
            );
    }
    ::std::string GetClassGUID() const  override {
        PYBIND11_OVERLOAD(
            _std_string,
            Shell,
            GetClassGUID,
            );
    }
    bool IsContainerType() override {
        PYBIND11_OVERLOAD(
            bool,
            Shell,
            IsContainerType,
            );
    }

};
void register_Shell_class(py::module &m){
py::class_<Shell , Shell_Overloads , std::shared_ptr<Shell >  , Topology  >(m, "Shell")
        .def(py::init<::TopoDS_Shell const &, ::std::string const & >(), py::arg("rkOcctShell"), py::arg("rkGuid") = "")
        .def(
            "Vertices",
            [](const Shell& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rVertices) {
                std::list<Vertex::Ptr> local;
                obj.Vertices(kpHostTopology, local);
                for (auto& x : local)
                    rVertices.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rVertices"))
        .def(
            "Edges",
            [](const Shell& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rEdges) {
            std::list<Edge::Ptr> local;
            obj.Edges(kpHostTopology, local);
            for (auto& x : local)
                rEdges.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rEdges"))

        .def(
            "Wires",
            [](const Shell& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rWires) {
            std::list<Wire::Ptr> local;
            obj.Wires(kpHostTopology, local);
            for (auto& x : local)
                rWires.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rWires"))
        .def(
            "Faces",
            [](const Shell& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rFaces) {
            std::list<Face::Ptr> local;
            obj.Faces(kpHostTopology, local);
            for (auto& x : local)
                rFaces.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rCells"))
        .def(
            "Shells",
            [](const Shell& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rShells) {
            std::list<Shell::Ptr> local;
            obj.Shells(kpHostTopology, local);
            for (auto& x : local)
                rShells.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rShells"))
        .def(
            "Cells",
            [](const Shell& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rCells) {
            std::list<Cell::Ptr> local;
            obj.Cells(kpHostTopology, local);
            for (auto& x : local)
                rCells.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rCells"))
        .def(
            "CellComplexes",
            [](const Shell& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rCellComplexes) {
            std::list<CellComplex::Ptr> local;
            obj.CellComplexes(kpHostTopology, local);
            for (auto& x : local)
                rCellComplexes.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rCellComplexes"))

        .def(
            "IsClosed", 
            (bool(Shell::*)() const ) &Shell::IsClosed, 
            " "  )
        .def_static(
            "ByFaces", 
            (::std::shared_ptr<TopologicCore::Shell>(*)(::std::list<std::shared_ptr<TopologicCore::Face>, std::allocator<std::shared_ptr<TopologicCore::Face>>> const &, double const, bool const)) &Shell::ByFaces, 
            " " , py::arg("rkFaces"), py::arg("kTolerance") = 0.001, py::arg("kCopyAttributes") = false)
        .def(
            "IsManifold", 
            (bool(Shell::*)(const Topology::Ptr &) const ) &Shell::IsManifold,
            " " , py::arg("kpHostTopology"))
        .def(
            "GetOcctShape", 
            (::TopoDS_Shape &(Shell::*)()) &Shell::GetOcctShape, 
            " "  )
        .def(
            "GetOcctShape", 
            (::TopoDS_Shape const &(Shell::*)() const ) &Shell::GetOcctShape, 
            " "  )
        .def(
            "GetOcctShell", 
            (::TopoDS_Shell &(Shell::*)()) &Shell::GetOcctShell, 
            " "  )
        .def(
            "GetOcctShell", 
            (::TopoDS_Shell const &(Shell::*)() const ) &Shell::GetOcctShell, 
            " "  )
        .def(
            "SetOcctShape", 
            (void(Shell::*)(::TopoDS_Shape const &)) &Shell::SetOcctShape, 
            " " , py::arg("rkOcctShape") )
        .def(
            "SetOcctShell", 
            (void(Shell::*)(::TopoDS_Shell const &)) &Shell::SetOcctShell, 
            " " , py::arg("rkOcctShell") )
        .def(
            "Geometry", 
            (void(Shell::*)(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> &) const ) &Shell::Geometry, 
            " " , py::arg("rOcctGeometries") )
        .def(
            "CenterOfMass", 
            (::std::shared_ptr<TopologicCore::Vertex>(Shell::*)() const ) &Shell::CenterOfMass, 
            " "  )
        .def(
            "GetType", 
            (::TopologicCore::TopologyType(Shell::*)() const ) &Shell::GetType, 
            " "  )
        .def(
            "GetTypeAsString", 
            (::std::string(Shell::*)() const ) &Shell::GetTypeAsString, 
            " "  )
        .def(
            "GetClassGUID", 
            (::std::string(Shell::*)() const ) &Shell::GetClassGUID, 
            " "  )
        .def_static(
            "Type", 
            (int(*)()) &Shell::Type, 
            " "  )
        .def(
            "IsContainerType", 
            (bool(Shell::*)()) &Shell::IsContainerType, 
            " "  )
    ;
}
