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
		GlobalCluster::GetInstance().AddTopology(pVertex->GetOcctVertex());
		return pVertex;
	}

	Vertex::Ptr Vertex::ByCoordinates(const double kX, const double kY, const double kZ)
	{
		TopoDS_Vertex occtVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(kX, kY, kZ));
		TopoDS_Vertex occtFixedVertex = TopoDS::Vertex(Topology::FixShape(occtVertex));
		Vertex::Ptr pVertex = std::make_shared<Vertex>(occtFixedVertex);
		GlobalCluster::GetInstance().AddTopology(pVertex->GetOcctVertex());
		return pVertex;
	}

	void Vertex::Edges(std::list<Edge::Ptr>& rEdges)
	{
		UpwardNavigation(rEdges);
	}

	bool Vertex::IsManifold() const
	{
		throw std::runtime_error("Not implemented yet");
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