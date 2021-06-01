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

#include "Shell.h"
#include "Cell.h"
#include "Vertex.h"
#include "Edge.h"
#include "Wire.h"
#include "Face.h"
#include "Aperture.h"
#include "ShellFactory.h"
#include "GlobalCluster.h"
#include "AttributeManager.h"

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeShell.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepGProp.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Surface.hxx>
#include <GProp_GProps.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <ShapeFix_Shell.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_MapOfShape.hxx>

#include <array>
#include <assert.h>

namespace TopologicCore
{
	void Shell::Cells(std::list<Cell::Ptr>& rCells) const
	{
		UpwardNavigation(rCells);
	}

	void Shell::Edges(std::list<Edge::Ptr>& rEdges) const
	{
		DownwardNavigation(rEdges);
	}

	void Shell::Wires(std::list<Wire::Ptr>& rWires) const
	{
		DownwardNavigation(rWires);
	}

	void Shell::Faces(std::list<Face::Ptr>& rFaces) const
	{
		DownwardNavigation(rFaces);
	}

	bool Shell::IsClosed() const
	{
		BRepCheck_Shell occtBrepCheckShell(TopoDS::Shell(GetOcctShape()));
		return (occtBrepCheckShell.Closed() == BRepCheck_NoError);
	}

	void Shell::Vertices(std::list<Vertex::Ptr>& rVertices) const
	{
		DownwardNavigation(rVertices);
	}

	Shell::Ptr Shell::ByFaces(const std::list<Face::Ptr>& rkFaces, const double kTolerance)
	{
		if (rkFaces.empty())
		{
			throw std::runtime_error("No face is passed.");
		}

		TopTools_ListOfShape occtShapes;
		for(const Face::Ptr& kpFace : rkFaces)
		{
			occtShapes.Append(kpFace->GetOcctShape());
		}

		if (occtShapes.Size() == 1)
		{
			TopoDS_Shell occtShell; // no need to copy, as this is a new instance
			TopoDS_Builder occtBuilder;
			occtBuilder.MakeShell(occtShell);
			for (TopTools_ListIteratorOfListOfShape occtShapeIterator(occtShapes); occtShapeIterator.More(); occtShapeIterator.Next())
			{
				occtBuilder.Add(occtShell, TopoDS::Face(occtShapeIterator.Value()));
				AttributeManager::GetInstance().DeepCopyAttributes(occtShapeIterator.Value(), occtShell);
			}
			Shell::Ptr pShell = std::make_shared<Shell>(occtShell);
			GlobalCluster::GetInstance().AddTopology(pShell);
			return pShell;
		}

		TopoDS_Shape occtShape = OcctSewFaces(occtShapes, kTolerance);
		try{
			TopoDS_Shell occtShell = TopoDS::Shell(occtShape);
			Shell::Ptr pShell = std::make_shared<Shell>(occtShell);
			//Shell::Ptr pCopyShell = std::dynamic_pointer_cast<Shell>(pShell->DeepCopy());
			std::list<Topology::Ptr> facesAsTopologies;
			for (const Face::Ptr& kpFace: rkFaces)
			{
				facesAsTopologies.push_back(kpFace);
				//AttributeManager::GetInstance().DeepCopyAttributes(kpFace->GetOcctFace(), pCopyShell->GetOcctShell());
			}
			Shell::Ptr pCopyShell = std::dynamic_pointer_cast<Shell>(pShell->DeepCopyAttributesFrom(facesAsTopologies));

			GlobalCluster::GetInstance().AddTopology(pCopyShell);
			return pCopyShell;
		}
		catch (Standard_TypeMismatch)
		{
			throw std::runtime_error("The set of faces does not create a valid shell.");
		}
	}

	bool Shell::IsManifold() const
	{
		throw std::runtime_error("Not implemented yet");
	}

	TopoDS_Shape& Shell::GetOcctShape()
	{
		return GetOcctShell();
	}

	const TopoDS_Shape& Shell::GetOcctShape() const
	{
		return GetOcctShell();
	}

	TopoDS_Shell& Shell::GetOcctShell()
	{
		assert(!m_occtShell.IsNull() && "Shell::m_occtShell is null.");
		if (m_occtShell.IsNull())
		{
			throw std::runtime_error("A null Shell is encountered.");
		}

		return m_occtShell;
	}

	const TopoDS_Shell& Shell::GetOcctShell() const
	{
		assert(!m_occtShell.IsNull() && "Shell::m_occtShell is null.");
		if (m_occtShell.IsNull())
		{
			throw std::runtime_error("A null Shell is encountered.");
		}

		return m_occtShell;
	}

	void Shell::SetOcctShape(const TopoDS_Shape & rkOcctShape)
	{
		SetOcctShell(TopoDS::Shell(rkOcctShape));
	}

	void Shell::SetOcctShell(const TopoDS_Shell & rkOcctShell)
	{
		m_occtShell = rkOcctShell;
	}

	Vertex::Ptr Shell::CenterOfMass() const
	{
		TopoDS_Vertex occtCenterOfMass = CenterOfMass(GetOcctShell());
		return std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtCenterOfMass));
	}

	TopoDS_Vertex Shell::CenterOfMass(const TopoDS_Shell & rkOcctShell)
	{
		GProp_GProps occtShapeProperties;
		BRepGProp::SurfaceProperties(rkOcctShell, occtShapeProperties);
		return BRepBuilderAPI_MakeVertex(occtShapeProperties.CentreOfMass());
	}

	std::string Shell::GetTypeAsString() const
	{
		return std::string("Shell");
	}

	void Shell::Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const
	{
		// Returns a list of faces
		std::list<Face::Ptr> faces;
		Faces(faces);
		for (const Face::Ptr& kpFace : faces)
		{
			rOcctGeometries.push_back(kpFace->Surface());
		}
	}

	Shell::Shell(const TopoDS_Shell& rkOcctShell, const std::string& rkGuid)
		: Topology(2, rkOcctShell, rkGuid.compare("") == 0 ? GetClassGUID() : rkGuid)
		, m_occtShell(rkOcctShell)
	{
		RegisterFactory(GetClassGUID(), std::make_shared<ShellFactory>());
	}

	Shell::~Shell()
	{

	}
}
