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

#include <Utilities/VertexUtility.h>
#include <Utilities/CellUtility.h>
#include <Utilities/FaceUtility.h>

#include <Aperture.h>
#include <Cell.h>
#include <CellComplex.h>
#include <Cluster.h>
#include <Edge.h>
#include <Face.h>
#include <Vertex.h>
#include <Shell.h>
#include <Wire.h>

#include <BRepExtrema_DistShapeShape.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <Geom_Point.hxx>

#include <algorithm>

namespace TopologicUtilities
{
	void VertexUtility::AdjacentEdges(
		const TopologicCore::Vertex::Ptr & kpVertex, 
		const TopologicCore::Topology::Ptr & kpParentTopology,
		std::list<TopologicCore::Edge::Ptr>& rCoreAdjacentEdges)
	{
		AdjacentEdges(kpVertex, kpParentTopology.get(), rCoreAdjacentEdges);
	}

	void VertexUtility::AdjacentEdges(const TopologicCore::Vertex::Ptr & kpVertex, TopologicCore::Topology const * const kpkParentTopology, std::list<TopologicCore::Edge::Ptr>& rCoreAdjacentEdges)
	{
		std::list<TopologicCore::Topology::Ptr> coreAdjacentTopologies;
		kpVertex->UpwardNavigation(kpkParentTopology->GetOcctShape(), TopologicCore::Edge::Type(), coreAdjacentTopologies);
		for (const TopologicCore::Topology::Ptr& kpAdjacentTopology : coreAdjacentTopologies)
		{
			rCoreAdjacentEdges.push_back(
				TopologicCore::TopologicalQuery::Downcast<TopologicCore::Edge>(kpAdjacentTopology)
			);
		}
	}

	double DistanceVertexVertex(
		const TopologicCore::Vertex::Ptr& kpVertex1,
		const TopologicCore::Vertex::Ptr& kpVertex2)
	{
		BRepExtrema_DistShapeShape occtDistance(kpVertex1->GetOcctShape(), kpVertex2->GetOcctShape(), Extrema_ExtFlag_MINMAX);
		return occtDistance.Value();
	}

	double DistanceVertexEdge(
		const TopologicCore::Vertex::Ptr& kpVertex,
		const TopologicCore::Edge::Ptr& kpEdge)
	{
		BRepExtrema_DistShapeShape occtDistance(kpVertex->GetOcctShape(), kpEdge->GetOcctShape(), Extrema_ExtFlag_MINMAX);
		return occtDistance.Value();
	}

	double DistanceVertexWire(
		const TopologicCore::Vertex::Ptr& kpVertex,
		const TopologicCore::Wire::Ptr& kpWire)
	{
		double minDistance = std::numeric_limits<double>::max();
		std::list<TopologicCore::Edge::Ptr> edges;
		kpWire->Edges(edges);

		for (const TopologicCore::Edge::Ptr& kpEdge : edges)
		{
			double distance = DistanceVertexEdge(kpVertex, kpEdge);
			if (distance < minDistance)
			{
				minDistance = distance;
			}
		}

		return minDistance;
	}

	double DistanceVertexFace(
		const TopologicCore::Vertex::Ptr& kpVertex,
		const TopologicCore::Face::Ptr& kpFace)
	{
		GeomAPI_ProjectPointOnSurf occtProjection(
			kpVertex->Point()->Pnt(),
			kpFace->Surface()
		);

		if (!occtProjection.IsDone())
		{
			return std::numeric_limits<double>::max();
		}

		try {
			// If the projection is outside the Face (use IsInside), use distance to edges.
			bool isInside = FaceUtility::IsInside(kpFace, kpVertex, 0.0001);

			if (isInside)
			{
				double distance = occtProjection.LowerDistance();
				return distance;
			}
			else
			{
				BRepExtrema_DistShapeShape occtDistance(kpVertex->GetOcctShape(), kpFace->GetOcctShape(), Extrema_ExtFlag_MINMAX);
				return occtDistance.Value();
			}
		}
		catch (const std::runtime_error&)
		{
			return std::numeric_limits<double>::max();
		}
	}

	double DistanceVertexShell(
		const TopologicCore::Vertex::Ptr& kpVertex,
		const TopologicCore::Shell::Ptr& kpShell)
	{
		double minDistance = std::numeric_limits<double>::max();
		std::list<TopologicCore::Face::Ptr> faces;
		kpShell->Faces(faces);

		for (const TopologicCore::Face::Ptr& kpFace : faces)
		{
			double distance = DistanceVertexFace(kpVertex, kpFace);
			if (distance < minDistance)
			{
				minDistance = distance;
			}
		}

		return minDistance;
	}

	double DistanceVertexCell(
		const TopologicCore::Vertex::Ptr& kpVertex,
		const TopologicCore::Cell::Ptr& kpCell)
	{
		CellContainmentState isContained = CellUtility::Contains(kpCell, kpVertex, 0.0001);
		if (isContained == INSIDE || isContained == ON_BOUNDARY)
		{
			return 0.0;
		}

		double minDistance = std::numeric_limits<double>::max();
		std::list<TopologicCore::Face::Ptr> faces;
		kpCell->Faces(faces);

		for (const TopologicCore::Face::Ptr& kpFace : faces)
		{
			double distance = DistanceVertexFace(kpVertex, kpFace);
			if (distance < minDistance)
			{
				minDistance = distance;
			}
		}

		return minDistance;
	}

	double DistanceVertexCellComplex(
		const TopologicCore::Vertex::Ptr& kpVertex,
		const TopologicCore::CellComplex::Ptr& kpCellComplex)
	{
		double minDistance = std::numeric_limits<double>::max();
		std::list<TopologicCore::Cell::Ptr> cells;
		kpCellComplex->Cells(cells);

		for (const TopologicCore::Cell::Ptr& kpCell : cells)
		{
			double distance = DistanceVertexCell(kpVertex, kpCell);
			if (distance < minDistance)
			{
				minDistance = distance;
			}
		}

		return minDistance;
	}

	double DistanceVertexCluster(
		const TopologicCore::Vertex::Ptr& kpVertex,
		const TopologicCore::Cluster::Ptr& kpCluster)
	{
		double minDistance = std::numeric_limits<double>::max();
		std::list<TopologicCore::Topology::Ptr> topologies;
		kpCluster->SubTopologies(topologies);

		for (const TopologicCore::Topology::Ptr& kpTopology : topologies)
		{
			double distance = VertexUtility::Distance(kpVertex, kpTopology);
			if (distance < minDistance)
			{
				minDistance = distance;
			}
		}

		return minDistance;
	}

	double VertexUtility::Distance(
		const TopologicCore::Vertex::Ptr& kpVertex,
		const TopologicCore::Topology::Ptr& kpTopology)
	{
		TopologicCore::TopologyType topologyType = kpTopology->GetType();
		switch (topologyType)
		{
		case TopologicCore::TOPOLOGY_VERTEX:
			return DistanceVertexVertex(
				kpVertex,
				TopologicCore::TopologicalQuery::Downcast<TopologicCore::Vertex>(kpTopology)
			);
		case TopologicCore::TOPOLOGY_EDGE:
			return DistanceVertexEdge(
				kpVertex,
				TopologicCore::TopologicalQuery::Downcast<TopologicCore::Edge>(kpTopology)
			);
		case TopologicCore::TOPOLOGY_WIRE:
			return DistanceVertexWire(
				kpVertex,
				TopologicCore::TopologicalQuery::Downcast<TopologicCore::Wire>(kpTopology)
			);
		case TopologicCore::TOPOLOGY_FACE:
			return DistanceVertexFace(
				kpVertex,
				TopologicCore::TopologicalQuery::Downcast<TopologicCore::Face>(kpTopology)
			);
		case TopologicCore::TOPOLOGY_SHELL:
			return DistanceVertexShell(
				kpVertex,
				TopologicCore::TopologicalQuery::Downcast<TopologicCore::Shell>(kpTopology)
			);
		case TopologicCore::TOPOLOGY_CELL:
			return DistanceVertexCell(
				kpVertex,
				TopologicCore::TopologicalQuery::Downcast<TopologicCore::Cell>(kpTopology)
			);
		case TopologicCore::TOPOLOGY_CELLCOMPLEX:
			return DistanceVertexCellComplex(
				kpVertex,
				TopologicCore::TopologicalQuery::Downcast<TopologicCore::CellComplex>(kpTopology)
			);
		case TopologicCore::TOPOLOGY_CLUSTER:
			return DistanceVertexCluster(
				kpVertex,
				TopologicCore::TopologicalQuery::Downcast<TopologicCore::Cluster>(kpTopology)
			);
		case TopologicCore::TOPOLOGY_APERTURE:
			return Distance(
				kpVertex,
				TopologicCore::TopologicalQuery::Downcast<TopologicCore::Aperture>(kpTopology)->Topology()
			);

		default:
			throw std::runtime_error("An unknown Topology is detected.");
		}
	}
}