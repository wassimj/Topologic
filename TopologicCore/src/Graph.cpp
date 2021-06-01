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

#include "Graph.h"
#include "Cluster.h"
#include "Wire.h"
#include "Face.h"
#include "Shell.h"
#include "Cell.h"
#include "CellComplex.h"
#include "Aperture.h"
#include "Attribute.h"
#include "DoubleAttribute.h"
#include "IntAttribute.h"
#include "AttributeManager.h"

#include <Utilities/EdgeUtility.h>
#include <Utilities/FaceUtility.h>
#include <Utilities/CellUtility.h>

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <TopoDS.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <Geom_CartesianPoint.hxx>
#include <BRep_Tool.hxx>

#include <algorithm>
#include <assert.h>
#include <numeric>
#include <queue>

namespace TopologicCore
{
	Graph::Ptr Graph::ByVerticesEdges(const std::list<Vertex::Ptr>& rkVertices, const std::list<Edge::Ptr>& rkEdges)
	{
		return std::make_shared<Graph>(rkVertices, rkEdges);
	}

	Graph::Ptr Graph::ByTopology(
		const Topology::Ptr topology,
		const bool kDirect,
		const bool kViaSharedTopologies,
		const bool kViaSharedApertures,
		const bool kToExteriorTopologies,
		const bool kToExteriorApertures,
		const bool kUseFaceInternalVertex,
		const double kTolerance)
	{
		switch (topology->GetType())
		{
		case TOPOLOGY_VERTEX: return Graph::ByVertex(std::dynamic_pointer_cast<Vertex>(topology), kToExteriorApertures, kUseFaceInternalVertex, kTolerance);
		case TOPOLOGY_EDGE: return Graph::ByEdge(std::dynamic_pointer_cast<TopologicCore::Edge>(topology), kDirect, kToExteriorApertures, kUseFaceInternalVertex, kTolerance);
		case TOPOLOGY_WIRE: return Graph::ByWire(std::dynamic_pointer_cast<Wire>(topology), kDirect, kToExteriorApertures, kUseFaceInternalVertex, kTolerance);
		case TOPOLOGY_FACE: return Graph::ByFace(std::dynamic_pointer_cast<Face>(topology), kToExteriorTopologies, kToExteriorApertures, kUseFaceInternalVertex, kTolerance);
		case TOPOLOGY_SHELL: return Graph::ByShell(
			std::dynamic_pointer_cast<Shell>(topology),
			kDirect,
			kViaSharedTopologies,
			kViaSharedApertures,
			kToExteriorTopologies,
			kToExteriorApertures,
			kUseFaceInternalVertex,
			kTolerance);
		case TOPOLOGY_CELL: return Graph::ByCell(std::dynamic_pointer_cast<Cell>(topology), kToExteriorTopologies, kToExteriorApertures, kUseFaceInternalVertex, kTolerance);
		case TOPOLOGY_CELLCOMPLEX: return Graph::ByCellComplex(std::dynamic_pointer_cast<CellComplex>(topology),
			kDirect,
			kViaSharedTopologies,
			kViaSharedApertures,
			kToExteriorTopologies,
			kToExteriorApertures,
			kUseFaceInternalVertex,
			kTolerance);
		case TOPOLOGY_CLUSTER: return Graph::ByCluster(std::dynamic_pointer_cast<Cluster>(topology),
			kDirect,
			kViaSharedTopologies,
			kViaSharedApertures,
			kToExteriorTopologies,
			kToExteriorApertures,
			kUseFaceInternalVertex,
			kTolerance);

		case TOPOLOGY_APERTURE:
			return Graph::ByTopology(
				std::dynamic_pointer_cast<Aperture>(topology)->Topology(),
				kDirect,
				kViaSharedTopologies,
				kViaSharedApertures,
				kToExteriorTopologies,
				kToExteriorApertures,
				kUseFaceInternalVertex,
				kTolerance);
		default:
			throw std::runtime_error("Fails to create a graph due to an unknown type of topology.");
		}
	}

	Graph::Graph(const std::list<Vertex::Ptr>& rkVertices, const std::list<Edge::Ptr>& rkEdges)
	{
		// 1. Add the vertices
		AddVertices(rkVertices, 0.0001);

		// 2. Add the edges
		AddEdges(rkEdges, 0.0001);
	}

	Graph::Graph(const Graph* kpAnotherGraph)
		: m_graphDictionary(kpAnotherGraph->m_graphDictionary)
		, m_occtEdges(kpAnotherGraph->m_occtEdges)
	{
	}

	Graph::~Graph()
	{
		m_graphDictionary.clear();
	}

	Topology::Ptr Graph::Topology() const
	{
		// Graph: visualise in this order:
		// 1. the edges
		// 2. isolated vertices

		// For a loop: circle, radius/diameter/circumference = average of the edge lengths

		std::list<Topology::Ptr> topologies;
		GraphMap processedAdjacency;
		for (const std::pair<TopoDS_Vertex, TopTools_MapOfShape>& rkDictionaryPair : m_graphDictionary)
		{
			TopoDS_Vertex occtVertex1 = rkDictionaryPair.first;
			Vertex::Ptr vertex1 = std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtVertex1));
			if (rkDictionaryPair.second.Size() == 0)
			{
				// Just add the vertex
				topologies.push_back(vertex1);
				processedAdjacency.insert(std::make_pair(occtVertex1, TopTools_MapOfShape()));
			}
			else
			{
				// Create edges
				for (TopTools_MapIteratorOfMapOfShape mapIterator(rkDictionaryPair.second);
					mapIterator.More();
					mapIterator.Next())
				{
					TopoDS_Shape occtVertex2 = mapIterator.Value();
					Vertex::Ptr vertex2 = std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtVertex2));

					GraphMap::iterator vertex1Iterator = processedAdjacency.find(occtVertex1);
					if (vertex1Iterator != processedAdjacency.end())
					{
						if (vertex1Iterator->second.Contains(occtVertex2))
						{
							continue;
						}
					}

					GraphMap::iterator vertex2Iterator = processedAdjacency.find(TopoDS::Vertex(occtVertex2));
					if (vertex2Iterator != processedAdjacency.end())
					{
						if (vertex2Iterator->second.Contains(occtVertex1))
						{
							continue;
						}
					}

					/*Edge::Ptr edge = Edge::ByStartVertexEndVertex(vertex1, vertex2);
					topologies.push_back(edge);*/

					TopoDS_Edge occtEdge = FindEdge(vertex1->GetOcctVertex(), vertex2->GetOcctVertex());
					if (occtEdge.IsNull())
					{
						continue;
					}
					//Edge::Ptr edge = Edge::ByStartVertexEndVertex(vertex1, vertex2);
					Edge::Ptr edge = std::dynamic_pointer_cast<TopologicCore::Edge>(Topology::ByOcctShape(occtEdge));
					topologies.push_back(edge);

					processedAdjacency[vertex1->GetOcctVertex()].Add(vertex2->GetOcctVertex());
					processedAdjacency[vertex2->GetOcctVertex()].Add(vertex1->GetOcctVertex());
				}
			}
		}

		Cluster::Ptr cluster = Cluster::ByTopologies(topologies);
		return cluster;
	}

	void Graph::Vertices(std::list<std::shared_ptr<Vertex>>& rVertices) const
	{
		for (const std::pair<TopoDS_Vertex, TopTools_MapOfShape>& rkDictionaryPair : m_graphDictionary)
		{
			Vertex::Ptr vertex = std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(rkDictionaryPair.first));
			rVertices.push_back(vertex);
		}
	}

	void Graph::Edges(std::list<std::shared_ptr<TopologicCore::Edge>>& rEdges, const double kTolerance) const
	{
		std::list<Vertex::Ptr> vertices;
		Edges(vertices, kTolerance, rEdges);
	}

	void Graph::Edges(
		const std::list<Vertex::Ptr>& rkVertices, 
		const double kTolerance, 
		std::list<std::shared_ptr<TopologicCore::Edge>>& rEdges) const
	{
		if (rkVertices.empty())
		{
			GraphMap processedAdjacency;
			for (const std::pair<TopoDS_Vertex, TopTools_MapOfShape>& rkDictionaryPair : m_graphDictionary)
			{
				TopoDS_Vertex occtVertex1 = rkDictionaryPair.first;
				Vertex::Ptr vertex1 = std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtVertex1));
				// Create edges
				for (TopTools_MapIteratorOfMapOfShape mapIterator(rkDictionaryPair.second);
					mapIterator.More();
					mapIterator.Next())
				{
					TopoDS_Shape occtVertex2 = mapIterator.Value();
					GraphMap::iterator vertex1Iterator = processedAdjacency.find(occtVertex1);
					if (vertex1Iterator != processedAdjacency.end())
					{
						if (vertex1Iterator->second.Contains(occtVertex2))
						{
							continue;
						}
					}

					GraphMap::iterator vertex2Iterator = processedAdjacency.find(TopoDS::Vertex(occtVertex2));
					if (vertex2Iterator != processedAdjacency.end())
					{
						if (vertex2Iterator->second.Contains(occtVertex1))
						{
							continue;
						}
					}

					Topology::Ptr topology = Topology::ByOcctShape(TopoDS::Vertex(occtVertex2));
					Vertex::Ptr vertex2 = std::dynamic_pointer_cast<Vertex>(topology);

					TopoDS_Edge occtEdge = FindEdge(vertex1->GetOcctVertex(), vertex2->GetOcctVertex());
					if (occtEdge.IsNull())
					{
						continue;
					}
					Edge::Ptr edge = std::dynamic_pointer_cast<TopologicCore::Edge>(Topology::ByOcctShape(occtEdge));
					rEdges.push_back(edge);

					processedAdjacency[vertex1->GetOcctVertex()].Add(vertex2->GetOcctVertex());
					processedAdjacency[vertex2->GetOcctVertex()].Add(vertex1->GetOcctVertex());
				}
			}
		}
		else
		{
			// Different algorithm if rkVertices is not empty.
			GraphMap processedAdjacency;

			for (const Vertex::Ptr& kpVertex : rkVertices)
			{
				// Get coincident edges
				std::list<Edge::Ptr> thisIncidentEdges;
				IncidentEdges(kpVertex, kTolerance, thisIncidentEdges);

				// Check: is already added?
				for (const Edge::Ptr& kpIncidentEdge : thisIncidentEdges)
				{
					Vertex::Ptr startVertex = kpIncidentEdge->StartVertex();
					Vertex::Ptr endVertex = kpIncidentEdge->EndVertex();

					bool isAdded = false;
					GraphMap::iterator vertex1Iterator = processedAdjacency.find(startVertex->GetOcctVertex());
					if (vertex1Iterator != processedAdjacency.end())
					{
						if (vertex1Iterator->second.Contains(endVertex->GetOcctVertex()))
						{
							isAdded = true;
						}
					}

					if (!isAdded)
					{
						GraphMap::iterator vertex1Iterator = processedAdjacency.find(endVertex->GetOcctVertex());
						if (vertex1Iterator != processedAdjacency.end())
						{
							if (vertex1Iterator->second.Contains(startVertex->GetOcctVertex()))
							{
								isAdded = true;
							}
						}
					}

					if (!isAdded)
					{
						rEdges.push_back(kpIncidentEdge);
						processedAdjacency[startVertex->GetOcctVertex()].Add(endVertex->GetOcctShape());
					}
				}
			}
		}
	}

	void Graph::AddVertices(const std::list<Vertex::Ptr>& rkVertices, const double kTolerance)
	{
		if (kTolerance <= 0.0)
		{
			throw std::runtime_error("The tolerance must have a positive value.");
		}

		for (const Vertex::Ptr& kpVertex : rkVertices)
		{
			if (!ContainsVertex(kpVertex, kTolerance))
			{
				m_graphDictionary.insert(std::make_pair(kpVertex->GetOcctVertex(), TopTools_MapOfShape()));
			}
		}
	}

	void Graph::AddEdges(const std::list<Edge::Ptr>& rkEdges, const double kTolerance)
	{
		if (kTolerance <= 0.0)
		{
			throw std::runtime_error("The tolerance must have a positive value.");
		}

		for (const Edge::Ptr& kpEdge : rkEdges)
		{
			if (!ContainsEdge(kpEdge, kTolerance))
			{
				Vertex::Ptr startVertex = kpEdge->StartVertex();
				TopoDS_Vertex occtStartCoincidentVertex = GetCoincidentVertex(startVertex->GetOcctVertex(), kTolerance);
				if (occtStartCoincidentVertex.IsNull())
				{
					occtStartCoincidentVertex = startVertex->GetOcctVertex();
				}
				Vertex::Ptr endVertex = kpEdge->EndVertex();
				TopoDS_Vertex occtEndCoincidentVertex = GetCoincidentVertex(endVertex->GetOcctVertex(), kTolerance);
				if (occtEndCoincidentVertex.IsNull())
				{
					occtEndCoincidentVertex = endVertex->GetOcctVertex();
				}

				m_graphDictionary[occtStartCoincidentVertex].Add(occtEndCoincidentVertex);
				m_graphDictionary[occtEndCoincidentVertex].Add(occtStartCoincidentVertex);

				m_occtEdges.Add(kpEdge->GetOcctEdge());
			}
		}
	}

	int Graph::VertexDegree(const std::shared_ptr<Vertex>& kpVertex) const
	{
		return VertexDegree(kpVertex->GetOcctVertex());
	}

	int Graph::VertexDegree(const TopoDS_Vertex & rkOcctVertex) const
	{
        auto vertexAdjacency = m_graphDictionary.find(rkOcctVertex);
		if (vertexAdjacency == m_graphDictionary.end())
		{
			return 0;
		}

		int numberOfEdges = vertexAdjacency->second.Size();
		int numberOfLoops = vertexAdjacency->second.Contains(rkOcctVertex) ? 1 : 0;
		int degree = numberOfEdges + numberOfLoops;

		return degree;
	}

	void Graph::AdjacentVertices(const std::shared_ptr<Vertex>& kpVertex, std::list<std::shared_ptr<Vertex>>& rAdjacentVertices) const
	{
		TopTools_MapOfShape occtAdjacentVertices;
		AdjacentVertices(kpVertex->GetOcctVertex(), occtAdjacentVertices);

		for (TopTools_MapIteratorOfMapOfShape mapIterator(occtAdjacentVertices);
			mapIterator.More();
			mapIterator.Next())
		{
			Vertex::Ptr adjacentVertex = std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(mapIterator.Value()));
			rAdjacentVertices.push_back(adjacentVertex);
		}
	}

	void Graph::AdjacentVertices(const TopoDS_Vertex& rkOcctVertex, TopTools_MapOfShape& rOcctAdjacentVertices) const
	{
		if (!ContainsVertex(rkOcctVertex, 0.0001))
		{
			return;
		}

		rOcctAdjacentVertices = m_graphDictionary.find(rkOcctVertex)->second;
	}

	void Graph::Connect(const std::list<std::shared_ptr<Vertex>>& rkVertices1, const std::list<std::shared_ptr<Vertex>>& rkVertices2, const double kTolerance)
	{
		if (kTolerance <= 0.0)
		{
			throw std::runtime_error("The tolerance must have a positive value.");
		}

        std::list<Vertex::Ptr>::const_iterator vertex1Iterator = rkVertices1.begin();
        std::list<Vertex::Ptr>::const_iterator vertex2Iterator = rkVertices2.begin();
        for (; vertex1Iterator != rkVertices1.end() && vertex2Iterator != rkVertices2.end(); ++vertex1Iterator, ++vertex2Iterator)
        {
            const Vertex::Ptr& kpVertex1 = *vertex1Iterator;
            const Vertex::Ptr& kpVertex2 = *vertex2Iterator;

            TopoDS_Vertex occtQueryVertex1 = GetCoincidentVertex(kpVertex1->GetOcctVertex(), kTolerance);
            if (occtQueryVertex1.IsNull())
            {
                occtQueryVertex1 = kpVertex1->GetOcctVertex();
            }
            TopoDS_Vertex occtQueryVertex2 = GetCoincidentVertex(kpVertex2->GetOcctVertex(), kTolerance);
            if (occtQueryVertex2.IsNull())
            {
                occtQueryVertex2 = kpVertex2->GetOcctVertex();
            }

            bool addingEdge = false;
            if (!m_graphDictionary[occtQueryVertex1].Contains(occtQueryVertex2))
            {
                m_graphDictionary[occtQueryVertex1].Add(occtQueryVertex2);
                addingEdge = true;
            }
            if (!m_graphDictionary[occtQueryVertex2].Contains(occtQueryVertex1))
            {
                m_graphDictionary[occtQueryVertex2].Add(occtQueryVertex1);
                addingEdge = true;
            }

            if (addingEdge)
            {
                Vertex::Ptr queryVertex1 = std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtQueryVertex1));
                Vertex::Ptr queryVertex2 = std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtQueryVertex2));
                Edge::Ptr edge = Edge::ByStartVertexEndVertex(queryVertex1, queryVertex2);
                m_occtEdges.Add(edge->GetOcctEdge());
            }
        }
	}

	bool Graph::ContainsVertex(const std::shared_ptr<Vertex>& kpVertex, const double kTolerance) const
	{
		return ContainsVertex(kpVertex->GetOcctVertex(), kTolerance);
	}

	bool Graph::ContainsVertex(const TopoDS_Vertex & rkOcctVertex, const double kTolerance) const
	{
		TopoDS_Vertex occtCoincidentVertex = GetCoincidentVertex(rkOcctVertex, kTolerance);
		return !occtCoincidentVertex.IsNull();
	}

	bool Graph::ContainsEdge(const std::shared_ptr<TopologicCore::Edge>& kpEdge, const double kTolerance) const
	{
		Vertex::Ptr startVertex = kpEdge->StartVertex();
		Vertex::Ptr endVertex = kpEdge->EndVertex();
		return ContainsEdge(startVertex->GetOcctVertex(), endVertex->GetOcctVertex(), kTolerance);
	}

	bool Graph::ContainsEdge(const TopoDS_Vertex & rkVertex1, const TopoDS_Vertex & rkVertex2, const double kTolerance) const
	{
		if (kTolerance <= 0.0)
		{
			throw std::runtime_error("The tolerance must have a positive value.");
		}

		TopoDS_Vertex occtStartCoincidentVertex = GetCoincidentVertex(rkVertex1, kTolerance);
		if (occtStartCoincidentVertex.IsNull())
		{
			return false;
		}
		TopoDS_Vertex occtEndCoincidentVertex = GetCoincidentVertex(rkVertex2, kTolerance);
		if (occtEndCoincidentVertex.IsNull())
		{
			return false;
		}

		TopTools_MapOfShape adjacentVerticesToStart = m_graphDictionary.find(occtStartCoincidentVertex)->second;
		TopTools_MapOfShape adjacentVerticesToEnd = m_graphDictionary.find(occtEndCoincidentVertex)->second;

		return adjacentVerticesToStart.Contains(occtEndCoincidentVertex) || adjacentVerticesToEnd.Contains(occtStartCoincidentVertex);
	}

	void Graph::DegreeSequence(std::list<int>& rDegreeSequence) const
	{
		for (const std::pair<TopoDS_Vertex, TopTools_MapOfShape>& rkDictionaryPair : m_graphDictionary)
		{
			Vertex::Ptr vertex = std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(rkDictionaryPair.first));
			rDegreeSequence.push_back(VertexDegree(vertex));
		}

		rDegreeSequence.sort(std::greater<int>());
	}

	double Graph::Density() const
	{
		int numOfVertices = (int)m_graphDictionary.size();

		std::list<Edge::Ptr> edges;
		Edges(edges);
		int numOfEdges = (int)edges.size();
		double denominator = numOfVertices * (numOfVertices - 1);
		if (denominator > -0.0001 && denominator < 0.0001)
		{
			// Divide by zero, return the largest double number
			return std::numeric_limits<double>::max();
		}
		return  (2 * numOfEdges) / denominator;
		//return numOfEdges / denominator; 
	}

	bool Graph::IsComplete() const
	{
		return Density() > 0.9999;
	}

	void Graph::IsolatedVertices(std::list<Vertex::Ptr>& rIsolatedVertices) const
	{
		for (const std::pair<TopoDS_Vertex, TopTools_MapOfShape>& kpPair : m_graphDictionary)
		{
			if (kpPair.second.IsEmpty())
			{
				rIsolatedVertices.push_back(std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(kpPair.first)));
			}
		}
	}

	int Graph::MinimumDelta() const
	{
		int minimumDelta = std::numeric_limits<int>::max();

		for (const std::pair<TopoDS_Vertex, TopTools_MapOfShape>& kpPair : m_graphDictionary)
		{
			int vertexDegree = VertexDegree(kpPair.first);
			if (vertexDegree < minimumDelta)
			{
				minimumDelta = vertexDegree;
			}
		}
		return minimumDelta;
	}

	int Graph::MaximumDelta() const
	{
		int maximumDelta = 0;

		for (const std::pair<TopoDS_Vertex, TopTools_MapOfShape>& kpPair : m_graphDictionary)
		{
			int vertexDegree = VertexDegree(kpPair.first);
			if (vertexDegree > maximumDelta)
			{
				maximumDelta = vertexDegree;
			}
		}
		return maximumDelta;
	}

	void Graph::AllPaths(
		const Vertex::Ptr & kpStartVertex,
		const Vertex::Ptr & kpEndVertex,
		const bool kUseTimeLimit,
		const int kTimeLimitInSeconds,
		std::list<Wire::Ptr>& rPaths) const
	{
		std::list<Vertex::Ptr> path;
		auto startingTime = std::chrono::system_clock::now();
		AllPaths(kpStartVertex, kpEndVertex, kUseTimeLimit, kTimeLimitInSeconds, startingTime, path, rPaths);
	}

	void Graph::AllPaths(
		const Vertex::Ptr & kpStartVertex,
		const Vertex::Ptr & kpEndVertex,
		const bool kUseTimeLimit,
		const int kTimeLimitInSeconds,
		const std::chrono::system_clock::time_point& rkStartingTime,
		std::list<Vertex::Ptr>& rPath,
		std::list<Wire::Ptr>& rPaths) const
	{
		if (kUseTimeLimit)
		{
			auto currentTime = std::chrono::system_clock::now();
			auto timeDifferenceInSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - rkStartingTime);
			if (timeDifferenceInSeconds.count() >= kTimeLimitInSeconds)
			{
				return;
			}
		}

		if (!ContainsVertex(kpStartVertex, 0.0001))
		{
			return;
		}

		rPath.push_back(kpStartVertex);
		if (kpStartVertex->IsSame(kpEndVertex))
		{
			// Create a wire
			Wire::Ptr pathWire = ConstructPath(rPath);

			rPaths.push_back(pathWire);
			return;
		}

		if (kUseTimeLimit)
		{
			auto currentTime = std::chrono::system_clock::now();
			auto timeDifferenceInSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - rkStartingTime);
			if (timeDifferenceInSeconds.count() >= kTimeLimitInSeconds)
			{
				return;
			}
		}

		TopTools_MapOfShape occtConnectedVertices = m_graphDictionary.find(kpStartVertex->GetOcctVertex())->second;
		for (TopTools_MapIteratorOfMapOfShape mapIterator(occtConnectedVertices);
			mapIterator.More();
			mapIterator.Next())
		{
			Vertex::Ptr connectedVertex = std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(mapIterator.Value()));
			if (std::find_if(rPath.begin(), rPath.end(), TopologyCompare(connectedVertex)) == rPath.end())
			{
				std::list<Wire::Ptr> extendedPaths;
				std::list<Vertex::Ptr> previousPath = rPath;

				if (kUseTimeLimit)
				{
					auto currentTime = std::chrono::system_clock::now();
					auto timeDifferenceInSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - rkStartingTime);
					if (timeDifferenceInSeconds.count() >= kTimeLimitInSeconds)
					{
						break;
					}
				}

				AllPaths(connectedVertex, kpEndVertex, kUseTimeLimit, kTimeLimitInSeconds, rkStartingTime, previousPath, extendedPaths);
				for (const Wire::Ptr& rkExtendedPath : extendedPaths)
				{
					rPaths.push_back(rkExtendedPath);
				}
			}

			if (kUseTimeLimit)
			{
				auto currentTime = std::chrono::system_clock::now();
				auto timeDifferenceInSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - rkStartingTime);
				if (timeDifferenceInSeconds.count() >= kTimeLimitInSeconds)
				{
					break;
				}
			}
		}

	}

	Wire::Ptr Graph::Path(const Vertex::Ptr & kpStartVertex, const Vertex::Ptr & kpEndVertex) const
	{
		std::list<Vertex::Ptr> path;
		return Path(kpStartVertex, kpEndVertex, path);
	}

	Wire::Ptr Graph::Path(const Vertex::Ptr & kpStartVertex, const Vertex::Ptr & kpEndVertex, std::list<Vertex::Ptr>& rPath) const
	{
		rPath.push_back(kpStartVertex);
		if (!ContainsVertex(kpStartVertex, 0.0001))
		{
			return nullptr;
		}

		if (kpStartVertex->IsSame(kpEndVertex))
		{
			Wire::Ptr pathWire = ConstructPath(rPath);
			return pathWire;
		}

		TopTools_MapOfShape occtConnectedVertices = m_graphDictionary.find(kpStartVertex->GetOcctVertex())->second;
		for (TopTools_MapIteratorOfMapOfShape mapIterator(occtConnectedVertices);
			mapIterator.More();
			mapIterator.Next())
		{
			Vertex::Ptr connectedVertex = std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(mapIterator.Value()));
			if (std::find_if(rPath.begin(), rPath.end(), TopologyCompare(connectedVertex)) == rPath.end())
			{
				Wire::Ptr extendedPath = Path(connectedVertex, kpEndVertex, rPath);
				if (extendedPath != nullptr)
				{
					return extendedPath;
				}
			}
		}

		return nullptr;
	}

	std::shared_ptr<Wire> Graph::ShortestPath(const Vertex::Ptr & kpStartVertex, const Vertex::Ptr & kpEndVertex, const std::string& rkVertexKey, const std::string& rkEdgeKey) const
	{
		return ShortestPath(kpStartVertex->GetOcctVertex(), kpEndVertex->GetOcctVertex(), rkVertexKey, rkEdgeKey);
	}

	std::shared_ptr<Wire> Graph::ShortestPath(const TopoDS_Vertex & rkOcctStartVertex, const TopoDS_Vertex & rkOcctEndVertex, const std::string& rkVertexKey, const std::string& rkEdgeKey) const
	{
		// Dijkstra's: https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm#Pseudocode
		std::list<TopoDS_Vertex> vertexList;
		std::map<TopoDS_Vertex, double, OcctShapeComparator> distanceMap;
		std::map<TopoDS_Vertex, TopoDS_Vertex, OcctShapeComparator> parentMap;
		for (const std::pair<TopoDS_Vertex, TopTools_MapOfShape>& vertexEdgesPair : m_graphDictionary)
		{
			distanceMap[vertexEdgesPair.first] = std::numeric_limits<double>::max();
			parentMap[vertexEdgesPair.first] = TopoDS_Vertex();
			vertexList.push_back(vertexEdgesPair.first);
		}

		distanceMap[rkOcctStartVertex] = 0;

		while (!vertexList.empty())
		{
			// Find vertex with the lowest distance
			double minDistance = std::numeric_limits<double>::max();
			TopoDS_Vertex occtVertexMinDistance;
			for (const TopoDS_Vertex& kpVertexInQueue : vertexList)
			{
				double distance = distanceMap[kpVertexInQueue];
				if (distance <= minDistance)
				{
					minDistance = distance;
					occtVertexMinDistance = kpVertexInQueue;
				}
			}

			vertexList.remove(occtVertexMinDistance);

			if (occtVertexMinDistance.IsNull())
			{
				continue;
			}

			if (occtVertexMinDistance == rkOcctEndVertex)
			{
				std::list<Vertex::Ptr> path;
				TopoDS_Vertex occtCurrentVertex = occtVertexMinDistance;
				if (parentMap.find(occtCurrentVertex) != parentMap.end() ||
					occtVertexMinDistance == rkOcctStartVertex)
				{
					while (!occtCurrentVertex.IsNull())
					{
						path.push_front(std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtCurrentVertex)));
						std::map<TopoDS_Vertex, TopoDS_Vertex>::iterator parentIterator = parentMap.find(occtCurrentVertex);
						if (parentIterator != parentMap.end())
						{
							occtCurrentVertex = parentIterator->second;
						}
						else
						{
							occtCurrentVertex = TopoDS_Vertex();
						}
					}
				}

				return ConstructPath(path);
			}

			TopTools_MapOfShape occtVertexMinDistanceNeighbours = m_graphDictionary.find(occtVertexMinDistance)->second;
			for (TopTools_MapIteratorOfMapOfShape mapIterator(occtVertexMinDistanceNeighbours);
				mapIterator.More();
				mapIterator.Next())
			{
				TopoDS_Vertex occtNeighbour = TopoDS::Vertex(mapIterator.Value());
				double length = 0.0;
				if (!occtVertexMinDistance.IsSame(occtNeighbour))
				{
					length = ComputeCost(occtVertexMinDistance, occtNeighbour, rkVertexKey, rkEdgeKey);
				}
				double alternativeDistance = distanceMap[occtVertexMinDistance] + length;
				if (alternativeDistance < distanceMap[occtNeighbour])
				{
					distanceMap[occtNeighbour] = alternativeDistance;
					parentMap[occtNeighbour] = occtVertexMinDistance;
				}
			}
		}

		return nullptr;
	}

	void Graph::ShortestPaths(
		const Vertex::Ptr & kpStartVertex,
		const Vertex::Ptr & kpEndVertex,
		const std::string& rkVertexKey,
		const std::string& rkEdgeKey,
		const bool kUseTimeLimit,
		const int kTimeLimit,
		std::list<std::shared_ptr<Wire>>& rPaths) const
	{
		return ShortestPaths(kpStartVertex->GetOcctVertex(), kpEndVertex->GetOcctVertex(), rkVertexKey, rkEdgeKey, kUseTimeLimit, kUseTimeLimit, rPaths);
	}

	void Graph::ShortestPaths(
		const TopoDS_Vertex & rkOcctStartVertex,
		const TopoDS_Vertex & rkOcctEndVertex,
		const std::string& rkVertexKey,
		const std::string& rkEdgeKey,
		const bool kUseTimeLimit,
		const int kTimeLimit,
		std::list<std::shared_ptr<Wire>>& rPaths) const
	{
		if (kTimeLimit <= 0.0)
		{
			throw std::runtime_error("The time limit must have a positive value.");
		}

		auto startingTime = std::chrono::system_clock::now();

		struct Node
		{
			TopoDS_Vertex val;
			std::list<TopoDS_Vertex> path;
			double distance;
		};

		const double kInfiniteDistance = std::numeric_limits<double>::max();

		std::map<TopoDS_Vertex, double, OcctShapeComparator> distanceMap;
		std::list<Vertex::Ptr> vertices;
		Vertices(vertices);
		for (const Vertex::Ptr& kpVertex : vertices)
		{
			distanceMap.insert(std::pair<TopoDS_Vertex, double>(kpVertex->GetOcctVertex(), kInfiniteDistance));
		}
		std::queue<Node> occtVertexQueue;
		std::list<Node> occtNodePaths;
		Node startNode;
		startNode.val = rkOcctStartVertex;
		startNode.path.push_back(rkOcctStartVertex);
		startNode.distance = 0.0;
		occtVertexQueue.push(startNode);

		double minDistance = kInfiniteDistance;
		distanceMap[rkOcctStartVertex] = 0.0;

		while (!occtVertexQueue.empty())
		{
			if (kUseTimeLimit)
			{
				auto currentTime = std::chrono::system_clock::now();
				auto timeDifferenceInSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startingTime);
				if (timeDifferenceInSeconds.count() > kTimeLimit)
				{
					break;
				}
			}

			Node currentNode = occtVertexQueue.front();
			occtVertexQueue.pop();
			if (currentNode.val == rkOcctEndVertex && currentNode.distance <= minDistance)
			{
				minDistance = currentNode.distance;
				occtNodePaths.push_back(currentNode);
			}

			if (currentNode.distance <= minDistance)
			{
				TopTools_MapOfShape occtAdjacentVertices;
				AdjacentVertices(currentNode.val, occtAdjacentVertices);
				for (TopTools_MapIteratorOfMapOfShape occtAdjacentVertexIterator(occtAdjacentVertices);
					occtAdjacentVertexIterator.More();
					occtAdjacentVertexIterator.Next())
				{
					TopoDS_Vertex occtAdjacentVertex = TopoDS::Vertex(occtAdjacentVertexIterator.Value());
					double currentDistance = distanceMap[occtAdjacentVertex];
					if (distanceMap[occtAdjacentVertex] >= currentNode.distance)
					{
						Node adjacentVertex;
						adjacentVertex.val = occtAdjacentVertex;
						adjacentVertex.path = currentNode.path;
						adjacentVertex.path.push_back(occtAdjacentVertex);

						double edgeCost = ComputeEdgeCost(currentNode.val, occtAdjacentVertex, rkEdgeKey);
						adjacentVertex.distance = currentNode.distance + edgeCost;

						double vertexCost = ComputeVertexCost(occtAdjacentVertex, rkVertexKey);
						adjacentVertex.distance += vertexCost;

						distanceMap[occtAdjacentVertex] = adjacentVertex.distance;
						occtVertexQueue.push(adjacentVertex);
					}
				}
			}
		}

		for (const Node& rkNode : occtNodePaths)
		{
			if (rkNode.distance > minDistance)
			{
				continue;
			}

			std::list<Vertex::Ptr> vertices;
			for (const TopoDS_Vertex& rkOcctVertex : rkNode.path)
			{
				Vertex::Ptr vertex = std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(rkOcctVertex));
				vertices.push_back(vertex);
			}
			if (vertices.size() > 1)
			{
				Wire::Ptr path = ConstructPath(vertices);
				if (path != nullptr)
				{
					rPaths.push_back(path);
				}
			}
		}
	}

	int Graph::Diameter() const
	{
		std::map<Vertex::Ptr, Vertex::Ptr> vertexPairs;
		for (std::map<TopoDS_Vertex, TopTools_MapOfShape, OcctShapeComparator>::const_iterator kVertexEdgesPair1 = m_graphDictionary.begin();
			kVertexEdgesPair1 != m_graphDictionary.end();
			kVertexEdgesPair1++)
		{
			TopoDS_Vertex occtVertex1 = kVertexEdgesPair1->first;
			Vertex::Ptr vertex1 = std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtVertex1));

			for (std::map<TopoDS_Vertex, TopTools_MapOfShape, OcctShapeComparator>::const_iterator kVertexEdgesPair2 = kVertexEdgesPair1;
				kVertexEdgesPair2 != m_graphDictionary.end();
				kVertexEdgesPair2++)
			{
				if (kVertexEdgesPair1 == kVertexEdgesPair2)
				{
					continue;
				}

				TopoDS_Vertex occtVertex2 = kVertexEdgesPair2->first;
				Vertex::Ptr vertex2 = std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtVertex2));

				vertexPairs.insert(std::pair<Vertex::Ptr, Vertex::Ptr>(vertex1, vertex2));
			}
		}

		int maxShortestPathDistance = 0;
		for (const std::pair<Vertex::Ptr, Vertex::Ptr>& rkVertexPair : vertexPairs)
		{
			int distance = TopologicalDistance(rkVertexPair.first, rkVertexPair.second);
			if (distance > maxShortestPathDistance)
			{
				maxShortestPathDistance = distance;
			}
		}

		return maxShortestPathDistance;
	}

	int Graph::TopologicalDistance(const std::shared_ptr<Vertex>& kpStartVertex, const std::shared_ptr<Vertex>& kpEndVertex, const double kTolerance) const
	{
		return TopologicalDistance(kpStartVertex->GetOcctVertex(), kpEndVertex->GetOcctVertex(), kTolerance);
	}

	int Graph::TopologicalDistance(const TopoDS_Vertex & rkOcctStartVertex, const TopoDS_Vertex & rkOcctEndVertex, const double kTolerance) const
	{
        if (kTolerance <= 0.0)
        {
            throw std::runtime_error("The tolerance must have a positive value.");
        }

        // Use Breadth-First Search
        TopoDS_Vertex occtCoincidentStartVertex = GetCoincidentVertex(rkOcctStartVertex, kTolerance);
        TopoDS_Vertex occtCoincidentEndVertex = GetCoincidentVertex(rkOcctEndVertex, kTolerance);

		if(occtCoincidentStartVertex.IsSame(occtCoincidentEndVertex))
		{
			return 0;
		}

        std::queue<TopoDS_Vertex> occtVertexQueue;
        TopTools_DataMapOfShapeInteger occtVertexDistanceMap; // also to check if the vertex is processed
        
        occtVertexQueue.push(occtCoincidentStartVertex);
        
        while (!occtVertexQueue.empty())
        {
            TopoDS_Vertex occtCurrentVertex = occtVertexQueue.front();
            occtVertexQueue.pop();
            
            TopTools_MapOfShape occtConnectedEdges;
            GraphMap::const_iterator vertexAdjacencyIter = m_graphDictionary.find(occtCurrentVertex);
            if (vertexAdjacencyIter == m_graphDictionary.end())
            {
                continue;
            }

            for (TopTools_MapIteratorOfMapOfShape occtVertexAdjacency(vertexAdjacencyIter->second);
                occtVertexAdjacency.More();
                occtVertexAdjacency.Next())
            {
                const TopoDS_Vertex& rkOcctAdjacentVertex = TopoDS::Vertex(occtVertexAdjacency.Value());
                try {
                    const int& rDistance = occtVertexDistanceMap.Find(rkOcctAdjacentVertex);

                    // vertex is already processed, so continue.
                    continue; 
                }
                catch (Standard_NoSuchObject&)
                {
                    // not found
                    int parentDistance = 0;
                    try{
                        parentDistance = occtVertexDistanceMap.Find(occtCurrentVertex);
                    }
                    catch (Standard_NoSuchObject&)
                    {
                    }
                    int currentDistance = parentDistance + 1;
                    if (rkOcctAdjacentVertex.IsSame(rkOcctEndVertex))
                    {
                        return currentDistance;
                    }
                    else
                    {
                        occtVertexQueue.push(rkOcctAdjacentVertex);
                        occtVertexDistanceMap.Bind(rkOcctAdjacentVertex, currentDistance);
                    }
                }
            }
        }

        // The start and end Vertices are in different components of a disjoint Graph. Return infinite.
        return std::numeric_limits<int>::max();
	}

	int Graph::Eccentricity(const std::shared_ptr<Vertex>& kpVertex) const
	{
		std::map<TopoDS_Vertex, TopTools_MapOfShape, OcctShapeComparator>::const_iterator occtAdjacentVerticesPair =
			m_graphDictionary.find(kpVertex->GetOcctVertex());
		if (occtAdjacentVerticesPair == m_graphDictionary.end())
		{
			return std::numeric_limits<int>::max(); // infinite distance
		}

		TopTools_MapOfShape occtAdjacentVertices = occtAdjacentVerticesPair->second;
		int eccentricity = 0;
		for (TopTools_MapIteratorOfMapOfShape mapIterator(occtAdjacentVertices);
			mapIterator.More();
			mapIterator.Next())
		{
			int distance = TopologicalDistance(occtAdjacentVerticesPair->first, TopoDS::Vertex(mapIterator.Value()));
			if (distance > eccentricity)
			{
				eccentricity = distance;
			}
		}

		return eccentricity;
	}

	bool Graph::IsErdoesGallai(const std::list<int>& rkSequence) const
	{
		int sum = std::accumulate(rkSequence.begin(), rkSequence.end(), 0);
		if (sum % 2 != 0)
		{
			return false;
		}

		if (IsDegreeSequence(rkSequence))
		{
			std::vector<int> sequenceVector{ std::begin(rkSequence), std::end(rkSequence) };
			int sizeOfSequence = (int)rkSequence.size();

			std::list<int> range(rkSequence.size());
			std::iota(range.begin(), range.end(), 1);

			for (const int k : range)
			{
				int left = std::accumulate(sequenceVector.begin(), sequenceVector.begin() + k, 0);
				std::vector<int> endSequence;
				for (std::vector<int>::iterator x = sequenceVector.begin() + k;
					x != sequenceVector.end();
					x++)
				{
					endSequence.push_back(std::min(*x, k));
				}
				int right = k * (k - 1) + std::accumulate(endSequence.begin(), endSequence.end(), 0);
				if (left > right)
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}

		return true;
	}

	void Graph::RemoveVertices(const std::list<Vertex::Ptr>& rkVertices)
	{
		for (const Vertex::Ptr& kpVertex : rkVertices)
		{
			TopoDS_Vertex occtVertex = kpVertex->GetOcctVertex();

			// Check if the connected vertices are connected to the vertex to be deleted. Remove it.
			for (GraphMap::iterator graphIterator = m_graphDictionary.begin();
				graphIterator != m_graphDictionary.end();
				graphIterator++)
			{
				TopTools_MapOfShape& rOcctConnectedVertices = graphIterator->second;
				rOcctConnectedVertices.Remove(occtVertex);
				TopoDS_Edge occtEdge = FindEdge(TopoDS::Vertex(graphIterator->first), occtVertex);
				if (!occtEdge.IsNull())
				{
					m_occtEdges.Remove(occtEdge);
				}
			}

			// Remove the entry from this map
			GraphMap::iterator vertexIterator = m_graphDictionary.find(occtVertex);
			if (vertexIterator != m_graphDictionary.end())
			{
				TopTools_MapOfShape& rOcctConnectedVertices = vertexIterator->second;
				for (TopTools_MapIteratorOfMapOfShape occtConnectedVertexIterator(rOcctConnectedVertices);
					occtConnectedVertexIterator.More();
					occtConnectedVertexIterator.Next())
				{
					TopoDS_Edge occtEdge = FindEdge(occtVertex, TopoDS::Vertex(occtConnectedVertexIterator.Value()));
					if (!occtEdge.IsNull())
					{
						m_occtEdges.Remove(occtEdge);
					}
				}

				m_graphDictionary.erase(vertexIterator);
			}
		}
	}

	void Graph::RemoveEdges(const std::list<Edge::Ptr>& rkEdges, const double kTolerance)
	{
        if (kTolerance <= 0.0)
        {
            throw std::runtime_error("The tolerance must have a positive value.");
        }

		for (const Edge::Ptr& kpEdge : rkEdges)
		{
			Vertex::Ptr startVertex = kpEdge->StartVertex();
            TopoDS_Vertex occtQueryVertex1 = GetCoincidentVertex(startVertex->GetOcctVertex(), kTolerance);
            if (occtQueryVertex1.IsNull())
            {
                continue;
            }

            Vertex::Ptr endVertex = kpEdge->EndVertex();
            TopoDS_Vertex occtQueryVertex2 = GetCoincidentVertex(endVertex->GetOcctVertex(), kTolerance);
            if (occtQueryVertex2.IsNull())
            {
                continue;
            }

			{
				GraphMap::iterator startVertexIterator = m_graphDictionary.find(occtQueryVertex1);
				if (startVertexIterator != m_graphDictionary.end())
				{
					TopTools_MapOfShape& adjacentVertices = startVertexIterator->second;
					adjacentVertices.Remove(endVertex->GetOcctVertex());
					TopoDS_Edge occtEdge = FindEdge(occtQueryVertex1, endVertex->GetOcctVertex());
					if (!occtEdge.IsNull())
					{
						m_occtEdges.Remove(occtEdge);
					}
				}
			}

			{
				GraphMap::iterator endVertexIterator = m_graphDictionary.find(endVertex->GetOcctVertex());
				if (endVertexIterator != m_graphDictionary.end())
				{
					TopTools_MapOfShape& adjacentVertices = endVertexIterator->second;
					adjacentVertices.Remove(occtQueryVertex1);
					TopoDS_Edge occtEdge = FindEdge(endVertex->GetOcctVertex(), occtQueryVertex1);
					if (!occtEdge.IsNull())
					{
						m_occtEdges.Remove(occtEdge);
					}
				}
			}
		}
	}

	void Graph::VerticesAtCoordinates(const double kX, const double kY, const double kZ, const double kTolerance, std::list<std::shared_ptr<Vertex>>& rVertices) const
	{
		if (kTolerance <= 0.0)
		{
			throw std::runtime_error("The tolerance must have a positive value.");
		}

        gp_Pnt occtQueryPoint(kX, kY, kZ);
		//TopoDS_Vertex occtQueryVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(kX, kY, kZ));
		double absDistanceThreshold = std::abs(kTolerance);
		for (GraphMap::const_iterator graphIterator = m_graphDictionary.begin();
			graphIterator != m_graphDictionary.end();
			graphIterator++)
		{
			TopoDS_Vertex currentVertex = graphIterator->first;
            Handle(Geom_CartesianPoint) currentPoint = new Geom_CartesianPoint(BRep_Tool::Pnt(currentVertex));
            /*BRepExtrema_DistShapeShape occtDistanceCalculation(rkVertex, currentVertex);
            bool isDone = occtDistanceCalculation.Perform();
            if (isDone)
            {
                double distance = occtDistanceCalculation.Value();
                if (distance < absDistanceThreshold)
                {
                    return currentVertex;
                }
            }*/

            double dx = (currentPoint->X() - occtQueryPoint.X());
            double dy = (currentPoint->Y() - occtQueryPoint.Y());
            double dz = (currentPoint->Z() - occtQueryPoint.Z());
            double sqDistance = sqrt(dx * dx + dy * dy + dz * dz);
            if (sqDistance < absDistanceThreshold)
            {
                Vertex::Ptr closestVertex = std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(currentVertex));
                rVertices.push_back(closestVertex);
            }
			//BRepExtrema_DistShapeShape occtDistanceCalculation(occtQueryVertex, currentVertex);
			/*bool isDone = occtDistanceCalculation.Perform();
			if (isDone)
			{
				double distance = occtDistanceCalculation.Value();
				if (distance < absDistanceThreshold)
				{
					Vertex::Ptr closestVertex = std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(currentVertex));
					rVertices.push_back(closestVertex);
				}
			}*/
		}
	}

	std::shared_ptr<Edge> Graph::Edge(const std::shared_ptr<Vertex>& kpVertex1, const std::shared_ptr<Vertex>& kpVertex2, const double kTolerance) const
	{
		if (kTolerance <= 0.0)
		{
			throw std::runtime_error("The tolerance must have a positive value.");
		}

		TopoDS_Vertex occtQueryVertex1 = GetCoincidentVertex(kpVertex1->GetOcctVertex(), kTolerance);
		if (occtQueryVertex1.IsNull())
		{
			return nullptr;
		}
		TopoDS_Vertex occtQueryVertex2 = GetCoincidentVertex(kpVertex2->GetOcctVertex(), kTolerance);
		if (occtQueryVertex2.IsNull())
		{
			return nullptr;
		}

		GraphMap::const_iterator kAdjacentVerticesIterator1 = m_graphDictionary.find(occtQueryVertex1);
		if (kAdjacentVerticesIterator1 == m_graphDictionary.end())
		{
			return nullptr;
		}

		TopTools_MapOfShape adjacentVerticesToVertex1 = kAdjacentVerticesIterator1->second;
		if (!adjacentVerticesToVertex1.Contains(occtQueryVertex2))
		{
			return nullptr;
		}

		TopoDS_Edge occtEdge = FindEdge(occtQueryVertex1, occtQueryVertex2);
		if (occtEdge.IsNull())
		{
			return nullptr;
		}
		return std::dynamic_pointer_cast<TopologicCore::Edge>(Topology::ByOcctShape(occtEdge));
	}

	void Graph::IncidentEdges(const std::shared_ptr<Vertex>& kpVertex, const double kTolerance, std::list<std::shared_ptr<TopologicCore::Edge>>& rEdges) const
	{
		TopoDS_Vertex occtQueryVertex = GetCoincidentVertex(kpVertex->GetOcctVertex(), kTolerance);
		if (occtQueryVertex.IsNull())
		{
			return;
		}

		Vertex::Ptr queryVertex = std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtQueryVertex));
		std::list<Vertex::Ptr> adjacentVertices;
		AdjacentVertices(queryVertex, adjacentVertices);
		for (const Vertex::Ptr& kpAdjacentVertex : adjacentVertices)
		{
			TopoDS_Edge occtEdge = FindEdge(queryVertex->GetOcctVertex(), kpAdjacentVertex->GetOcctVertex());
			if (occtEdge.IsNull())
			{
				continue;
			}
			Edge::Ptr edge = std::dynamic_pointer_cast<TopologicCore::Edge>(Topology::ByOcctShape(occtEdge));

			rEdges.push_back(edge);
		}
	}

	Vertex::Ptr CalculateGraphVertexFromAperture(const Aperture::Ptr kpAperture, const bool kUseFaceInternalVertex, const double kTolerance)
	{
		Topology::Ptr apertureTopology = kpAperture->Topology();
		if (apertureTopology->GetType() == TOPOLOGY_FACE)
		{
			Face::Ptr apertureFace = TopologicalQuery::Downcast<Face>(apertureTopology);
			if (kUseFaceInternalVertex)
			{
				return TopologicUtilities::FaceUtility::InternalVertex(
					apertureFace,
					kTolerance
				);
			}
			else
			{
				return apertureFace->CenterOfMass();
			}
		}
		else if (apertureTopology->GetType() == TOPOLOGY_CELL)
		{
			return TopologicUtilities::CellUtility::InternalVertex(
				TopologicalQuery::Downcast<Cell>(apertureTopology),
				kTolerance
			);
		}

		// else
		return kpAperture->CenterOfMass();
	}

	Graph::Ptr Graph::ByVertex(const Vertex::Ptr kpVertex,
		const bool kToExteriorApertures,
		const bool kUseFaceInternalVertex,
		const double kTolerance)
	{
		std::list<Vertex::Ptr> apertureCentresOfMass;
		if (kToExteriorApertures)
		{
			std::list<Topology::Ptr> contents;
			kpVertex->Contents(contents);

			for (const Topology::Ptr& kpContent : contents)
			{
				if (kpContent->GetType() == TOPOLOGY_APERTURE)
				{
					Vertex::Ptr graphVertex = CalculateGraphVertexFromAperture(
						TopologicalQuery::Downcast<Aperture>(kpContent), 
						kUseFaceInternalVertex, 
						kTolerance);
					AttributeManager::GetInstance().CopyAttributes(kpContent->GetOcctShape(), graphVertex->GetOcctShape());
					apertureCentresOfMass.push_back(graphVertex);
				}
			}
		}

		std::list<Vertex::Ptr> vertices;
		std::list<Edge::Ptr> edges;
		for(const Vertex::Ptr& kpApertureCentreOfMass : apertureCentresOfMass)
		{ 
			Edge::Ptr edge = Edge::ByStartVertexEndVertex(kpVertex, kpApertureCentreOfMass);
			edges.push_back(edge);
		}
		if (edges.empty())
		{
			vertices.push_back(kpVertex);
		}
		return std::make_shared<Graph>(vertices, edges);
	}

	Graph::Ptr Graph::ByEdge(const Edge::Ptr kpEdge,
		const bool kDirect,
		const bool kToExteriorApertures,
		const bool kUseFaceInternalVertex,
		const double kTolerance)
	{
		std::list<Vertex::Ptr> vertices;
		std::list<Edge::Ptr> edges;
		std::list<Vertex::Ptr> edgeVertices;

		if (kDirect)
		{
			kpEdge->Vertices(vertices);
			edges.push_back(kpEdge);
		}

		std::list<Vertex::Ptr> apertureCentresOfMass;
		if (kToExteriorApertures)
		{
			std::list<Topology::Ptr> contents;
			kpEdge->Contents(contents);

			for (const Topology::Ptr& kpContent : contents)
			{
				if (kpContent->GetType() == TOPOLOGY_APERTURE)
				{
					Vertex::Ptr contentCenterOfMass = CalculateGraphVertexFromAperture(
						TopologicalQuery::Downcast<Aperture>(kpContent),
						kUseFaceInternalVertex,
						kTolerance);
					AttributeManager::GetInstance().CopyAttributes(kpContent->GetOcctShape(), contentCenterOfMass->GetOcctShape());
					vertices.push_back(contentCenterOfMass);
					std::list<Vertex::Ptr> edgeVertices;
					kpEdge->Vertices(edgeVertices);

					for (const Vertex::Ptr& kpVertex : edgeVertices)
					{
						Edge::Ptr edge = Edge::ByStartVertexEndVertex(kpVertex, contentCenterOfMass);
						edges.push_back(edge);
					}
				}
			}
		}

		return std::make_shared<Graph>(vertices, edges);
	}

	Graph::Ptr Graph::ByWire(const Wire::Ptr kpWire,
		const bool kDirect,
		const bool kToExteriorApertures,
		const bool kUseFaceInternalVertex,
		const double kTolerance)
	{
		std::list<Vertex::Ptr> vertices;
		std::list<Edge::Ptr> edges;
		if (kDirect || kToExteriorApertures)
		{
			kpWire->Edges(edges);
			kpWire->Vertices(vertices);
		}

		if (kToExteriorApertures)
		{
			// Iterate through the edges
			for (const Edge::Ptr& kpEdge : edges)
			{
				// Get the apertures
				std::list<Topology::Ptr> contents;
				kpEdge->Contents(contents);

				std::list<Vertex::Ptr> edgeVertices;
				kpEdge->Vertices(edgeVertices);

				for (const Topology::Ptr& kpContent : contents)
				{
					if (kpContent->GetType() == TOPOLOGY_APERTURE)
					{
						Vertex::Ptr contentCenterOfMass = CalculateGraphVertexFromAperture(
							TopologicalQuery::Downcast<Aperture>(kpContent), kUseFaceInternalVertex, kTolerance);
						AttributeManager::GetInstance().CopyAttributes(kpContent->GetOcctShape(), contentCenterOfMass->GetOcctShape());
						vertices.push_back(contentCenterOfMass);
						for (const Vertex::Ptr& edgeVertex :edgeVertices)
						{
							Edge::Ptr edge = Edge::ByStartVertexEndVertex(edgeVertex, contentCenterOfMass);
							edges.push_back(edge);
						}
					}
				}
			}
		}

		return std::make_shared<Graph>(vertices, edges);
	}

	Graph::Ptr Graph::ByFace(const Face::Ptr kpFace,
		const bool kToExteriorTopologies,
		const bool kToExteriorApertures,
		const bool kUseFaceInternalVertex,
		const double kTolerance)
	{
		std::list<Vertex::Ptr> vertices;
		std::list<Edge::Ptr> edges;

		Vertex::Ptr internalVertex = nullptr;
		if (kUseFaceInternalVertex)
		{
			internalVertex = TopologicUtilities::FaceUtility::InternalVertex(kpFace, kTolerance);
		}
		else
		{
			internalVertex = kpFace->CenterOfMass();
		}
		AttributeManager::GetInstance().CopyAttributes(kpFace->GetOcctShape(), internalVertex->GetOcctShape());
		vertices.push_back(internalVertex);

		if (kToExteriorTopologies || kToExteriorApertures)
		{
			std::list<Edge::Ptr> faceEdges;
			kpFace->Edges(faceEdges);

			for (const Edge::Ptr& kpFaceEdge : faceEdges)
			{
				if (kToExteriorTopologies)
				{
					Vertex::Ptr theOtherVertex = kpFaceEdge->CenterOfMass();
					AttributeManager::GetInstance().CopyAttributes(kpFaceEdge->GetOcctShape(), theOtherVertex->GetOcctShape());
					Edge::Ptr edge = Edge::ByStartVertexEndVertex(internalVertex, theOtherVertex);
					edges.push_back(edge);
				}

				if (kToExteriorApertures)
				{
					std::list<Topology::Ptr> contents;
					kpFaceEdge->Contents(contents);

					for (const Topology::Ptr& kpContent : contents)
					{
						if (kpContent->GetType() == TOPOLOGY_APERTURE)
						{
							Vertex::Ptr pApertureCenterOfMass = CalculateGraphVertexFromAperture(
								TopologicalQuery::Downcast<Aperture>(kpContent),
								kUseFaceInternalVertex, kTolerance);
							vertices.push_back(pApertureCenterOfMass);
							Edge::Ptr edge = Edge::ByStartVertexEndVertex(pApertureCenterOfMass, internalVertex);
							edges.push_back(edge);
						}
					}
				}
			}
		}

		return std::make_shared<Graph>(vertices, edges);
	}

	Graph::Ptr Graph::ByShell(const Shell::Ptr kpShell,
		const bool kDirect,
		const bool kViaSharedTopologies,
		const bool kViaSharedApertures,
		const bool kToExteriorTopologies,
		const bool kToExteriorApertures,
		const bool kUseFaceInternalVertex,
		const double kTolerance)
	{
		if (kpShell == nullptr)
		{
			return nullptr;
		}

		// 1. Get the vertices mapped to their original topologies
		//    - Face --> centroid
		//    Occt shapes must be used as the keys. Topologic shapes cannot be used because 
		//    there can be many shapes representing the same OCCT shapes.
		std::map<TopoDS_Face, TopologicCore::Vertex::Ptr, TopologicCore::OcctShapeComparator> faceCentroids;

		std::list<TopologicCore::Face::Ptr> faces;
		kpShell->Faces(faces);
		for (const TopologicCore::Face::Ptr& kpFace : faces)
		{
			Vertex::Ptr internalVertex = nullptr;
			if (kUseFaceInternalVertex)
			{
				internalVertex = TopologicUtilities::FaceUtility::InternalVertex(kpFace, kTolerance);
			}
			else
			{
				internalVertex = kpFace->CenterOfMass();
			}
			AttributeManager::GetInstance().CopyAttributes(kpFace->GetOcctShape(), internalVertex->GetOcctShape());
			faceCentroids.insert(std::make_pair(kpFace->GetOcctFace(), internalVertex));
		}

		// 2. Check the configurations. Add the edges to a cluster.
		std::list<TopologicCore::Edge::Ptr> graphEdges;
		if (kDirect)
		{
			// Iterate through all faces and check for adjacency.
			// For each adjacent faces, connect the centroids
			for (const TopologicCore::Face::Ptr& kpFace : faces)
			{
				std::list<TopologicCore::Face::Ptr> adjacentFaces;
				//kpFace->AdjacentFaces(adjacentFaces); // here
                TopologicUtilities::FaceUtility::AdjacentFaces(kpFace.get(), kpShell, adjacentFaces);

				for (const TopologicCore::Face::Ptr& kpAdjacentFace : adjacentFaces)
				{
					std::map<TopoDS_Face, TopologicCore::Vertex::Ptr, TopologicCore::OcctShapeComparator>::iterator adjacentCentroidPair =
						faceCentroids.find(kpAdjacentFace->GetOcctFace());
					if (adjacentCentroidPair == faceCentroids.end())
					{
						continue;
					}
					TopologicCore::Edge::Ptr pEdge = TopologicCore::Edge::ByStartVertexEndVertex(faceCentroids[kpFace->GetOcctFace()], adjacentCentroidPair->second);
					graphEdges.push_back(pEdge);
				}
			}
		}

		std::list<TopologicCore::Edge::Ptr> edges;
		kpShell->Edges(edges);
		for (const TopologicCore::Edge::Ptr& kpEdge : edges)
		{
			TopologicCore::Vertex::Ptr pCentroid = kpEdge->CenterOfMass();
			bool isManifold = kpEdge->IsManifold(kpShell); // here
			AttributeManager::GetInstance().CopyAttributes(kpEdge->GetOcctShape(), pCentroid->GetOcctShape());

			std::list<TopologicCore::Face::Ptr> adjacentFaces;
            TopologicUtilities::EdgeUtility::AdjacentFaces(kpEdge, kpShell, adjacentFaces);
			//kpEdge->Faces(adjacentFaces); // here

			std::list<Topology::Ptr> contents;
			kpEdge->Contents(contents);

			// Get the apertures and calculate their centroids
			std::list<TopologicCore::Vertex::Ptr> apertureCentroids;
			for (const Topology::Ptr& kpContent : contents)
			{
				// If this is not an aperture, skip it
				if (kpContent->GetType() != TopologicCore::TOPOLOGY_APERTURE)
				{
					continue;
				}
				TopologicCore::Aperture::Ptr pAperture = TopologicalQuery::Downcast<TopologicCore::Aperture>(kpContent);
				TopologicCore::Vertex::Ptr pApertureCentroid = CalculateGraphVertexFromAperture(pAperture, kUseFaceInternalVertex, kTolerance);

				AttributeManager::GetInstance().CopyAttributes(pAperture->GetOcctShape(), pApertureCentroid->GetOcctShape());
				apertureCentroids.push_back(pApertureCentroid);
			}

			// Check 
			for (const TopologicCore::Face::Ptr& kpAdjacentFace : adjacentFaces)
			{
				if ((!isManifold && kViaSharedTopologies) // i.e. non-manifold faces
					||
					(isManifold && kToExteriorTopologies))
				{
					TopologicCore::Edge::Ptr pEdge = TopologicCore::Edge::ByStartVertexEndVertex(pCentroid, faceCentroids[kpAdjacentFace->GetOcctFace()]);
					graphEdges.push_back(pEdge);
				}

				for (const TopologicCore::Vertex::Ptr& rkApertureCentroid : apertureCentroids)
				{
					if ((!isManifold && kViaSharedApertures) // i.e. non-manifold apertures
						||
						(isManifold && kToExteriorApertures))
					{
						TopologicCore::Edge::Ptr pEdge = TopologicCore::Edge::ByStartVertexEndVertex(rkApertureCentroid, faceCentroids[kpAdjacentFace->GetOcctFace()]);
						graphEdges.push_back(pEdge);
					}
				}
			}
		}
		std::list<Vertex::Ptr> vertices;
		for (const Topology::Ptr& kpEdgeTopology : graphEdges)
		{
			std::list<Vertex::Ptr> edgeVertices;
			kpEdgeTopology->Vertices(edgeVertices);
			for (const Vertex::Ptr& kpVertex : edgeVertices)
			{
				vertices.push_back(kpVertex);
			}
		}
		return std::make_shared<Graph>(vertices, graphEdges);
	}

	Graph::Ptr Graph::ByCell(const Cell::Ptr kpCell,
		const bool kToExteriorTopologies,
		const bool kToExteriorApertures,
		const bool kUseFaceInternalVertex,
		const double kTolerance)
	{
		std::list<Vertex::Ptr> vertices;
		std::list<Edge::Ptr> edges;

		Vertex::Ptr internalVertex = TopologicUtilities::CellUtility::InternalVertex(kpCell, kTolerance);
		AttributeManager::GetInstance().CopyAttributes(kpCell->GetOcctShape(), internalVertex->GetOcctShape());
		vertices.push_back(internalVertex);
		if (kToExteriorTopologies || kToExteriorApertures)
		{
			std::list<Face::Ptr> cellFaces;
			kpCell->Faces(cellFaces);

			for (const Face::Ptr& kpCellFace : cellFaces)
			{
				if (kToExteriorTopologies)
				{
					Vertex::Ptr pCellFaceCenterOfMass = TopologicUtilities::FaceUtility::InternalVertex(kpCellFace, kTolerance);
					vertices.push_back(pCellFaceCenterOfMass);
					AttributeManager::GetInstance().CopyAttributes(kpCellFace->GetOcctShape(), pCellFaceCenterOfMass->GetOcctShape());
					Edge::Ptr edge = Edge::ByStartVertexEndVertex(pCellFaceCenterOfMass, internalVertex);
					edges.push_back(edge);
				}

				if (kToExteriorApertures)
				{
					std::list<Topology::Ptr> contents;
					kpCellFace->Contents(contents);

					for (const Topology::Ptr& kpContent : contents)
					{
						if (kpContent->GetType() == TOPOLOGY_APERTURE)
						{
							Vertex::Ptr pApertureCenterOfMass = CalculateGraphVertexFromAperture(
								TopologicalQuery::Downcast<TopologicCore::Aperture>(kpContent),
								kUseFaceInternalVertex,
								kTolerance);

							vertices.push_back(pApertureCenterOfMass);
							Edge::Ptr edge = Edge::ByStartVertexEndVertex(pApertureCenterOfMass, internalVertex);
							edges.push_back(edge);
						}
					}
				}
			}
		}

		return std::make_shared<Graph>(vertices, edges);
	}

	Graph::Ptr Graph::ByCellComplex(const CellComplex::Ptr kpCellComplex,
		const bool kDirect,
		const bool kViaSharedTopologies,
		const bool kViaSharedApertures,
		const bool kToExteriorTopologies,
		const bool kToExteriorApertures,
		const bool kUseFaceInternalVertex,
		const double kTolerance)
	{
		if (kpCellComplex == nullptr)
		{
			return nullptr;
		}

		// 1. Get the vertices mapped to their original topologies
		//    - Cell --> centroid
		//   Occt shapes must be used as the keys. Topologic shapes cannot be used because there can be many shapes representing the same OCCT shapes.
		std::map<TopoDS_Solid, TopologicCore::Vertex::Ptr, TopologicCore::OcctShapeComparator> cellCentroids;

		std::list<TopologicCore::Cell::Ptr> cells;
		kpCellComplex->Cells(cells);
		for (const TopologicCore::Cell::Ptr& kpCell : cells)
		{
			TopologicCore::Vertex::Ptr pCentroid = TopologicUtilities::CellUtility::InternalVertex(kpCell, kTolerance);
			AttributeManager::GetInstance().CopyAttributes(kpCell->GetOcctShape(), pCentroid->GetOcctShape());
			cellCentroids.insert(std::make_pair(kpCell->GetOcctSolid(), pCentroid));
		}

		// 2. If direct = true, check cellAdjacency.
		std::list<TopologicCore::Edge::Ptr> edges;
		if (kDirect)
		{
			TopTools_DataMapOfShapeListOfShape occtCellAdjacency;
			for (const TopologicCore::Cell::Ptr& kpCell : cells)
			{
				// Get faces
				std::list<Face::Ptr> faces;
				kpCell->Faces(faces);

				// Get adjacent cells. Only add here if the cell is not already added here, and 
				// the reverse is not in occtCellAdjacency.
				TopTools_ListOfShape occtCellUncheckedAdjacentCells;
				for (const Face::Ptr& kpFace : faces)
				{
					std::list<TopologicCore::Cell::Ptr> currentFaceAdjacentCells;
					TopologicUtilities::FaceUtility::AdjacentCells(kpFace, kpCellComplex, currentFaceAdjacentCells);

					for (const Cell::Ptr& kpCurrentFaceAdjacentCell : currentFaceAdjacentCells)
					{
						// The same as this Cell? Continue.
						if (kpCurrentFaceAdjacentCell->IsSame(kpCell))
						{
							continue;
						}

						// Is Cell already added in this list (occtCellAdjacentCells)? Continue.
						if (occtCellUncheckedAdjacentCells.Contains(kpCurrentFaceAdjacentCell->GetOcctShape()))
						{
							continue;
						}

						// Is the reverse already added in occtCellAdjacency? Continue.
						try {
							const TopTools_ListOfShape& reverseAdjacency = occtCellAdjacency.Find(kpCurrentFaceAdjacentCell->GetOcctShape());
							if (reverseAdjacency.Contains(kpCell->GetOcctShape()))
							{
								continue;
							}
						}
						catch (...)
						{

						}

						// If passes the tests, add to occtCellUncheckedAdjacentCells
						occtCellUncheckedAdjacentCells.Append(kpCurrentFaceAdjacentCell->GetOcctShape());
					}
				}

				if (!occtCellUncheckedAdjacentCells.IsEmpty())
				{
					occtCellAdjacency.Bind(kpCell->GetOcctShape(), occtCellUncheckedAdjacentCells);
				}
			}


			// Create the edges from the Cell adjacency information
			for (TopTools_DataMapIteratorOfDataMapOfShapeListOfShape occtCellAdjacencyIterator(occtCellAdjacency);
				occtCellAdjacencyIterator.More();
				occtCellAdjacencyIterator.Next())
			{
				const TopoDS_Solid& rkOcctCell = TopoDS::Solid(occtCellAdjacencyIterator.Key());
				Cell::Ptr pCell = std::make_shared<Cell>(rkOcctCell);
				Vertex::Ptr pCellInternalVertex = nullptr;
				try {
					auto internalVertexIterator = cellCentroids.find(rkOcctCell);
					pCellInternalVertex = internalVertexIterator->second;
				}
				catch (...)
				{
					assert(false);
					throw std::runtime_error("No Cell internal vertex pre-computed.");
				}
				const TopTools_ListOfShape& rkOcctAdjacentCells = occtCellAdjacencyIterator.Value();

				for (TopTools_ListIteratorOfListOfShape occtAdjacentCellIterator(rkOcctAdjacentCells);
					occtAdjacentCellIterator.More();
					occtAdjacentCellIterator.Next())
				{
					const TopoDS_Solid& rkOcctAdjacentCell = TopoDS::Solid(occtAdjacentCellIterator.Value());
					Cell::Ptr pAdjacentCell = std::make_shared<Cell>(rkOcctAdjacentCell);
					Vertex::Ptr pAdjacentInternalVertex = nullptr;
					try {
						auto internalVertexIterator = cellCentroids.find(rkOcctAdjacentCell);
						pAdjacentInternalVertex = internalVertexIterator->second;
					}
					catch (...)
					{
						assert(false);
						throw std::runtime_error("No Cell internal vertex pre-computed.");
					}
					TopologicCore::Edge::Ptr pEdge = TopologicCore::Edge::ByStartVertexEndVertex(
						pCellInternalVertex,
						pAdjacentInternalVertex);
					edges.push_back(pEdge);
				}
			}
		}

		std::list<TopologicCore::Face::Ptr> faces;
		kpCellComplex->Faces(faces);
		for (const TopologicCore::Face::Ptr& kpFace : faces)
		{
			Vertex::Ptr internalVertex = nullptr;
			if (kUseFaceInternalVertex)
			{
				internalVertex = TopologicUtilities::FaceUtility::InternalVertex(kpFace, kTolerance);
			}
			else
			{
				internalVertex = kpFace->CenterOfMass();
			}
			AttributeManager::GetInstance().CopyAttributes(kpFace->GetOcctShape(), internalVertex->GetOcctShape());
			
			bool isManifold = kpFace->IsManifoldToTopology(kpCellComplex);
			std::list<TopologicCore::Cell::Ptr> adjacentCells;
            TopologicUtilities::FaceUtility::AdjacentCells(kpFace, kpCellComplex, adjacentCells);

			std::list<Topology::Ptr> contents;
			kpFace->Contents(contents);

			// Get the apertures and calculate their centroids
			std::list<TopologicCore::Vertex::Ptr> apertureCentroids;
			for (const Topology::Ptr& kpContent : contents)
			{
				// If this is not an aperture, skip it
				if (kpContent->GetType() != TopologicCore::TOPOLOGY_APERTURE)
				{
					continue;
				}
				TopologicCore::Aperture::Ptr pAperture = TopologicalQuery::Downcast<TopologicCore::Aperture>(kpContent);
				TopologicCore::Vertex::Ptr pApertureCentroid = CalculateGraphVertexFromAperture(pAperture, kUseFaceInternalVertex, kTolerance);

				AttributeManager::GetInstance().CopyAttributes(pAperture->GetOcctShape(), pApertureCentroid->GetOcctShape());
				apertureCentroids.push_back(pApertureCentroid);
			}

			// Check 
			for (const TopologicCore::Cell::Ptr& kpAdjacentCell : adjacentCells)
			{
				if ((!isManifold && kViaSharedTopologies) // i.e. non-manifold faces
					||
					(isManifold && kToExteriorTopologies))
				{
					std::map<TopoDS_Solid, TopologicCore::Vertex::Ptr, TopologicCore::OcctShapeComparator>::iterator adjacentCellIterator =
						cellCentroids.find(kpAdjacentCell->GetOcctSolid());
					if (adjacentCellIterator == cellCentroids.end())
					{
						continue;
					}
					TopologicCore::Edge::Ptr pEdge = TopologicCore::Edge::ByStartVertexEndVertex(
						internalVertex, 
						adjacentCellIterator->second);
					if (pEdge != nullptr)
					{
						edges.push_back(pEdge);
					}
				}

				for (const TopologicCore::Vertex::Ptr& rkApertureCentroid : apertureCentroids)
				{
					if ((!isManifold && kViaSharedApertures) // i.e. non-manifold apertures
						||
						(isManifold && kToExteriorApertures))
					{
						std::map<TopoDS_Solid, TopologicCore::Vertex::Ptr, TopologicCore::OcctShapeComparator>::iterator adjacentCellIterator =
							cellCentroids.find(kpAdjacentCell->GetOcctSolid());
						if (adjacentCellIterator == cellCentroids.end())
						{
							continue;
						}
						TopologicCore::Edge::Ptr pEdge = TopologicCore::Edge::ByStartVertexEndVertex(rkApertureCentroid, adjacentCellIterator->second);
						if (pEdge != nullptr)
						{
							edges.push_back(pEdge);
						}
					}
				}
			}
		}
		std::list<Vertex::Ptr> vertices;
		for (const Edge::Ptr& kpEdgeTopology : edges)
		{
			std::list<Vertex::Ptr> edgeVertices;
			kpEdgeTopology->Vertices(edgeVertices);
			for (const Vertex::Ptr& kpVertex : edgeVertices)
			{
				vertices.push_back(kpVertex);
			}
		}
		return std::make_shared<Graph>(vertices, edges);
	}

	Graph::Ptr Graph::ByCluster(const Cluster::Ptr cluster,
		const bool kDirect,
		const bool kViaSharedTopologies,
		const bool kViaSharedApertures,
		const bool kToExteriorTopologies,
		const bool kToExteriorApertures,
		const bool kUseFaceInternalVertex,
		const double kTolerance)
	{
		std::list<Topology::Ptr> subtopologies;
		cluster->SubTopologies(subtopologies);
		std::list<Vertex::Ptr> vertices;
		std::list<Edge::Ptr> edges;
		for (const Topology::Ptr& kpSubtopology : subtopologies)
		{
			Graph::Ptr graph = Graph::ByTopology(
				kpSubtopology, kDirect, kViaSharedTopologies, kViaSharedApertures, kToExteriorTopologies, kToExteriorApertures, kUseFaceInternalVertex, kTolerance);
			std::list<Vertex::Ptr> subtopologyVertices;
			graph->Vertices(subtopologyVertices);
			std::list<Edge::Ptr> subtopologyEdges;
			graph->Edges(subtopologyEdges);
			vertices.insert(vertices.end(), subtopologyVertices.begin(), subtopologyVertices.end());
			edges.insert(edges.end(), subtopologyEdges.begin(), subtopologyEdges.end());
		}
		return std::make_shared<Graph>(vertices, edges);
	}

	Wire::Ptr Graph::ConstructPath(const std::list<Vertex::Ptr>& rkPathVertices) const
	{
		auto startingTime = std::chrono::system_clock::now();
		return ConstructPath(rkPathVertices, false, 0, startingTime);
	}

	std::shared_ptr<Wire> Graph::ConstructPath(
		const std::list<Vertex::Ptr>& rkPathVertices, 
		const bool kUseTimeLimit, 
		const int kTimeLimitInSeconds,
		const std::chrono::system_clock::time_point& rkStartingTime) const
	{
		std::list<Vertex::Ptr>::const_iterator lastVertexIterator = rkPathVertices.end();
		lastVertexIterator--;
		std::list<Edge::Ptr> edges;
		for (std::list<Vertex::Ptr>::const_iterator vertexIterator = rkPathVertices.begin();
			vertexIterator != lastVertexIterator;
			vertexIterator++)
		{
			if (kUseTimeLimit)
			{
				auto currentTime = std::chrono::system_clock::now();
				auto timeDifferenceInSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - rkStartingTime);
				if (timeDifferenceInSeconds.count() >= kTimeLimitInSeconds)
				{
					return nullptr;
				}
			}

			std::list<Vertex::Ptr>::const_iterator nextVertexIterator = vertexIterator;
			nextVertexIterator++;
			TopoDS_Edge occtEdge = FindEdge((*vertexIterator)->GetOcctVertex(), (*nextVertexIterator)->GetOcctVertex());

			Edge::Ptr edge = nullptr;
			if (!occtEdge.IsNull())
			{
				edge = std::dynamic_pointer_cast<TopologicCore::Edge>(Topology::ByOcctShape(occtEdge));
			}else
			{
				edge = Edge::ByStartVertexEndVertex(*vertexIterator, *nextVertexIterator);
			}

			edges.push_back(edge);
		}
		if (edges.empty())
		{
			return nullptr;
		}
		Wire::Ptr pathWire = Wire::ByEdges(edges);
		return pathWire;
	}

	bool Graph::IsDegreeSequence(const std::list<int>& rkSequence) const
	{
		std::list<int>::const_iterator beforeEndIterator = rkSequence.end();
		beforeEndIterator--;

		for(std::list<int>::const_iterator sequenceIterator = rkSequence.begin();
			sequenceIterator != beforeEndIterator;
			sequenceIterator++)
		{
			std::list<int>::const_iterator nextIterator = sequenceIterator;
			nextIterator++;

			if (*nextIterator > *sequenceIterator)
			{
				return false;
			}
		}

		return true;
	}

	TopoDS_Vertex Graph::GetCoincidentVertex(const TopoDS_Vertex & rkVertex, const double kTolerance) const
    {
        Handle(Geom_CartesianPoint) point = new Geom_CartesianPoint(BRep_Tool::Pnt(rkVertex));
		double absDistanceThreshold = std::abs(kTolerance);
		for (GraphMap::const_iterator graphIterator = m_graphDictionary.begin();
			graphIterator != m_graphDictionary.end();
			graphIterator++)
		{
			TopoDS_Vertex currentVertex = graphIterator->first;
            Handle(Geom_CartesianPoint) currentPoint = new Geom_CartesianPoint(BRep_Tool::Pnt(currentVertex));
            /*BRepExtrema_DistShapeShape occtDistanceCalculation(rkVertex, currentVertex);
			bool isDone = occtDistanceCalculation.Perform();
			if (isDone)
			{
				double distance = occtDistanceCalculation.Value();
				if (distance < absDistanceThreshold)
				{
					return currentVertex;
				}
			}*/

            double dx = (currentPoint->X() - point->X());
            double dy = (currentPoint->Y() - point->Y());
            double dz = (currentPoint->Z() - point->Z());
            double sqDistance = dx*dx + dy*dy + dz*dz;
            if (sqDistance < absDistanceThreshold)
            {
                return currentVertex;
            }
		}

		return TopoDS_Vertex(); // null vertex
	}

	double Graph::ComputeCost(const TopoDS_Vertex & rkVertex1, const TopoDS_Vertex & rkVertex2, const std::string & rkVertexKey, const std::string & rkEdgeKey) const
	{
		double edgeCost = ComputeEdgeCost(rkVertex1, rkVertex2, rkEdgeKey);
		if (edgeCost >= std::numeric_limits<double>::max())
		{
			return edgeCost;
		}
		double vertexCost = ComputeVertexCost(rkVertex2, rkVertexKey);
		return vertexCost + edgeCost;
	}

	double Graph::ComputeVertexCost(const TopoDS_Vertex & rkVertex, const std::string & rkVertexKey) const
	{
		if (rkVertexKey.compare("") == 0)
		{
			return 0.0;
		}


		AttributeManager::AttributeMap attributeMap;
		bool hasAttribute = AttributeManager::GetInstance().FindAll(rkVertex, attributeMap);
		if (!hasAttribute)
		{
			return 0.0;
		}

		AttributeManager::AttributeMap::iterator attributeIterator = attributeMap.find(rkVertexKey);
		Attribute::Ptr attribute = attributeIterator->second;

		// Only add if double or int
		DoubleAttribute::Ptr doubleAttribute = std::dynamic_pointer_cast<DoubleAttribute>(attribute);
		if (doubleAttribute != nullptr)
		{
			return doubleAttribute->DoubleValue();
		}

		IntAttribute::Ptr intAttribute = std::dynamic_pointer_cast<IntAttribute>(attribute);
		if (intAttribute != nullptr)
		{
			return (double)intAttribute->IntValue();
		}

		return 0.0;
	}

	double Graph::ComputeEdgeCost(const TopoDS_Vertex & rkVertex1, const TopoDS_Vertex & rkVertex2, const std::string & rkEdgeKey) const
	{
		// Check: if not connected, return the largest double value
		if (!ContainsEdge(rkVertex1, rkVertex2, 0.0001))
		{
			return std::numeric_limits<double>::max();
		}
		else
		{
			// Check edge key
			if (rkEdgeKey.compare("") == 0)
			{
				return 1.0;
			}
			else
			{
				TopoDS_Edge occtEdge = FindEdge(rkVertex1, rkVertex2);
				if (occtEdge.IsNull())
				{
					return std::numeric_limits<double>::max();
				}

				AttributeManager::AttributeMap attributeMap;
				bool hasAttribute = AttributeManager::GetInstance().FindAll(occtEdge, attributeMap);
				std::string lowercaseEdgeKey = rkEdgeKey;
				std::transform(lowercaseEdgeKey.begin(), lowercaseEdgeKey.end(), lowercaseEdgeKey.begin(), ::tolower);
				AttributeManager::AttributeMap::iterator attributeIterator = attributeMap.find(lowercaseEdgeKey);
				if (attributeIterator == attributeMap.end())
				{
					if ((lowercaseEdgeKey.compare("distance") == 0 || lowercaseEdgeKey.compare("length") == 0)) // no attribute with this name is found
					{
						BRepExtrema_DistShapeShape occtDistance(rkVertex1, rkVertex2);
						return occtDistance.Value();
					}
					else
					{
						return 1.0;
					}
				}
				else
				{
					Attribute::Ptr attribute = attributeIterator->second;

					// Only add if double or int
					DoubleAttribute::Ptr doubleAttribute = std::dynamic_pointer_cast<DoubleAttribute>(attribute);
					if (doubleAttribute != nullptr)
					{
						return doubleAttribute->DoubleValue();
					}

					IntAttribute::Ptr intAttribute = std::dynamic_pointer_cast<IntAttribute>(attribute);
					if (intAttribute != nullptr)
					{
						return (double) intAttribute->IntValue();
					}

					return 1.0;
				}
			}
		}
	}
	TopoDS_Edge Graph::FindEdge(const TopoDS_Vertex & rkVertex1, const TopoDS_Vertex & rkVertex2, const double kTolerance) const
	{
		for (TopTools_MapIteratorOfMapOfShape occtEdgeIterator(m_occtEdges);
			occtEdgeIterator.More();
			occtEdgeIterator.Next())
		{
			TopoDS_Edge occtEdge = TopoDS::Edge(occtEdgeIterator.Value());
			ShapeAnalysis_Edge occtShapeAnalysisEdge;
			TopoDS_Vertex occtFirstVertex = occtShapeAnalysisEdge.FirstVertex(occtEdge);
			TopoDS_Vertex occtLastVertex = occtShapeAnalysisEdge.LastVertex(occtEdge);

			if ((IsCoincident(occtFirstVertex, rkVertex1, kTolerance) && IsCoincident(occtLastVertex, rkVertex2, kTolerance)) ||
				(IsCoincident(occtFirstVertex, rkVertex2, kTolerance) && IsCoincident(occtLastVertex, rkVertex1, kTolerance)))
			{
				return occtEdge;
			}
		}

		return TopoDS_Edge();
	}

	bool Graph::IsCoincident(const TopoDS_Vertex & rkVertex1, const TopoDS_Vertex & rkVertex2, const double kTolerance)
	{
		//BRepExtrema_DistShapeShape occtDistanceCalculation(rkVertex1, rkVertex2);
		//bool isDone = occtDistanceCalculation.Perform();
		//if (isDone)


        Handle(Geom_CartesianPoint) point1 = new Geom_CartesianPoint(BRep_Tool::Pnt(rkVertex1));
        Handle(Geom_CartesianPoint) point2 = new Geom_CartesianPoint(BRep_Tool::Pnt(rkVertex2));

        double dx = (point2->X() - point1->X());
        double dy = (point2->Y() - point1->Y());
        double dz = (point2->Z() - point1->Z());
        double sqDistance = dx * dx + dy * dy + dz * dz;
        if (sqDistance < kTolerance)
		{
			/*double distance = occtDistanceCalculation.Value();
			if (distance < kTolerance)
			{*/
				return true;
			//}
		}

		return false;
	}
}