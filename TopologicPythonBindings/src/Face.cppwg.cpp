#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "Face.cppwg.hpp"

namespace py = pybind11;
//typedef TopologicCore::Face Face;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
typedef ::std::shared_ptr<TopologicCore::Vertex> _std_shared_ptr_lt_TopologicCore_Vertex_gt_;
typedef ::TopoDS_Shape & _TopoDS_ShapeRef;
typedef ::TopoDS_Shape const & _TopoDS_ShapeconstRef;
typedef ::TopoDS_Face & _TopoDS_FaceRef;
typedef ::TopoDS_Face const & _TopoDS_FaceconstRef;
typedef ::TopologicCore::TopologyType _TopologicCore_TopologyType;
typedef ::std::string _std_string;
typedef ::std::string _std_string;

class Face_Overloads : public Face{
    public:
    using Face::Face;
    /*void Vertices(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>>& rVertices) const  override {
        PYBIND11_OVERLOAD(
            void,
            Face,
            Vertices,
            rVertices);
    } */
    ::std::shared_ptr<TopologicCore::Vertex> CenterOfMass() const  override {
        PYBIND11_OVERLOAD(
            _std_shared_ptr_lt_TopologicCore_Vertex_gt_,
            Face,
            CenterOfMass,
            );
    }
    bool IsManifold(::TopologicCore::Topology::Ptr const& kpHostTopology) const  override {
        PYBIND11_OVERLOAD(
            bool,
            Face,
            IsManifold,
            kpHostTopology);
    }
    void Geometry(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> & rOcctGeometries) const  override {
        PYBIND11_OVERLOAD(
            void,
            Face,
            Geometry,
            rOcctGeometries);
    }
    ::TopoDS_Shape & GetOcctShape() override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShapeRef,
            Face,
            GetOcctShape,
            );
    }
    ::TopoDS_Shape const & GetOcctShape() const  override {
        PYBIND11_OVERLOAD(
            _TopoDS_ShapeconstRef,
            Face,
            GetOcctShape,
            );
    }
    ::TopoDS_Face & GetOcctFace() override {
        PYBIND11_OVERLOAD(
            _TopoDS_FaceRef,
            Face,
            GetOcctFace,
            );
    }
    ::TopoDS_Face const & GetOcctFace() const  override {
        PYBIND11_OVERLOAD(
            _TopoDS_FaceconstRef,
            Face,
            GetOcctFace,
            );
    }
    void SetOcctShape(::TopoDS_Shape const & rkOcctShape) override {
        PYBIND11_OVERLOAD(
            void,
            Face,
            SetOcctShape,
            rkOcctShape);
    }
    ::TopologicCore::TopologyType GetType() const  override {
        PYBIND11_OVERLOAD(
            _TopologicCore_TopologyType,
            Face,
            GetType,
            );
    }
    ::std::string GetTypeAsString() const  override {
        PYBIND11_OVERLOAD(
            _std_string,
            Face,
            GetTypeAsString,
            );
    }
    ::std::string GetClassGUID() const  override {
        PYBIND11_OVERLOAD(
            _std_string,
            Face,
            GetClassGUID,
            );
    }
    bool IsContainerType() override {
        PYBIND11_OVERLOAD(
            bool,
            Face,
            IsContainerType,
            );
    }

};
void register_Face_class(py::module &m){
py::class_<Face , Face_Overloads , std::shared_ptr<Face >  , Topology  >(m, "Face")
        .def(py::init<::TopoDS_Face const &, ::std::string const & >(), py::arg("rkOcctFace"), py::arg("rkGuid") = "")
        .def(
            "AdjacentFaces",
            [](const Face& obj, const TopologicCore::Topology::Ptr& kpParentTopology, py::list& rAdjacentFaces) {
                std::list<Face::Ptr> local;
                obj.AdjacentFaces(kpParentTopology, local);
                   for (auto& x : local)
                        rAdjacentFaces.append(x);
            },
            " ", py::arg("kpParentTopology"), py::arg("rAdjacentFaces"))
        .def(
            "Vertices",
            [](const Face& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rVertices) {
                std::list<Vertex::Ptr> local;
                obj.Vertices(kpHostTopology, local);
                for (auto& x : local)
                    rVertices.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rVertices"))
        .def(
            "Edges",
            [](const Face& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rEdges) {
                std::list<Edge::Ptr> local;
                obj.Edges(kpHostTopology, local);
                for (auto& x : local)
                    rEdges.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rEdges"))

        .def(
            "Wires",
            [](const Face& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rWires) {
                std::list<Wire::Ptr> local;
                obj.Wires(kpHostTopology, local);
                for (auto& x : local)
                    rWires.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rWires"))
        .def(
            "Faces",
            [](const Face& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rFaces) {
                std::list<Face::Ptr> local;
                obj.Faces(kpHostTopology, local);
                for (auto& x : local)
                    rFaces.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rCells"))
        .def(
            "Shells",
            [](const Face& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rShells) {
                std::list<Shell::Ptr> local;
                obj.Shells(kpHostTopology, local);
                for (auto& x : local)
                    rShells.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rShells"))
        .def(
            "Cells",
            [](const Face& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rCells) {
                std::list<Cell::Ptr> local;
                obj.Cells(kpHostTopology, local);
                for (auto& x : local)
                    rCells.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rCells"))
        .def(
            "CellComplexes",
            [](const Face& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rCellComplexes) {
                std::list<CellComplex::Ptr> local;
                obj.CellComplexes(kpHostTopology, local);
                for (auto& x : local)
                    rCellComplexes.append(x);
            },
            " ", py::arg("kpHostTopology"), py::arg("rCellComplexes"))
        .def(
            "CenterOfMass", 
            (::std::shared_ptr<TopologicCore::Vertex>(Face::*)() const ) &Face::CenterOfMass, 
            " "  )
        .def_static(
            "ByExternalBoundary", 
            (::std::shared_ptr<TopologicCore::Face>(*)(::std::shared_ptr<TopologicCore::Wire> const &, const bool)) &Face::ByExternalBoundary, 
            " " , py::arg("kpExternalBoundary"), py::arg("kCopyAttributes") = false)
        .def_static(
            "ByExternalInternalBoundaries", 
            (::std::shared_ptr<TopologicCore::Face>(*)(::std::shared_ptr<TopologicCore::Wire> const &, ::std::list<std::shared_ptr<TopologicCore::Wire>, std::allocator<std::shared_ptr<TopologicCore::Wire>>> const &, const bool)) &Face::ByExternalInternalBoundaries, 
            " " , py::arg("pkExternalBoundary"), py::arg("rkInternalBoundaries"), py::arg("kCopyAttributes") = false)
        .def_static(
            "ByEdges", 
            (::std::shared_ptr<TopologicCore::Face>(*)(::std::list<std::shared_ptr<TopologicCore::Edge>, std::allocator<std::shared_ptr<TopologicCore::Edge>>> const &, const bool)) &Face::ByEdges, 
            " " , py::arg("rkEdges"), py::arg("kCopyAttributes") = false)
        .def_static(
            "BySurface", 
            (::std::shared_ptr<TopologicCore::Face>(*)(::opencascade::handle<Geom_Surface>)) &Face::BySurface, 
            " " , py::arg("pOcctSurface") )
        .def_static(
            "BySurface", 
            (::std::shared_ptr<TopologicCore::Face>(*)(::TColgp_Array2OfPnt const &, ::TColStd_Array2OfReal const &, ::TColStd_Array1OfReal const &, ::TColStd_Array1OfReal const &, ::TColStd_Array1OfInteger const &, ::TColStd_Array1OfInteger const &, int const, int const, bool const, bool const, bool const, ::std::shared_ptr<TopologicCore::Wire> const &, ::std::list<std::shared_ptr<TopologicCore::Wire>, std::allocator<std::shared_ptr<TopologicCore::Wire>>> const &)) &Face::BySurface, 
            " " , py::arg("rkOcctPoles"), py::arg("rkOcctWeights"), py::arg("rkOcctUKnots"), py::arg("rkOcctVKnots"), py::arg("rkOcctUMultiplicities"), py::arg("rkOcctVMultiplicities"), py::arg("kUDegree"), py::arg("kVDegree"), py::arg("kIsUPeriodic"), py::arg("kIsVPeriodic"), py::arg("kIsRational"), py::arg("kpOuterWire"), py::arg("rkInnerWires") )
        /*.def(
            "SharedEdges", 
            (void(Face::*)(::std::shared_ptr<TopologicCore::Face> const &, ::std::list<std::shared_ptr<TopologicCore::Edge>, std::allocator<std::shared_ptr<TopologicCore::Edge>>> &) const ) &Face::SharedEdges, 
            " " , py::arg("kpAnotherFace"), py::arg("rEdges") )*/
        .def(
            "SharedEdges",
            [](const Face& obj, ::std::shared_ptr<TopologicCore::Face> const& kpAnotherFace, py::list& rSharedEdges) {
                std::list<Edge::Ptr> local;
                obj.SharedEdges(kpAnotherFace, local);
                for (auto& x : local)
                    rSharedEdges.append(x);
            },
            " ", py::arg("kpAnotherFace"), py::arg("rSharedEdges"))
        /*.def(
            "SharedVertices", 
            (void(Face::*)(::std::shared_ptr<TopologicCore::Face> const &, ::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> &) const ) &Face::SharedVertices, 
            " " , py::arg("kpAnotherFace"), py::arg("rVertices") )*/
        .def(
            "SharedVertices",
            [](const Face& obj, ::std::shared_ptr<TopologicCore::Face> const& kpAnotherFace, py::list& rSharedVertices) {
                std::list<Vertex::Ptr> local;
                obj.SharedVertices(kpAnotherFace, local);
                for (auto& x : local)
                    rSharedVertices.append(x);
            },
            " ", py::arg("kpAnotherFace"), py::arg("rSharedVertices"))
        .def(
            "ExternalBoundary", 
            (::std::shared_ptr<TopologicCore::Wire>(Face::*)() const ) &Face::ExternalBoundary, 
            " "  )
        /*.def(
            "InternalBoundaries", 
            (void(Face::*)(::std::list<std::shared_ptr<TopologicCore::Wire>, std::allocator<std::shared_ptr<TopologicCore::Wire>>> &) const ) &Face::InternalBoundaries, 
            " " , py::arg("rInternalBoundaries") )*/
        .def(
            "InternalBoundaries",
            [](const Face& obj, py::list& rInternalBoundaries) {
                std::list<Wire::Ptr> local;
                obj.InternalBoundaries(local);
                for (auto& x : local)
                    rInternalBoundaries.append(x);
            },
            " ", py::arg("rInternalBoundaries"))
        .def(
            "AddInternalBoundary", 
            (void(Face::*)(::std::shared_ptr<TopologicCore::Wire> const &)) &Face::AddInternalBoundary, 
            " " , py::arg("kpWire") )
        .def(
            "AddInternalBoundaries", 
            (void(Face::*)(::std::list<std::shared_ptr<TopologicCore::Wire>, std::allocator<std::shared_ptr<TopologicCore::Wire>>> const &)) &Face::AddInternalBoundaries, 
            " " , py::arg("rkWires") )
        .def(
            "IsManifold",
            (bool(Face::*)(::TopologicCore::Topology::Ptr const&) const)& Face::IsManifold,
            " ", py::arg("kpHostTopology"))
        .def(
            "Geometry", 
            (void(Face::*)(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> &) const ) &Face::Geometry, 
            " " , py::arg("rOcctGeometries") )
        .def(
            "GetOcctShape", 
            (::TopoDS_Shape &(Face::*)()) &Face::GetOcctShape, 
            " "  )
        .def(
            "GetOcctShape", 
            (::TopoDS_Shape const &(Face::*)() const ) &Face::GetOcctShape, 
            " "  )
        .def(
            "GetOcctFace", 
            (::TopoDS_Face &(Face::*)()) &Face::GetOcctFace, 
            " "  )
        .def(
            "GetOcctFace", 
            (::TopoDS_Face const &(Face::*)() const ) &Face::GetOcctFace, 
            " "  )
        .def(
            "SetOcctShape", 
            (void(Face::*)(::TopoDS_Shape const &)) &Face::SetOcctShape, 
            " " , py::arg("rkOcctShape") )
        .def(
            "SetOcctFace", 
            (void(Face::*)(::TopoDS_Face const &)) &Face::SetOcctFace, 
            " " , py::arg("rkOcctFace") )
        .def(
            "Surface", 
            (::opencascade::handle<Geom_Surface>(Face::*)() const ) &Face::Surface, 
            " "  )
        .def(
            "GetType", 
            (::TopologicCore::TopologyType(Face::*)() const ) &Face::GetType, 
            " "  )
        .def(
            "GetTypeAsString", 
            (::std::string(Face::*)() const ) &Face::GetTypeAsString, 
            " "  )
        .def(
            "GetClassGUID", 
            (::std::string(Face::*)() const ) &Face::GetClassGUID, 
            " "  )
        .def_static(
            "Type", 
            (int(*)()) &Face::Type, 
            " "  )
        .def(
            "IsContainerType", 
            (bool(Face::*)()) &Face::IsContainerType, 
            " "  )
        /*.def(
            "Triangulate", 
            (void(Face::*)(double const, double const, ::std::list<std::shared_ptr<TopologicCore::Face>, std::allocator<std::shared_ptr<TopologicCore::Face>>> &) const ) &Face::Triangulate, 
            " " , py::arg("kLinearDeflection"), py::arg("kAngularDeflection"), py::arg("rTriangles") ) */
        .def(
            "Triangulate",
            [](const Face& obj, const double kLinearDeflection, const double kAngularDeflection, py::list& rTriangles) {
                std::list<Face::Ptr> local;
                obj.Triangulate(kLinearDeflection, kAngularDeflection, local);
                for (auto& x : local)
                    rTriangles.append(x);
            },
            " ", py::arg("kLinearDeflection"), py::arg("kAngularDeflection"), py::arg("rTriangles"))
        .def_static(
            "OcctShapeFix", 
            (::TopoDS_Face(*)(::TopoDS_Face const &)) &Face::OcctShapeFix, 
            " " , py::arg("rkOcctInputFace") )
    ;
}
