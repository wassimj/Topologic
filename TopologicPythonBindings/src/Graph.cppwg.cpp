#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "wrapper_header_collection.hpp"

#include "Graph.cppwg.hpp"

namespace py = pybind11;
//typedef TopologicCore::Graph Graph;
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

void register_Graph_class(py::module &m){
py::class_<Graph  , std::shared_ptr<Graph >   >(m, "Graph")
        .def(py::init<::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> const &, ::std::list<std::shared_ptr<TopologicCore::Edge>, std::allocator<std::shared_ptr<TopologicCore::Edge>>> const & >(), py::arg("rkVertices"), py::arg("rkEdges"))
        .def(py::init<::TopologicCore::Graph const * >(), py::arg("rkAnotherGraph"))
        .def_static(
            "ByVerticesEdges", 
            (::TopologicCore::Graph::Ptr(*)(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> const &, ::std::list<std::shared_ptr<TopologicCore::Edge>, std::allocator<std::shared_ptr<TopologicCore::Edge>>> const &)) &Graph::ByVerticesEdges, 
            " " , py::arg("rkVertices"), py::arg("rkEdges") )
        .def_static(
            "ByTopology", 
            (::TopologicCore::Graph::Ptr(*)(::TopologicCore::Topology::Ptr const, bool const, bool const, bool const, bool const, bool const, bool const, double const)) &Graph::ByTopology, 
            " " , py::arg("topology"), py::arg("kDirect"), py::arg("kViaSharedTopologies"), py::arg("kViaSharedApertures"), py::arg("kToExteriorTopologies"), py::arg("kToExteriorApertures"), py::arg("useFaceInternalVertex"), py::arg("kTolerance") )
        .def(
            "Topology", 
            (::TopologicCore::Topology::Ptr(Graph::*)() const ) &Graph::Topology, 
            " "  )
        /*.def(
            "Vertices", 
            (void(Graph::*)(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> &) const ) &Graph::Vertices, 
            " " , py::arg("rVertices") )*/
        .def(
            "Vertices",
            [](const Graph& obj, py::list& rVertices) {
                std::list<Vertex::Ptr> local;
                obj.Vertices(local);
                for (auto& x : local)
                    rVertices.append(x);
            },
            " ", py::arg("rVertices"))
        /*.def(
            "Edges", 
            (void(Graph::*)(::std::list<std::shared_ptr<TopologicCore::Edge>, std::allocator<std::shared_ptr<TopologicCore::Edge>>> &, double const) const ) &Graph::Edges, 
            " " , py::arg("rEdges"), py::arg("kTolerance") = 1.0E-4 )*/
        .def(
            "Edges",
            [](const Graph& obj, py::list& rEdges, double const kTolerance) {
                std::list<Edge::Ptr> local;
                obj.Edges(local, kTolerance);
                for (auto& x : local)
                    rEdges.append(x);
            },
            " ", py::arg("rEdges"), py::arg("kTolerance") = 1.0E-4)
        /*.def(
            "Edges", 
            (void(Graph::*)(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> const &, double const, ::std::list<std::shared_ptr<TopologicCore::Edge>, std::allocator<std::shared_ptr<TopologicCore::Edge>>> &) const ) &Graph::Edges, 
            " " , py::arg("rkVertices"), py::arg("kTolerance"), py::arg("rEdges") )*/
        .def(
            "Edges",
            [](const Graph& obj, ::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> const & rkVertices, double const kTolerance, py::list& rEdges) {
                std::list<Edge::Ptr> local;
                obj.Edges(rkVertices, kTolerance, local);
                for (auto& x : local)
                    rEdges.append(x);
            },
            " ", py::arg("rkVertices"), py::arg("kTolerance") = 1.0E-4, py::arg("rEdges"))
        .def(
            "AddVertices", 
            (void(Graph::*)(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> const &, double const)) &Graph::AddVertices, 
            " " , py::arg("rkVertices"), py::arg("kTolerance") )
        .def(
            "AddEdges", 
            (void(Graph::*)(::std::list<std::shared_ptr<TopologicCore::Edge>, std::allocator<std::shared_ptr<TopologicCore::Edge>>> const &, double const)) &Graph::AddEdges, 
            " " , py::arg("rkEdges"), py::arg("kTolerance") )
        .def(
            "VertexDegree", 
            (int(Graph::*)(::std::shared_ptr<TopologicCore::Vertex> const &) const ) &Graph::VertexDegree, 
            " " , py::arg("kpVertex") )
        .def(
            "VertexDegree", 
            (int(Graph::*)(::TopoDS_Vertex const &) const ) &Graph::VertexDegree, 
            " " , py::arg("kpVertex") )
        /*.def(
            "AdjacentVertices", 
            (void(Graph::*)(::std::shared_ptr<TopologicCore::Vertex> const &, ::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> &) const ) &Graph::AdjacentVertices, 
            " " , py::arg("kpVertex"), py::arg("rAdjacentVertices") )*/
        .def(
            "AdjacentVertices",
            [](const Graph& obj, ::std::shared_ptr<TopologicCore::Vertex> const & kpVertex, py::list& rAdjacentVertices) {
                std::list<Vertex::Ptr> local;
                obj.AdjacentVertices(kpVertex, local);
                for (auto& x : local)
                    rAdjacentVertices.append(x);
            },
            " ", py::arg("kpVertex"), py::arg("rAdjacentVertices"))
        .def(
            "AdjacentVertices", 
            (void(Graph::*)(::TopoDS_Vertex const &, ::TopTools_MapOfShape &) const ) &Graph::AdjacentVertices, 
            " " , py::arg("rkOcctVertex"), py::arg("rOcctAdjacentVertices") )
        .def(
            "Connect", 
            (void(Graph::*)(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> const &, ::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> const &, double const)) &Graph::Connect, 
            " " , py::arg("rkVertices1"), py::arg("rkVertices2"), py::arg("kTolerance") )
        .def(
            "ContainsVertex", 
            (bool(Graph::*)(::std::shared_ptr<TopologicCore::Vertex> const &, double const) const ) &Graph::ContainsVertex, 
            " " , py::arg("kpVertex"), py::arg("kTolerance") )
        .def(
            "ContainsVertex", 
            (bool(Graph::*)(::TopoDS_Vertex const &, double const) const ) &Graph::ContainsVertex, 
            " " , py::arg("rkOcctVertex"), py::arg("kTolerance") )
        .def(
            "ContainsEdge", 
            (bool(Graph::*)(::std::shared_ptr<TopologicCore::Edge> const &, double const) const ) &Graph::ContainsEdge, 
            " " , py::arg("kpEdge"), py::arg("kTolerance") )
        .def(
            "ContainsEdge", 
            (bool(Graph::*)(::TopoDS_Vertex const &, ::TopoDS_Vertex const &, double const) const ) &Graph::ContainsEdge, 
            " " , py::arg("rkVertex1"), py::arg("rkVertex2"), py::arg("kTolerance") )
        /*.def(
            "DegreeSequence", 
            (void(Graph::*)(::std::list<int, std::allocator<int>> &) const ) &Graph::DegreeSequence, 
            " " , py::arg("rDegreeSequence") ) */
        .def(
            "DegreeSequence",
            [](const Graph& obj, py::list& rDegreeSequence) {
                std::list<int> local;
                obj.DegreeSequence(local);
                for (auto& x : local)
                    rDegreeSequence.append(x);
            },
            " ", py::arg("rDegreeSequence"))
        .def(
            "Density", 
            (double(Graph::*)() const ) &Graph::Density, 
            " "  )
        .def(
            "IsComplete", 
            (bool(Graph::*)() const ) &Graph::IsComplete, 
            " "  )
        /*.def(
            "IsolatedVertices", 
            (void(Graph::*)(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> &) const ) &Graph::IsolatedVertices, 
            " " , py::arg("rIsolatedVertices") )*/
        .def(
            "IsolatedVertices",
            [](const Graph& obj, py::list& rIsolatedVertices) {
                std::list<Vertex::Ptr> local;
                obj.IsolatedVertices(local);
                for (auto& x : local)
                    rIsolatedVertices.append(x);
            },
            " ", py::arg("rIsolatedVertices"))
        .def(
            "MinimumDelta", 
            (int(Graph::*)() const ) &Graph::MinimumDelta, 
            " "  )
        .def(
            "MaximumDelta", 
            (int(Graph::*)() const ) &Graph::MaximumDelta, 
            " "  )
        /*.def(
            "AllPaths", 
            (void(Graph::*)(::TopologicCore::Vertex::Ptr const &, ::TopologicCore::Vertex::Ptr const &, bool const, int const, ::std::list<std::shared_ptr<TopologicCore::Wire>, std::allocator<std::shared_ptr<TopologicCore::Wire>>> &) const ) &Graph::AllPaths, 
            " " , py::arg("kpStartVertex"), py::arg("kpEndVertex"), py::arg("kUseTimeLimit"), py::arg("kTimeLimit"), py::arg("rPaths") )*/
        .def(
            "AllPaths",
            [](const Graph& obj, ::TopologicCore::Vertex::Ptr const& kpStartVertex, ::TopologicCore::Vertex::Ptr const& kpEndVertex, bool const kUseTimeLimit, int const kTimeLimit, py::list& rPaths) {
                std::list<Wire::Ptr> local;
                obj.AllPaths(kpStartVertex, kpEndVertex, kUseTimeLimit, kTimeLimit, local);
                for (auto& x : local)
                    rPaths.append(x);
            },
            " ", py::arg("kpStartVertex"), py::arg("kpEndVertex"), py::arg("kUseTimeLimit"), py::arg("kTimeLimit"), py::arg("rPaths"))
        .def(
            "AllPaths", 
            (void(Graph::*)(::TopologicCore::Vertex::Ptr const &, ::TopologicCore::Vertex::Ptr const &, bool const, int const, ::std::chrono::system_clock::time_point const &, ::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> &, ::std::list<std::shared_ptr<TopologicCore::Wire>, std::allocator<std::shared_ptr<TopologicCore::Wire>>> &) const ) &Graph::AllPaths, 
            " " , py::arg("kpStartVertex"), py::arg("kpEndVertex"), py::arg("kUseTimeLimit"), py::arg("kTimeLimitInSeconds"), py::arg("rkStartingTime"), py::arg("rPath"), py::arg("rPaths") )
        .def(
            "Path", 
            (::std::shared_ptr<TopologicCore::Wire>(Graph::*)(::TopologicCore::Vertex::Ptr const &, ::TopologicCore::Vertex::Ptr const &) const ) &Graph::Path, 
            " " , py::arg("kpStartVertex"), py::arg("kpEndVertex") )
        .def(
            "Path", 
            (::std::shared_ptr<TopologicCore::Wire>(Graph::*)(::TopologicCore::Vertex::Ptr const &, ::TopologicCore::Vertex::Ptr const &, ::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> &) const ) &Graph::Path, 
            " " , py::arg("kpStartVertex"), py::arg("kpEndVertex"), py::arg("rPath") )
        .def(
            "ShortestPath", 
            (::std::shared_ptr<TopologicCore::Wire>(Graph::*)(::TopologicCore::Vertex::Ptr const &, ::TopologicCore::Vertex::Ptr const &, ::std::string const &, ::std::string const &) const ) &Graph::ShortestPath, 
            " " , py::arg("kpStartVertex"), py::arg("kpEndVertex"), py::arg("rkVertexKey"), py::arg("rkEdgeKey") )
        .def(
            "ShortestPath", 
            (::std::shared_ptr<TopologicCore::Wire>(Graph::*)(::TopoDS_Vertex const &, ::TopoDS_Vertex const &, ::std::string const &, ::std::string const &) const ) &Graph::ShortestPath, 
            " " , py::arg("rkOcctStartVertex"), py::arg("rkOcctEndVertex"), py::arg("rkVertexKey"), py::arg("rkEdgeKey") )
        /*.def(
            "ShortestPaths", 
            (void(Graph::*)(::TopologicCore::Vertex::Ptr const &, ::TopologicCore::Vertex::Ptr const &, ::std::string const &, ::std::string const &, bool const, int const, ::std::list<std::shared_ptr<TopologicCore::Wire>, std::allocator<std::shared_ptr<TopologicCore::Wire>>> &) const ) &Graph::ShortestPaths, 
            " " , py::arg("kpStartVertex"), py::arg("kpEndVertex"), py::arg("rkVertexKey"), py::arg("rkEdgeKey"), py::arg("kUseTimeLimit"), py::arg("kTimeLimit"), py::arg("rPaths") )*/
        .def(
            "ShortestPaths",
            [](const Graph& obj, ::TopologicCore::Vertex::Ptr const& kpStartVertex, ::TopologicCore::Vertex::Ptr const& kpEndVertex, ::std::string const & rkVertexKey, ::std::string const & rkEdgeKey, bool const kUseTimeLimit, int const kTimeLimit, py::list& rPaths) {
                std::list<Wire::Ptr> local;
                obj.ShortestPaths(kpStartVertex, kpEndVertex, rkVertexKey, rkEdgeKey, kUseTimeLimit, kTimeLimit, local);
                for (auto& x : local)
                    rPaths.append(x);
            },
            " ", py::arg("kpStartVertex"), py::arg("kpEndVertex"), py::arg("rkVertexKey"), py::arg("rkEdgeKey"), py::arg("kUseTimeLimit"), py::arg("kTimeLimit"), py::arg("rPaths"))
        .def(
            "ShortestPaths", 
            (void(Graph::*)(::TopoDS_Vertex const &, ::TopoDS_Vertex const &, ::std::string const &, ::std::string const &, bool const, int const, ::std::list<std::shared_ptr<TopologicCore::Wire>, std::allocator<std::shared_ptr<TopologicCore::Wire>>> &) const ) &Graph::ShortestPaths, 
            " " , py::arg("rkOcctStartVertex"), py::arg("rkOcctEndVertex"), py::arg("rkVertexKey"), py::arg("rkEdgeKey"), py::arg("kUseTimeLimit"), py::arg("kTimeLimitInSeconds"), py::arg("rPaths") )
        .def(
            "Diameter", 
            (int(Graph::*)() const ) &Graph::Diameter, 
            " "  )
        .def(
            "TopologicalDistance", 
            (int(Graph::*)(::std::shared_ptr<TopologicCore::Vertex> const &, ::std::shared_ptr<TopologicCore::Vertex> const &, double const) const ) &Graph::TopologicalDistance, 
            " " , py::arg("kpStartVertex"), py::arg("kpEndVertex"), py::arg("kTolerance") = 1.0E-4 )
        .def(
            "TopologicalDistance", 
            (int(Graph::*)(::TopoDS_Vertex const &, ::TopoDS_Vertex const &, double const) const ) &Graph::TopologicalDistance, 
            " " , py::arg("rkOcctStartVertex"), py::arg("rkOcctEndVertex"), py::arg("kTolerance") = 1.0E-4 )
        .def(
            "Eccentricity", 
            (int(Graph::*)(::std::shared_ptr<TopologicCore::Vertex> const &) const ) &Graph::Eccentricity, 
            " " , py::arg("kpVertex") )
        .def(
            "IsErdoesGallai", 
            (bool(Graph::*)(::std::list<int, std::allocator<int>> const &) const ) &Graph::IsErdoesGallai, 
            " " , py::arg("rkSequence") )
        .def(
            "RemoveVertices", 
            (void(Graph::*)(::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> const &)) &Graph::RemoveVertices, 
            " " , py::arg("rkVertices") )
        .def(
            "RemoveEdges", 
            (void(Graph::*)(::std::list<std::shared_ptr<TopologicCore::Edge>, std::allocator<std::shared_ptr<TopologicCore::Edge>>> const &, double const)) &Graph::RemoveEdges, 
            " " , py::arg("rkEdges"), py::arg("kTolerance") = 1.0E-4 )
        /*.def(
            "VerticesAtCoordinates", 
            (void(Graph::*)(double const, double const, double const, double const, ::std::list<std::shared_ptr<TopologicCore::Vertex>, std::allocator<std::shared_ptr<TopologicCore::Vertex>>> &) const ) &Graph::VerticesAtCoordinates, 
            " " , py::arg("kX"), py::arg("kY"), py::arg("kZ"), py::arg("kTolerance"), py::arg("rVertices") )*/
        .def(
            "VerticesAtCoordinates",
            [](const Graph& obj, double const kX, double const kY, double const kZ, double const kTolerance, py::list& rVertices) {
                std::list<Vertex::Ptr> local;
                obj.VerticesAtCoordinates(kX, kY, kZ, kTolerance, local);
                for (auto& x : local)
                    rVertices.append(x);
            },
            " ", py::arg("kX"), py::arg("kY"), py::arg("kZ"), py::arg("kTolerance"), py::arg("rVertices"))
        .def(
            "Edge", 
            (::std::shared_ptr<TopologicCore::Edge>(Graph::*)(::std::shared_ptr<TopologicCore::Vertex> const &, ::std::shared_ptr<TopologicCore::Vertex> const &, double const) const ) &Graph::Edge, 
            " " , py::arg("kpVertex1"), py::arg("kpVertex2"), py::arg("kTolerance") )
        /*.def(
            "IncidentEdges", 
            (void(Graph::*)(::std::shared_ptr<TopologicCore::Vertex> const &, double const, ::std::list<std::shared_ptr<TopologicCore::Edge>, std::allocator<std::shared_ptr<TopologicCore::Edge>>> &) const ) &Graph::IncidentEdges, 
            " " , py::arg("kpVertex"), py::arg("kTolerance"), py::arg("rEdges") )*/
        .def(
            "IncidentEdges",
            [](const Graph& obj, ::TopologicCore::Vertex::Ptr const& kpVertex, double const kTolerance, py::list& rEdges) {
                std::list<Edge::Ptr> local;
                obj.IncidentEdges(kpVertex, kTolerance, local);
                for (auto& x : local)
                    rEdges.append(x);
            },
            " ", py::arg("kpVertex"), py::arg("kTolerance"), py::arg("rEdges"))
    ;
}
