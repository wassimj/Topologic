#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "Topology.cppwg.hpp"

namespace py = pybind11;
//typedef TopologicCore::Topology Topology;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);
typedef ::TopoDS_Shape& _TopoDS_ShapeRef;
typedef ::TopoDS_Shape const& _TopoDS_ShapeconstRef;
typedef ::TopologicCore::TopologyType _TopologicCore_TopologyType;
typedef ::std::string _std_string;
typedef ::std::shared_ptr<TopologicCore::Vertex> _std_shared_ptr_lt_TopologicCore_Vertex_gt_;
typedef ::std::string _std_string;

void register_TopoDS_Shape_class(py::module& m) {
    py::class_<TopoDS_Shape, std::shared_ptr<TopoDS_Shape>>(m, "TopoDS_Shape")
        .def(py::init<>())
        ;
}

class Topology_Overloads : public Topology {
public:
    using Topology::Topology;
    void Geometry(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>>& rOcctGeometries) const  override {
        PYBIND11_OVERLOAD_PURE(
            void,
            Topology,
            Geometry,
            rOcctGeometries);
    }
    bool IsManifold(::TopologicCore::Topology::Ptr const& kpHostTopology) const  override {
        PYBIND11_OVERLOAD_PURE(
            bool,
            Topology,
            IsManifold,
            kpHostTopology);
    }
    ::TopoDS_Shape& GetOcctShape() override {
        PYBIND11_OVERLOAD_PURE(
            _TopoDS_ShapeRef,
            Topology,
            GetOcctShape,
            );
    }
    void SetOcctShape(::TopoDS_Shape const& rkOcctShape) override {
        PYBIND11_OVERLOAD_PURE(
            void,
            Topology,
            SetOcctShape,
            rkOcctShape);
    }
    ::TopoDS_Shape const& GetOcctShape() const  override {
        PYBIND11_OVERLOAD_PURE(
            _TopoDS_ShapeconstRef,
            Topology,
            GetOcctShape,
            );
    }
    ::TopologicCore::TopologyType GetType() const  override {
        PYBIND11_OVERLOAD_PURE(
            _TopologicCore_TopologyType,
            Topology,
            GetType,
            );
    }
    ::std::string GetTypeAsString() const  override {
        PYBIND11_OVERLOAD_PURE(
            _std_string,
            Topology,
            GetTypeAsString,
            );
    }
    void Vertices(::TopologicCore::Topology::Ptr const& kpHostTopology, ::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>>& rVertices) const  override {
        PYBIND11_OVERLOAD(
            void,
            Topology,
            Vertices,
            kpHostTopology,
            rVertices);
    }
    ::std::shared_ptr<TopologicCore::Vertex> CenterOfMass() const  override {
        PYBIND11_OVERLOAD_PURE(
            _std_shared_ptr_lt_TopologicCore_Vertex_gt_,
            Topology,
            CenterOfMass,
            );
    }
    ::std::string GetClassGUID() const  override {
        PYBIND11_OVERLOAD_PURE(
            _std_string,
            Topology,
            GetClassGUID,
            );
    }
    bool IsContainerType() override {
        PYBIND11_OVERLOAD_PURE(
            bool,
            Topology,
            IsContainerType,
            );
    }

};
void register_Topology_class(py::module& m) {
    py::class_<Topology, Topology_Overloads, std::shared_ptr<Topology >, TopologicalQuery  >(m, "Topology")
        .def_static(
            "ByOcctShape",
            (::TopologicCore::Topology::Ptr(*)(::TopoDS_Shape const&, ::std::string const&)) & Topology::ByOcctShape,
            " ", py::arg("rkOcctShape"), py::arg("rkInstanceGuid") = "")
        .def_static(
            "ByGeometry",
            (::TopologicCore::Topology::Ptr(*)(::opencascade::handle<Geom_Geometry>)) & Topology::ByGeometry,
            " ", py::arg("pGeometry"))
        .def_static(
            "ByContext",
            (::TopologicCore::Topology::Ptr(*)(::std::shared_ptr<TopologicCore::Context> const&)) & Topology::ByContext,
            " ", py::arg("kpContext"))
        .def_static(
            "ByVertexIndex",
            (void(*)(::std::vector<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> const&, ::std::list<std::list<int, std::allocator<int>>, std::allocator<std::list<int, std::allocator<int>>>> const&, ::std::list<std::shared_ptr<TopologicCore::Topology>, std::allocator<std::shared_ptr<TopologicCore::Topology>>> &)) & Topology::ByVertexIndex,
            " ", py::arg("rkVertices"), py::arg("rkVertexIndices"), py::arg("rTopologies"))
        .def_static(
            "ByFaces",
            (::std::shared_ptr<TopologicCore::Topology>(*)(::std::list<std::shared_ptr<TopologicCore::Face>, std::allocator<std::shared_ptr<TopologicCore::Face>>> const&, double const)) & Topology::ByFaces,
            " ", py::arg("rkFaces"), py::arg("kTolerance") = 0.001)
        .def(
            "AddContent",
            (void(Topology::*)(::TopologicCore::Topology::Ptr const&)) & Topology::AddContent,
            " ", py::arg("rkTopology"))
        .def(
            "AddContent",
            (::TopologicCore::Topology::Ptr(Topology::*)(::TopologicCore::Topology::Ptr const&, int const)) & Topology::AddContent,
            " ", py::arg("rkTopology"), py::arg("kTypeFilter"))
        .def(
            "AddContents",
            (::TopologicCore::Topology::Ptr(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Topology>, std::allocator<std::shared_ptr<TopologicCore::Topology>>> const&, int const)) & Topology::AddContents,
            " ", py::arg("rkContentTopologies"), py::arg("kTypeFilter"))
        .def(
            "RemoveContent",
            (void(Topology::*)(::TopologicCore::Topology::Ptr const&)) & Topology::RemoveContent,
            " ", py::arg("rkTopology"))
        .def(
            "RemoveContents",
            (::TopologicCore::Topology::Ptr(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Topology>, std::allocator<std::shared_ptr<TopologicCore::Topology>>> const&)) & Topology::RemoveContents,
            " ", py::arg("rkTopologies"))
        .def(
            "AddContext",
            (void(Topology::*)(::std::shared_ptr<TopologicCore::Context> const&)) & Topology::AddContext,
            " ", py::arg("rkContext"))
        .def(
            "AddContexts",
            (::TopologicCore::Topology::Ptr(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Context>, std::allocator<std::shared_ptr<TopologicCore::Context>>> const&)) & Topology::AddContexts,
            " ", py::arg("rkContexts"))
        .def(
            "RemoveContext",
            (void(Topology::*)(::std::shared_ptr<TopologicCore::Context> const&)) & Topology::RemoveContext,
            " ", py::arg("rkContext"))
        .def(
            "RemoveContexts",
            (::TopologicCore::Topology::Ptr(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Context>, std::allocator<std::shared_ptr<TopologicCore::Context>>> const&)) & Topology::RemoveContexts,
            " ", py::arg("rkContexts"))
        /*.def(
            "SharedTopologies",
            (void(Topology::*)(::TopologicCore::Topology::Ptr const &, int const, ::std::list<std::shared_ptr<TopologicCore::Topology>, std::allocator<std::shared_ptr<TopologicCore::Topology>>> &) const ) &Topology::SharedTopologies,
            " " , py::arg("kpTopology"), py::arg("kFilterType"), py::arg("rSharedTopologies") )*/
        .def(
            "SharedTopologies",
            [](const Topology& obj, ::std::shared_ptr<TopologicCore::Topology> const& kpTopology, int const kFilterType, py::list& rSharedTopologies) {
        std::list<Topology::Ptr> local;
        obj.SharedTopologies(kpTopology, kFilterType, local);
        for (auto& x : local)
            rSharedTopologies.append(x);
    },
            " ", py::arg("kpTopology"), py::arg("kFilterType"), py::arg("rSharedTopologies"))
        .def(
            "SetDictionaries",
            (::TopologicCore::Topology::Ptr(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> const&, ::std::list<std::map<std::basic_string<char>, std::shared_ptr<TopologicCore::Attribute>, std::less<std::basic_string<char>>, std::allocator<std::pair<const std::basic_string<char>, std::shared_ptr<TopologicCore::Attribute>>>>, std::allocator<std::map<std::basic_string<char>, std::shared_ptr<TopologicCore::Attribute>, std::less<std::basic_string<char>>, std::allocator<std::pair<const std::basic_string<char>, std::shared_ptr<TopologicCore::Attribute>>>>>> const&, int const)) & Topology::SetDictionaries,
            " ", py::arg("rkSelectors"), py::arg("rkDictionaries"), py::arg("kTypeFilter") = (int const)(::TopologicCore::TopologyType::TOPOLOGY_ALL))
        .def(
            "SetDictionaries",
            (::TopologicCore::Topology::Ptr(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> const&, ::std::list<std::map<std::basic_string<char>, std::shared_ptr<TopologicCore::Attribute>, std::less<std::basic_string<char>>, std::allocator<std::pair<const std::basic_string<char>, std::shared_ptr<TopologicCore::Attribute>>>>, std::allocator<std::map<std::basic_string<char>, std::shared_ptr<TopologicCore::Attribute>, std::less<std::basic_string<char>>, std::allocator<std::pair<const std::basic_string<char>, std::shared_ptr<TopologicCore::Attribute>>>>>> const&, ::std::list<int, std::allocator<int>> const&, bool const)) & Topology::SetDictionaries,
            " ", py::arg("rkSelectors"), py::arg("rkDictionaries"), py::arg("rkTypeFilters"), py::arg("expectDuplicateTopologies") = false)
        .def_static(
            "OcctSewFaces",
            (::TopoDS_Shape(*)(::TopTools_ListOfShape const&, double const)) & Topology::OcctSewFaces,
            " ", py::arg("rkOcctFaces"), py::arg("kTolerance") = 0.001)
        .def(
            "Difference",
            (::TopologicCore::Topology::Ptr(Topology::*)(::TopologicCore::Topology::Ptr const&, bool const)) & Topology::Difference,
            " ", py::arg("kpOtherTopology") = nullptr, py::arg("kTransferDictionary") = false)
        .def(
            "Impose",
            (::TopologicCore::Topology::Ptr(Topology::*)(::TopologicCore::Topology::Ptr const&, bool const)) & Topology::Impose,
            " ", py::arg("kpTool") = nullptr, py::arg("kTransferDictionary") = false)
        .def(
            "Imprint",
            (::TopologicCore::Topology::Ptr(Topology::*)(::TopologicCore::Topology::Ptr const&, bool const)) & Topology::Imprint,
            " ", py::arg("kpTool") = nullptr, py::arg("kTransferDictionary") = false)
        .def(
            "Intersect",
            (::TopologicCore::Topology::Ptr(Topology::*)(::TopologicCore::Topology::Ptr const&, bool const)) & Topology::Intersect,
            " ", py::arg("kpOtherTopology") = nullptr, py::arg("kTransferDictionary") = false)
        .def(
            "Merge",
            (::TopologicCore::Topology::Ptr(Topology::*)(::TopologicCore::Topology::Ptr const&, bool const)) & Topology::Merge,
            " ", py::arg("kpOtherTopology") = nullptr, py::arg("kTransferDictionary") = false)
        .def(
            "SelfMerge",
            (::TopologicCore::Topology::Ptr(Topology::*)()) & Topology::SelfMerge,
            " ")
        .def(
            "Slice",
            (::TopologicCore::Topology::Ptr(Topology::*)(::TopologicCore::Topology::Ptr const&, bool const)) & Topology::Slice,
            " ", py::arg("kpTool") = nullptr, py::arg("kTransferDictionary") = false)
        .def(
            "Union",
            (::TopologicCore::Topology::Ptr(Topology::*)(::TopologicCore::Topology::Ptr const&, bool const)) & Topology::Union,
            " ", py::arg("kpOtherTopology") = nullptr, py::arg("kTransferDictionary") = false)
        .def(
            "XOR",
            (::TopologicCore::Topology::Ptr(Topology::*)(::TopologicCore::Topology::Ptr const&, bool const)) & Topology::XOR,
            " ", py::arg("kpOtherTopology") = nullptr, py::arg("kTransferDictionary") = false)
        .def(
            "Divide",
            (::TopologicCore::Topology::Ptr(Topology::*)(::TopologicCore::Topology::Ptr const&, bool const)) & Topology::Divide,
            " ", py::arg("kpTool") = nullptr, py::arg("kTransferDictionary") = false)
        .def_static(
            "TransferContents",
            (void(*)(::TopoDS_Shape const&, ::TopologicCore::Topology::Ptr const&)) & Topology::TransferContents,
            " ", py::arg("rkOcctShape1"), py::arg("kpTopology2"))
        .def_static(
            "TransferContents",
            (void(*)(::TopoDS_Shape const&, ::TopologicCore::Topology::Ptr const&, ::TopTools_ListOfShape const&)) & Topology::TransferContents,
            " ", py::arg("rkOcctShape1"), py::arg("kpTopology2"), py::arg("rkOcctDeletedSubshapes"))
        .def(
            "Geometry",
            (void(Topology::*)(::std::list<opencascade::handle<Geom_Geometry>, std::allocator<opencascade::handle<Geom_Geometry>>> &) const) & Topology::Geometry,
            " ", py::arg("rOcctGeometries"))
        /*.def(
            "Contents",
            (void(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Topology>, std::allocator<std::shared_ptr<TopologicCore::Topology>>> &) const ) &Topology::Contents,
            " " , py::arg("rContents") )*/
        .def(
            "Contents",
            [](const Topology& obj, py::list& rContents) {
        std::list<Topology::Ptr> local;
        obj.Contents(local);
        for (auto& x : local)
            rContents.append(x);
    },
            " ", py::arg("rContents"))
        /*.def(
            "Apertures",
            (void(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Aperture>, std::allocator<std::shared_ptr<TopologicCore::Aperture>>> &) const ) &Topology::Apertures,
            " " , py::arg("rApertures") )*/
        .def(
            "Apertures",
            [](const Topology& obj, py::list& rApertures) {
        std::list<Aperture::Ptr> local;
        obj.Apertures(local);
        for (auto& x : local)
            rApertures.append(x);
    },
            " ", py::arg("rApertures"))
        /*.def(
            "SubContents",
            (void(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Topology>, std::allocator<std::shared_ptr<TopologicCore::Topology>>> &) const ) &Topology::SubContents,
            " " , py::arg("rSubContents") )*/
        .def(
            "SubContents",
            [](const Topology& obj, py::list& rSubContents) {
        std::list<Topology::Ptr> local;
        obj.SubContents(local);
        for (auto& x : local)
            rSubContents.append(x);
    },
            " ", py::arg("rSubContents"))
        /*.def(
            "Contexts",
            (bool(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Context>, std::allocator<std::shared_ptr<TopologicCore::Context>>> &) const ) &Topology::Contexts,
            " " , py::arg("rContexts") )*/
        .def(
            "Contexts",
            [](const Topology& obj, py::list& rContexts) {
        std::list<Context::Ptr> local;
        obj.Contexts(local);
        for (auto& x : local)
            rContexts.append(x);
    },
            " ", py::arg("rContexts"))
        .def(
            "IsManifold",
            (bool(Topology::*)(::TopologicCore::Topology::Ptr const&) const) & Topology::IsManifold,
            " ", py::arg("kpHostTopology"))
        .def(
            "ExportToBRep",
            (bool(Topology::*)(::std::string const&) const) & Topology::ExportToBRep,
            " ", py::arg("rkFilePath"))
        .def_static(
            "ByImportedBRep",
            (::TopologicCore::Topology::Ptr(*)(::std::string const&)) & Topology::ByImportedBRep,
            " ", py::arg("rkFilePath"))
        .def_static(
            "ByString",
            (::TopologicCore::Topology::Ptr(*)(::std::string const&)) & Topology::ByString,
            " ", py::arg("rkBrepString"))
        .def(
            "String",
            (::std::string(Topology::*)() const) & Topology::String,
            " ")
        .def_static(
            "Filter",
            (void(*)(::std::list<std::shared_ptr<TopologicCore::Topology>, std::allocator<std::shared_ptr<TopologicCore::Topology>>> const&, int const, ::std::list<std::shared_ptr<TopologicCore::Topology>, std::allocator<std::shared_ptr<TopologicCore::Topology>>> &)) & Topology::Filter,
            " ", py::arg("rkTopologies"), py::arg("kTypeFilter"), py::arg("rFilteredTopologies"))
        .def_static(
            "Simplify",
            (::TopoDS_Shape(*)(::TopoDS_Shape&)) & Topology::Simplify,
            " ", py::arg("rOcctShape"))
        .def_static(
            "BooleanSubTopologyContainment",
            (::TopoDS_Shape(*)(::TopoDS_Shape&)) & Topology::BooleanSubTopologyContainment,
            " ", py::arg("rOcctShape"))
        .def(
            "Analyze",
            (::std::string(Topology::*)()) & Topology::Analyze,
            " ")
        .def(
            "Dimensionality",
            (int(Topology::*)() const) & Topology::Dimensionality,
            " ")
        .def(
            "GetOcctShape",
            (::TopoDS_Shape & (Topology::*)()) & Topology::GetOcctShape,
            " ")
        .def(
            "SetOcctShape",
            (void(Topology::*)(::TopoDS_Shape const&)) & Topology::SetOcctShape,
            " ", py::arg("rkOcctShape"))
        .def(
            "GetOcctShape",
            (::TopoDS_Shape const& (Topology::*)() const) & Topology::GetOcctShape,
            " ")
        .def(
            "GetType",
            (::TopologicCore::TopologyType(Topology::*)() const) & Topology::GetType,
            " ")
        .def(
            "SubTopologies",
            (void(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Topology>, std::allocator<std::shared_ptr<TopologicCore::Topology>>> &) const) & Topology::SubTopologies,
            " ", py::arg("rSubTopologies"))
        .def(
            "NumOfSubTopologies",
            (int(Topology::*)() const) & Topology::NumOfSubTopologies,
            " ")
        .def(
            "GetTypeAsString",
            (::std::string(Topology::*)() const) & Topology::GetTypeAsString,
            " ")
        /*.def(
            "Shells",
            (void(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Shell>, std::allocator<std::shared_ptr<TopologicCore::Shell>>> &) const ) &Topology::Shells,
            " " , py::arg("rShells") )
        .def(
            "Edges",
            (void(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Edge>, std::allocator<std::shared_ptr<TopologicCore::Edge>>> &) const ) &Topology::Edges,
            " " , py::arg("rEdges") )
        .def(
            "Faces",
            (void(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Face>, std::allocator<std::shared_ptr<TopologicCore::Face>>> &) const ) &Topology::Faces,
            " " , py::arg("rFaces") )
        .def(
            "Vertices",
            (void(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> &) const ) &Topology::Vertices,
            " " , py::arg("rVertices") )
        .def(
            "Wires",
            (void(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Wire>, std::allocator<std::shared_ptr<TopologicCore::Wire>>> &) const ) &Topology::Wires,
            " " , py::arg("rWires") )
        .def(
            "Cells",
            (void(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Cell>, std::allocator<std::shared_ptr<TopologicCore::Cell>>> &) const ) &Topology::Cells,
            " " , py::arg("rCells") )
        .def(
            "CellComplexes",
            (void(Topology::*)(::std::list<std::shared_ptr<TopologicCore::CellComplex>, std::allocator<std::shared_ptr<TopologicCore::CellComplex>>> &) const ) &Topology::CellComplexes,
            " " , py::arg("rCellComplexes") )*/

        .def(
            "CellComplexes",
            [](const Topology& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rCellComplexes) {
        std::list<CellComplex::Ptr> local;
        obj.CellComplexes(kpHostTopology, local);
        for (auto& x : local)
            rCellComplexes.append(x);
    },
            " ", py::arg("kpHostTopology"), py::arg("rCellComplexes"))

        .def(
            "Shells",
            [](const Topology& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rShells) {
        std::list<Shell::Ptr> local;
        obj.Shells(kpHostTopology, local);
        for (auto& x : local)
            rShells.append(x);
    },
            " ", py::arg("kpHostTopology"), py::arg("rShells"))

        .def(
            "Edges",
            [](const Topology& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rEdges) {
        std::list<Edge::Ptr> local;
        obj.Edges(kpHostTopology, local);
        for (auto& x : local)
            rEdges.append(x);
    },
            " ", py::arg("kpHostTopology"), py::arg("rEdges"))

        .def(
            "Faces",
            [](const Topology& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rFaces) {
        std::list<Face::Ptr> local;
        obj.Faces(kpHostTopology, local);
        for (auto& x : local)
            rFaces.append(x);
    },
            " ", py::arg("kpHostTopology"), py::arg("rFaces"))

        .def(
            "Vertices",
            [](const Topology& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rVertices) {
        std::list<Vertex::Ptr> local;
        obj.Vertices(kpHostTopology, local);
        for (auto& x : local)
            rVertices.append(x);
    },
            " ", py::arg("kpHostTopology"), py::arg("rVertices"))

        .def(
            "Wires",
            [](const Topology& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rWires) {
        std::list<Wire::Ptr> local;
        obj.Wires(kpHostTopology, local);
        for (auto& x : local)
            rWires.append(x);
    },
            " ", py::arg("kpHostTopology"), py::arg("rWires"))

        .def(
            "Cells",
            [](const Topology& obj, ::TopologicCore::Topology::Ptr const& kpHostTopology, py::list& rCells) {
        std::list<Cell::Ptr> local;
        obj.Cells(kpHostTopology, local);
        for (auto& x : local)
            rCells.append(x);
    },
            " ", py::arg("kpHostTopology"), py::arg("rCells"))

        /*.def(
            "UpwardNavigation",
            (void(Topology::*)(::TopoDS_Shape const&, int const, ::std::list<std::shared_ptr<TopologicCore::Topology>, std::allocator<std::shared_ptr<TopologicCore::Topology>>> &) const) & Topology::UpwardNavigation,
            " ", py::arg("rkOcctHostTopology"), py::arg("kTopologyType"), py::arg("rAncestors"))
        .def_static(
            "DownwardNavigation",
            (void(*)(::TopoDS_Shape const&, ::TopAbs_ShapeEnum const&, ::TopTools_MapOfShape&)) & Topology::DownwardNavigation,
            " ", py::arg("rkOcctShape"), py::arg("rkShapeEnum"), py::arg("rOcctMembers"))*/
        .def(
            "DeepCopy",
            (::TopologicCore::Topology::Ptr(Topology::*)()) & Topology::DeepCopy,
            " ")
        .def(
            "ShallowCopy",
            (::TopologicCore::Topology::Ptr(Topology::*)()) & Topology::ShallowCopy,
            " ")
        .def_static(
            "CopyOcct",
            (::TopoDS_Shape(*)(::TopoDS_Shape const&)) & Topology::CopyOcct,
            " ", py::arg("rkOcctShape"))
        .def(
            "ReplaceSubentity",
            (void(Topology::*)(::TopologicCore::Topology::Ptr const&, ::TopologicCore::Topology::Ptr const&)) & Topology::ReplaceSubentity,
            " ", py::arg("rkOriginalSubshape"), py::arg("rkNewSubshape"))
        .def(
            "ReplaceSubentity",
            (void(Topology::*)(::TopoDS_Shape const&, ::TopoDS_Shape const&)) & Topology::ReplaceSubentity,
            " ", py::arg("rkOcctOriginalSubshape"), py::arg("rkOcctNewSubshape"))
        .def(
            "IsSame",
            (bool(Topology::*)(::TopologicCore::Topology::Ptr const&) const) & Topology::IsSame,
            " ", py::arg("kpTopology"))
        .def(
            "Members",
            (void(Topology::*)(::TopTools_ListOfShape&) const) & Topology::Members,
            " ", py::arg("rOcctMembers"))
        /*.def(
            "Members",
            (void(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Topology>, std::allocator<std::shared_ptr<TopologicCore::Topology>>> &) const ) &Topology::Members,
            " " , py::arg("rMembers") )*/
        .def(
            "Members",
            [](const Topology& obj, py::list& rMembers) {
        std::list<Topology::Ptr> local;
        obj.Members(local);
        for (auto& x : local)
            rMembers.append(x);
    },
            " ", py::arg("rMembers"))
        .def(
            "ClosestSimplestSubshape",
            (::TopologicCore::Topology::Ptr(Topology::*)(::TopologicCore::Topology::Ptr const&) const) & Topology::ClosestSimplestSubshape,
            " ", py::arg("kpTopology"))
        .def(
            "SelectSubtopology",
            (::TopologicCore::Topology::Ptr(Topology::*)(::std::shared_ptr<TopologicCore::Vertex> const&, int const) const) & Topology::SelectSubtopology,
            " ", py::arg("kpSelector"), py::arg("kTypeFilter") = (int const)(::TopologicCore::TopologyType::TOPOLOGY_ALL))
        .def(
            "CenterOfMass",
            (::std::shared_ptr<TopologicCore::Vertex>(Topology::*)() const) & Topology::CenterOfMass,
            " ")
        .def(
            "Centroid",
            (::std::shared_ptr<TopologicCore::Vertex>(Topology::*)() const) & Topology::Centroid,
            " ")
        .def(
            "GetClassGUID",
            (::std::string(Topology::*)() const) & Topology::GetClassGUID,
            " ")
        .def(
            "GetInstanceGUID",
            (::std::string const(Topology::*)() const) & Topology::GetInstanceGUID,
            " ")
        .def(
            "SetInstanceGUID",
            (void(Topology::*)(::TopoDS_Shape const&, ::std::string const&)) & Topology::SetInstanceGUID,
            " ", py::arg("rkOcctShape"), py::arg("rkGuid"))
        .def_static(
            "GetTopologyType",
            (::TopologicCore::TopologyType(*)(::TopAbs_ShapeEnum const&)) & Topology::GetTopologyType,
            " ", py::arg("rkOcctType"))
        .def_static(
            "GetOcctTopologyType",
            (::TopAbs_ShapeEnum(*)(::TopologicCore::TopologyType const&)) & Topology::GetOcctTopologyType,
            " ", py::arg("rkType"))
        .def(
            "IsContainerType",
            (bool(Topology::*)()) & Topology::IsContainerType,
            " ")
        .def(
            "RegisterFactory",
            (void(Topology::*)(::std::string const&, ::std::shared_ptr<TopologicCore::TopologyFactory> const&)) & Topology::RegisterFactory,
            " ", py::arg("rkGuid"), py::arg("kpTopologyFactory"))
        .def(
            "IsReversed",
            (bool(Topology::*)()) & Topology::IsReversed,
            " ")
        .def(
            "DeepCopyAttributesFrom",
            (::TopologicCore::Topology::Ptr(Topology::*)(::std::list<std::shared_ptr<TopologicCore::Topology>, std::allocator<std::shared_ptr<TopologicCore::Topology>>> const&)) & Topology::DeepCopyAttributesFrom,
            " ", py::arg("kpOriginTopologies"))
        .def(
            "SetDictionary",
            (void(Topology::*)(::TopologicCore::Dictionary const&)) & Topology::SetDictionary,
            " ", py::arg("dictionary"))
        .def(
            "GetDictionary",
            (::TopologicCore::Dictionary(Topology::*)()) & Topology::GetDictionary,
            " ")
        ;
}