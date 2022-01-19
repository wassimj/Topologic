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

#include "Vertex.h"
#include "Edge.h"
#include "Wire.h"
#include "Face.h"
#include "Shell.h"
#include "Cell.h"
#include "CellComplex.h"
#include "Cluster.h"
#include "VertexFactory.h"
#include "GlobalCluster.h"

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Tool.hxx>
#include <Geom_CartesianPoint.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

#include <assert.h>

namespace TopologicCore
{
	Vertex::Vertex(const TopoDS_Vertex& rkOcctVertex, const std::string& rkGuid)
		: Topology(0, rkOcctVertex, rkGuid.compare("") == 0 ? GetClassGUID() : rkGuid)
		, m_occtVertex(rkOcctVertex)
	{
		RegisterFactory(GetClassGUID(), std::make_shared<VertexFactory>());
	}

	Vertex::~Vertex()
	{

	}

	Vertex::Ptr Vertex::ByPoint(Handle(Geom_Point) pOcctPoint)
	{
		TopoDS_Vertex occtVertex = BRepBuilderAPI_MakeVertex(pOcctPoint->Pnt());
		TopoDS_Vertex occtFixedVertex = TopoDS::Vertex(Topology::FixShape(occtVertex));
		Vertex::Ptr pVertex = std::make_shared<Vertex>(occtFixedVertex);
		//GlobalCluster::GetInstance().AddTopology(pVertex->GetOcctVertex());
		return pVertex;
	}

	Vertex::Ptr Vertex::ByCoordinates(const double kX, const double kY, const double kZ)
	{
		TopoDS_Vertex occtVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(kX, kY, kZ));
		TopoDS_Vertex occtFixedVertex = TopoDS::Vertex(Topology::FixShape(occtVertex));
		Vertex::Ptr pVertex = std::make_shared<Vertex>(occtFixedVertex);
		//GlobalCluster::GetInstance().AddTopology(pVertex->GetOcctVertex());
		return pVertex;
	}

	void Vertex::Edges(const Topology::Ptr& kpHostTopology, std::list<Edge::Ptr>& rEdges) const
	{
		if(kpHostTopology)
		{
			UpwardNavigation(kpHostTopology->GetOcctShape(), rEdges);
		}
		else
		{
			throw std::runtime_error("Host Topology cannot be NULL when searching for ancestors.");
		}
	}

	bool Vertex::IsManifold(const Topology::Ptr& kpHostTopology) const
	{
		// For now, in the context of an Edge, a Vertex is considered always manifold.
		// TODO: Vertex is considered manifold if it is the start or end vertex.
		if (kpHostTopology->GetType() == TOPOLOGY_EDGE)
		{
			return true;
		}
		// In the context of a Wire, a Vertex is non-manifold if it connects more than two edges.
		if (kpHostTopology->GetType() == TOPOLOGY_WIRE)
		{
			std::list<Edge::Ptr> edges;
			Edges(kpHostTopology, edges);
			if (edges.size() > 2)
			{
				return false;
			}
		}
		// In the context of a Face, a Vertex is non-manifold if it connects more than two edges.
		if (kpHostTopology->GetType() == TOPOLOGY_FACE)
		{
			std::list<Edge::Ptr> edges;
			Edges(kpHostTopology, edges);
			if (edges.size() > 2)
			{
				return false;
			}
		}
		// In the context of a Shell, a Vertex is non-manifold if it connects more than two Faces.
		if (kpHostTopology->GetType() == TOPOLOGY_SHELL)
		{
			std::list<Face::Ptr> faces;
			Faces(kpHostTopology, faces);
			if (faces.size() > 2)
			{
				return false;
			}
		}
		// In the context of a Cell, a Vertex is non-manifold if it connects more than two Faces.
		if (kpHostTopology->GetType() == TOPOLOGY_CELL)
		{
			std::list<Face::Ptr> faces;
			Faces(kpHostTopology, faces);
			if (faces.size() > 2)
			{
				return false;
			}
		}
		// In the context of a CellComplex, a Vertex is non-manifold if it connects more than one Cell.
		if (kpHostTopology->GetType() == TOPOLOGY_CELLCOMPLEX)
		{
			std::list<Cell::Ptr> cells;
			Cells(kpHostTopology, cells);
			if (cells.size() > 1)
			{
				return false;
			}
		}
		// In the context of a Cluster, Check all the SubTopologies
		if (kpHostTopology->GetType() == TOPOLOGY_CLUSTER)
		{
			std::list<TopologicCore::CellComplex::Ptr> cellComplexes;
			kpHostTopology->CellComplexes(nullptr, cellComplexes);
			for (const CellComplex::Ptr kpCellComplex : cellComplexes)
			{
				if (IsManifold(kpCellComplex))
				{
					return false;
				}
			}
			std::list<TopologicCore::Cell::Ptr> cells;
			for (const Cell::Ptr kpCell : cells)
			{
				if (IsManifold(kpCell))
				{
					return false;
				}
			}
			std::list<TopologicCore::Shell::Ptr> shells;
			for (const Shell::Ptr kpShell : shells)
			{
				if (IsManifold(kpShell))
				{
					return false;
				}
			}
			std::list<TopologicCore::Face::Ptr> faces;
			for (const Face::Ptr kpFace : faces)
			{
				if (IsManifold(kpFace))
				{
					return false;
				}
			}
			std::list<TopologicCore::Wire::Ptr> wires;
			for (const Wire::Ptr kpWire : wires)
			{
				if (IsManifold(kpWire))
				{
					return false;
				}
			}
		}
		return true;
	}
	void Vertex::AdjacentVertices(const TopologicCore::Topology::Ptr& kpHostTopology, std::list<Vertex::Ptr>& rAdjacentVertices) const
	{
		// Find the constituent edges
		TopTools_MapOfShape occtAdjacentVertices;
		// Find the edges
		std::list<Topology::Ptr> edges;
		if (kpHostTopology)
		{
			UpwardNavigation(kpHostTopology->GetOcctShape(), TOPOLOGY_EDGE, edges);
		}
		else
		{
			throw std::runtime_error("Host Topology cannot be NULL when searching for ancestors.");
		}
		for (const Topology::Ptr& kpEdge : edges)
		{
			std::list<Vertex::Ptr> vertices;
			kpEdge->Vertices(nullptr, vertices);
			for (const Topology::Ptr& kpVertex : vertices)
			{
				// If not the same as this Vertex, add to the list
				if (!IsSame(kpVertex))
				{
					occtAdjacentVertices.Add(kpVertex->GetOcctShape());
				}
			}
		}

		for (TopTools_MapIteratorOfMapOfShape occtAdjacentVertexIterator(occtAdjacentVertices);
			occtAdjacentVertexIterator.More();
			occtAdjacentVertexIterator.Next())
		{
			rAdjacentVertices.push_back(std::make_shared<Vertex>(TopoDS::Vertex(occtAdjacentVertexIterator.Value())));
		}
	}

	void Vertex::Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const
	{
		rOcctGeometries.push_back(Point());
	}

	TopoDS_Shape& Vertex::GetOcctShape()
	{
		return GetOcctVertex();
	}

	void Vertex::SetOcctShape(const TopoDS_Shape & rkOcctShape)
	{
		try {
			SetOcctVertex(TopoDS::Vertex(rkOcctShape));
		}
		catch (Standard_Failure e)
		{
			throw std::runtime_error(e.GetMessageString());
		}
	}

	const TopoDS_Shape& Vertex::GetOcctShape() const
	{
		return GetOcctVertex();
	}

	TopoDS_Vertex& Vertex::GetOcctVertex()
	{
		assert(!m_occtVertex.IsNull() && "Vertex::m_occtVertex is null.");
		if (m_occtVertex.IsNull())
		{
			throw std::runtime_error("A null Vertex is encountered.");
		}

		return m_occtVertex;
	}

	void Vertex::SetOcctVertex(const TopoDS_Vertex & rkOcctVertex)
	{
		m_occtVertex = rkOcctVertex;
	}

	const TopoDS_Vertex& Vertex::GetOcctVertex() const
	{
		assert(!m_occtVertex.IsNull() && "Vertex::m_occtVertex is null.");
		if (m_occtVertex.IsNull())
		{
			throw std::runtime_error("A null Vertex is encountered.");
		}

		return m_occtVertex;
	}

	Handle(Geom_Point) Vertex::Point() const
	{
		return new Geom_CartesianPoint(BRep_Tool::Pnt(GetOcctVertex()));
	}

	double Vertex::X() const
	{
		Handle(Geom_Point) pOcctPoint = Point();
		return pOcctPoint->X();
	}

	double Vertex::Y() const
	{
		Handle(Geom_Point) pOcctPoint = Point();
		return pOcctPoint->Y();
	}

	double Vertex::Z() const
	{
		Handle(Geom_Point) pOcctPoint = Point();
		return pOcctPoint->Z();
	}

	std::tuple<double, double, double> Vertex::Coordinates() const
	{
		Handle(Geom_Point) pOcctPoint = Point();
		return std::tuple<double, double, double>(pOcctPoint->X(), pOcctPoint->Y(), pOcctPoint->Z());
	}

	std::shared_ptr<Vertex> Vertex::CenterOfMass() const
	{
		TopoDS_Vertex occtCenterOfMass = CenterOfMass(GetOcctVertex());
		return std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtCenterOfMass));
	}
	TopoDS_Vertex Vertex::CenterOfMass(const TopoDS_Vertex & rkOcctVertex)
	{
		TopoDS_Vertex occtCenterOfMass = BRepBuilderAPI_MakeVertex(BRep_Tool::Pnt(rkOcctVertex));
		TopoDS_Vertex occtFixedCenterOfMass = TopoDS::Vertex(Topology::FixShape(occtCenterOfMass));
		return occtFixedCenterOfMass;
	}
	std::string Vertex::GetTypeAsString() const
	{
		return std::string("Vertex");
	}
}
