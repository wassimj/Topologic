// This file is part of Topologic software library.
// Copyright(C) 2019, Cardiff University and University College London
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "Utilities.h"
#include "Vertex.h"
#include "Edge.h"

#include <list>
#include <chrono>

namespace TopologicCore
{
	class Vertex;
	class Edge;
	class Wire;
	class Face;
	class Shell;
	class Cell;
	class CellComplex;
	class Cluster;

	class Graph
	{
	public:
		typedef std::shared_ptr<Graph> Ptr;

	public:
		static Graph::Ptr ByVerticesEdges(const std::list<Vertex::Ptr>& rkVertices, const std::list<Edge::Ptr>& rkEdges);

		static TOPOLOGIC_API Graph::Ptr ByTopology(
			const Topology::Ptr topology, 
			const bool kDirect,
			const bool kViaSharedTopologies, 
			const bool kViaSharedApertures, 
			const bool kToExteriorTopologies, 
			const bool kToExteriorApertures,
			const bool useFaceInternalVertex,
			const double kTolerance);

		Graph(const std::list<Vertex::Ptr>& rkVertices, const std::list<Edge::Ptr>& rkEdges);

		TOPOLOGIC_API Graph(const Graph* rkAnotherGraph);

		virtual ~Graph();

		//TOPOLOGIC_API Topology::Ptr Topology() const;
		TOPOLOGIC_API TopologicCore::Topology::Ptr Topology() const;

		TOPOLOGIC_API void Vertices(std::list<std::shared_ptr<Vertex>>& rVertices) const;

		void Edges(std::list<std::shared_ptr<Edge>>& rEdges, const double kTolerance = 0.0001) const;

		TOPOLOGIC_API void Edges(
			const std::list<std::shared_ptr<Vertex>>& rkVertices, 
			const double kTolerance,
			std::list<std::shared_ptr<Edge>>& rEdges) const;

		TOPOLOGIC_API void AddVertices(const std::list<std::shared_ptr<Vertex>>& rkVertices, const double kTolerance);

		TOPOLOGIC_API void AddEdges(const std::list<std::shared_ptr<Edge>>& rkEdges, const double kTolerance);

		TOPOLOGIC_API int VertexDegree(const std::shared_ptr<Vertex>& kpVertex) const;

		int VertexDegree(const TopoDS_Vertex& kpVertex) const;

		TOPOLOGIC_API void AdjacentVertices(const std::shared_ptr<Vertex>& kpVertex, std::list<std::shared_ptr<Vertex>>& rAdjacentVertices) const;

		void AdjacentVertices(const TopoDS_Vertex& rkOcctVertex, TopTools_MapOfShape& rOcctAdjacentVertices) const;

		TOPOLOGIC_API void Connect(const std::list<std::shared_ptr<Vertex>>& rkVertices1, const std::list<std::shared_ptr<Vertex>>& rkVertices2, const double kTolerance);

		TOPOLOGIC_API bool ContainsVertex(const std::shared_ptr<Vertex>& kpVertex, const double kTolerance) const;

		bool ContainsVertex(const TopoDS_Vertex& rkOcctVertex, const double kTolerance) const;

		TOPOLOGIC_API bool ContainsEdge(const std::shared_ptr<Edge>& kpEdge, const double kTolerance) const;
		
		bool ContainsEdge(const TopoDS_Vertex& rkVertex1, const TopoDS_Vertex& rkVertex2, const double kTolerance) const;

		TOPOLOGIC_API void DegreeSequence(std::list<int>& rDegreeSequence) const;

		TOPOLOGIC_API double Density() const;

		TOPOLOGIC_API bool IsComplete() const;

		TOPOLOGIC_API void IsolatedVertices(std::list<Vertex::Ptr>& rIsolatedVertices) const;

		TOPOLOGIC_API int MinimumDelta() const;

		TOPOLOGIC_API int MaximumDelta() const;

		TOPOLOGIC_API void AllPaths(
			const Vertex::Ptr& kpStartVertex,
			const Vertex::Ptr& kpEndVertex,
			const bool kUseTimeLimit,
			const int kTimeLimit,
			std::list<std::shared_ptr<Wire>>& rPaths) const;

		void AllPaths(
			const Vertex::Ptr& kpStartVertex, 
			const Vertex::Ptr& kpEndVertex,
			const bool kUseTimeLimit,
			const int kTimeLimitInSeconds,
			const std::chrono::system_clock::time_point& rkStartingTime,
			std::list<Vertex::Ptr>& rPath,
			std::list<std::shared_ptr<Wire>>& rPaths) const;

		TOPOLOGIC_API std::shared_ptr<Wire> Path(
			const Vertex::Ptr& kpStartVertex,
			const Vertex::Ptr& kpEndVertex) const;

		std::shared_ptr<Wire> Path(
			const Vertex::Ptr& kpStartVertex,
			const Vertex::Ptr& kpEndVertex,
			std::list<Vertex::Ptr>& rPath) const;

		TOPOLOGIC_API std::shared_ptr<Wire> ShortestPath(
			const Vertex::Ptr& kpStartVertex,
			const Vertex::Ptr& kpEndVertex,
			const std::string& rkVertexKey,
			const std::string& rkEdgeKey) const;

		std::shared_ptr<Wire> ShortestPath(
			const TopoDS_Vertex& rkOcctStartVertex,
			const TopoDS_Vertex& rkOcctEndVertex,
			const std::string& rkVertexKey,
			const std::string& rkEdgeKey) const;

		TOPOLOGIC_API void ShortestPaths(
			const Vertex::Ptr& kpStartVertex,
			const Vertex::Ptr& kpEndVertex,
			const std::string& rkVertexKey,
			const std::string& rkEdgeKey,
			const bool kUseTimeLimit,
			const int kTimeLimit,
			std::list<std::shared_ptr<Wire>>& rPaths) const;

		void ShortestPaths(
			const TopoDS_Vertex& rkOcctStartVertex,
			const TopoDS_Vertex& rkOcctEndVertex,
			const std::string& rkVertexKey,
			const std::string& rkEdgeKey,
			const bool kUseTimeLimit,
			const int kTimeLimitInSeconds,
			std::list<std::shared_ptr<Wire>>& rPaths) const;

		TOPOLOGIC_API int Diameter() const;

		TOPOLOGIC_API int TopologicalDistance(const std::shared_ptr<Vertex>& kpStartVertex, const std::shared_ptr<Vertex>& kpEndVertex, const double kTolerance = 0.0001) const;

		int TopologicalDistance(const TopoDS_Vertex& rkOcctStartVertex, const TopoDS_Vertex& rkOcctEndVertex, const double kTolerance = 0.0001) const;

		TOPOLOGIC_API int Eccentricity(const std::shared_ptr<Vertex>& kpVertex) const;

		TOPOLOGIC_API bool IsErdoesGallai(const std::list<int>& rkSequence) const;

		TOPOLOGIC_API void RemoveVertices(const std::list<Vertex::Ptr>& rkVertices);

		TOPOLOGIC_API void RemoveEdges(const std::list<Edge::Ptr>& rkEdges, const double kTolerance = 0.0001);

		TOPOLOGIC_API void VerticesAtCoordinates(const double kX, const double kY, const double kZ, const double kTolerance, std::list<std::shared_ptr<Vertex>>& rVertices) const;

		//TOPOLOGIC_API std::shared_ptr<Edge> Edge(const std::shared_ptr<Vertex>& kpVertex1, const std::shared_ptr<Vertex>& kpVertex2, const double kTolerance) const;
		TOPOLOGIC_API std::shared_ptr<TopologicCore::Edge> Edge(const std::shared_ptr<Vertex>& kpVertex1, const std::shared_ptr<Vertex>& kpVertex2, const double kTolerance) const;

		void IncidentEdges(const std::shared_ptr<Vertex>& kpVertex, const double kTolerance, std::list<std::shared_ptr<TopologicCore::Edge>>& rEdges) const;

	protected:

		typedef std::map<TopoDS_Vertex, TopTools_MapOfShape, OcctShapeComparator> GraphMap;

		static Graph::Ptr ByVertex(
			const std::shared_ptr<Vertex> kpVertex,
			const bool kToExteriorApertures,
			const bool kUseFaceInternalVertex,
			const double kTolerance);

		static Graph::Ptr ByEdge(
			const std::shared_ptr<TopologicCore::Edge> kpEdge,
			const bool kDirect,
			const bool kToExteriorApertures,
			const bool kUseFaceInternalVertex,
			const double kTolerance);

		static Graph::Ptr ByWire(const std::shared_ptr<Wire> kpWire,
			const bool kDirect,
			const bool kToExteriorApertures,
			const bool kUseFaceInternalVertex,
			const double kTolerance);

		static Graph::Ptr ByFace(const std::shared_ptr<Face> kpFace,
			const bool kToExteriorTopologies,
			const bool kToExteriorApertures,
			const bool kUseFaceInternalVertex,
			const double kTolerance);

		static Graph::Ptr ByShell(const std::shared_ptr<Shell> kpShell,
			const bool kDirect,
			const bool kViaSharedTopologies,
			const bool kViaSharedApertures,
			const bool kToExteriorTopologies,
			const bool kToExteriorApertures,
			const bool kUseFaceInternalVertex,
			const double kTolerance);

		static Graph::Ptr ByCell(const std::shared_ptr<Cell> kpCell,
			const bool kToExteriorTopologies,
			const bool kToExteriorApertures,
			const bool kUseFaceInternalVertex,
			const double kTolerance);

		static Graph::Ptr ByCellComplex(const std::shared_ptr<CellComplex> kpCellComplex,
			const bool kDirect,
			const bool kViaSharedTopologies,
			const bool kViaSharedApertures,
			const bool kToExteriorTopologies,
			const bool kToExteriorApertures,
			const bool kUseFaceInternalVertex,
			const double kTolerance);

		static Graph::Ptr ByCluster(const std::shared_ptr<Cluster> kpCluster,
			const bool kDirect,
			const bool kViaSharedTopologies,
			const bool kViaSharedApertures,
			const bool kToExteriorTopologies,
			const bool kToExteriorApertures,
			const bool kUseFaceInternalVertex,
			const double kTolerance);

		std::shared_ptr<Wire> ConstructPath(const std::list<Vertex::Ptr>& rkPathVertices) const;

		std::shared_ptr<Wire> ConstructPath(
			const std::list<Vertex::Ptr>& rkPathVertices,
			const bool kUseTimeLimit,
			const int kTimeLimitInSeconds,
			const std::chrono::system_clock::time_point& rkStartingTime) const;

		bool IsDegreeSequence(const std::list<int>& rkSequence) const;

		TopoDS_Vertex GetCoincidentVertex(const TopoDS_Vertex& rkVertex, const double kTolerance) const;

		double ComputeCost(const TopoDS_Vertex& rkVertex1, const TopoDS_Vertex& rkVertex2, const std::string& rkVertexKey, const std::string& rkEdgeKey) const;
		double ComputeVertexCost(const TopoDS_Vertex& rkVertex, const std::string& rkVertexKey) const;
		double ComputeEdgeCost(const TopoDS_Vertex& rkVertex1, const TopoDS_Vertex& rkVertex2, const std::string& rkEdgeKey) const;

		TopoDS_Edge FindEdge(const TopoDS_Vertex& rkVertex1, const TopoDS_Vertex& rkVertex2, const double kTolerance = 0.0001) const;
		static bool IsCoincident(const TopoDS_Vertex& rkVertex1, const TopoDS_Vertex& rkVertex2, const double kTolerance = 0.0001);

		GraphMap m_graphDictionary;
		TopTools_MapOfShape m_occtEdges;
	};
}
