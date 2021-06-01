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

#include "Cell.h"
#include "Vertex.h"
#include "Edge.h"
#include "Wire.h"
#include "Face.h"
#include "Shell.h"
#include "CellComplex.h"
#include "CellFactory.h"
#include "GlobalCluster.h"
#include "AttributeManager.h"

#include <BOPAlgo_MakerVolume.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepClass3d.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <Message_ProgressIndicator.hxx>
#include <ShapeFix_Solid.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS.hxx>

#include <assert.h>

namespace TopologicCore
{
	void Cell::AdjacentCells(std::list<Cell::Ptr>& rAdjacentCells) const
	{
		// Get a map of Face->Solid[]
		TopTools_IndexedDataMapOfShapeListOfShape occtFaceSolidMap;
		TopExp::MapShapesAndUniqueAncestors(GlobalCluster::GetInstance().GetOcctCompound(), TopAbs_FACE, TopAbs_SOLID, occtFaceSolidMap);

		// Find the constituent Faces
		TopTools_MapOfShape occtFaces;
		DownwardNavigation(GetOcctShape(), TopAbs_FACE, occtFaces);

		// Iterate through the Faces and find the incident cells which are not this cell.
		const TopoDS_Solid& rkOcctSolid = GetOcctSolid();
		TopTools_MapOfShape occtAdjacentSolids;
		for (TopTools_MapOfShape::const_iterator kOcctFaceIterator = occtFaces.cbegin();
			kOcctFaceIterator != occtFaces.cend();
			kOcctFaceIterator++)
		{
			const TopoDS_Shape& rkOcctFace = *kOcctFaceIterator;
			try {
				const TopTools_ListOfShape& rkIncidentCells = occtFaceSolidMap.FindFromKey(rkOcctFace);

				for (TopTools_ListOfShape::const_iterator kOcctCellIterator = rkIncidentCells.cbegin();
					kOcctCellIterator != rkIncidentCells.cend();
					kOcctCellIterator++)
				{
					const TopoDS_Shape& rkIncidentCell = *kOcctCellIterator;
					if (!rkOcctSolid.IsSame(rkIncidentCell))
					{
						occtAdjacentSolids.Add(rkIncidentCell);
					}
				}
			}
			catch (Standard_NoSuchObject)
			{
				assert("Cannot find a Face in the global Cluster.");
				throw std::runtime_error("Cannot find a Face in the global Cluster.");
			}
		}

		// Output the adjacent Cells
		for (TopTools_MapIteratorOfMapOfShape occtAdjacentSolidIterator(occtAdjacentSolids); 
			occtAdjacentSolidIterator.More();
			occtAdjacentSolidIterator.Next())
		{
			rAdjacentCells.push_back(std::make_shared<Cell>(TopoDS::Solid(occtAdjacentSolidIterator.Value())));
		}
	}

	void Cell::CellComplexes(std::list<std::shared_ptr<TopologicCore::CellComplex>>& rCellComplexes) const
	{
		UpwardNavigation(rCellComplexes);
	}

	void Cell::Shells(std::list<Shell::Ptr>& rShells) const
	{
		DownwardNavigation(rShells);
	}

	void Cell::Edges(std::list<Edge::Ptr>& rEdges) const
	{
		DownwardNavigation(rEdges);
	}

	void Cell::Faces(std::list<Face::Ptr>& rFaces) const
	{
		DownwardNavigation(rFaces);
	}

	void Cell::Vertices(std::list<Vertex::Ptr>& rVertices) const
	{
		DownwardNavigation(rVertices);
	}

	void Cell::Wires(std::list<Wire::Ptr>& rWires) const
	{
		DownwardNavigation(rWires);
	}

	Vertex::Ptr Cell::CenterOfMass() const
	{
		TopoDS_Vertex occtCenterOfMass = CenterOfMass(GetOcctSolid());
		return std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtCenterOfMass));
	}

	TopoDS_Vertex Cell::CenterOfMass(const TopoDS_Solid & rkOcctSolid)
	{
		GProp_GProps occtShapeProperties;
		BRepGProp::VolumeProperties(rkOcctSolid, occtShapeProperties);
		return BRepBuilderAPI_MakeVertex(occtShapeProperties.CentreOfMass());
	}

	Cell::Ptr Cell::ByFaces(const std::list<Face::Ptr>& rkFaces, double kTolerance)
	{
		if (kTolerance <= 0.0)
		{
			throw std::runtime_error("The tolerance must have a positive value.");
		}

		if (rkFaces.empty())
		{
			throw std::runtime_error("The input Face list is empty.");
		}

		BOPAlgo_MakerVolume occtMakerVolume;
		TopTools_ListOfShape occtShapes;
		for (const Face::Ptr& kpFace : rkFaces)
		{
			occtShapes.Append(kpFace->GetOcctShape());
		}
		bool isParallel = false;
		bool doesIntersection = true;

		occtMakerVolume.SetArguments(occtShapes);
		occtMakerVolume.SetRunParallel(isParallel);
		occtMakerVolume.SetIntersect(doesIntersection);
		occtMakerVolume.SetFuzzyValue(kTolerance);

		occtMakerVolume.Perform();
		if (occtMakerVolume.HasErrors()) {
			throw std::runtime_error("The input Faces do not form a Cell.");
		}

		const TopoDS_Shape& rkOcctResult = occtMakerVolume.Shape();

		std::list<Cell::Ptr> cells;
		// The result is either:
		// - A solid
		TopoDS_Solid occtSolid;
		if (rkOcctResult.ShapeType() == TopAbs_SOLID)
		{
			// Return the Cell
			occtSolid = TopoDS::Solid(rkOcctResult);
		}
		// - A compound, collect the solids
		else if (rkOcctResult.ShapeType() == TopAbs_COMPOUND)
		{
			std::string txt = Topology::Analyze(rkOcctResult);
			// Must only have 1 shape
			TopoDS_Shape occtShape;
			for (TopExp_Explorer occtExplorer(rkOcctResult, TopAbs_SOLID); occtExplorer.More(); occtExplorer.Next())
			{
				const TopoDS_Shape& occtCurrent = occtExplorer.Current();
				if (occtShape.IsNull())
				{
					occtShape = TopoDS::Solid(occtCurrent);
				}
				else
				{
					throw std::runtime_error("The input Faces do not form a Cell.");
				}
			}

			if (occtShape.IsNull())
			{
				throw std::runtime_error("The input Faces do not form a Cell.");
			}
			else
			{
				occtSolid = TopoDS::Solid(occtShape);
			}
		}

		// Shape fix the solid
		TopoDS_Solid occtFixedSolid = OcctShapeFix(occtSolid);
		Cell::Ptr fixedCell = std::make_shared<Cell>(occtFixedSolid);

		// Deep copy the Cell
		Cell::Ptr copyFixedCell = TopologicalQuery::Downcast<Cell>(fixedCell->DeepCopy());

		// Register to Global Cluster
		GlobalCluster::GetInstance().AddTopology(fixedCell->GetOcctSolid());

		// Copy the Dictionaries
		std::list<Topology::Ptr> facesAsTopologies;
		for (const Face::Ptr& kpFace : rkFaces)
		{
			facesAsTopologies.push_back(kpFace);
			//AttributeManager::GetInstance().DeepCopyAttributes(kpFace->GetOcctFace(), copyFixedCell->GetOcctSolid());
		}
		copyFixedCell->DeepCopyAttributesFrom(facesAsTopologies);

		return copyFixedCell;
	}

	Cell::Ptr Cell::ByShell(const Shell::Ptr& kpShell)
	{
		if (!kpShell->IsClosed())
		{
			throw std::runtime_error("The input Shell is open.");
		}

		BRepBuilderAPI_MakeSolid occtMakeSolid;
		try {
			occtMakeSolid = BRepBuilderAPI_MakeSolid(kpShell->GetOcctShell());
		}
		catch (StdFail_NotDone&)
		{
			throw std::runtime_error("The input Shell does not form a valid Cell.");
		}

		// Create a cell from the shell. The faces are the same and the contents
		// are automatically passed.

		// Shape fix the solid
		TopoDS_Solid occtFixedSolid = OcctShapeFix(occtMakeSolid);
		Cell::Ptr fixedCell = std::make_shared<Cell>(occtFixedSolid);

		// Deep copy the Cell
		Cell::Ptr copyFixedCell = TopologicalQuery::Downcast<Cell>(fixedCell->DeepCopy());

		// Register to Global Cluster
		GlobalCluster::GetInstance().AddTopology(fixedCell->GetOcctSolid());

		// Copy the Dictionaries
		AttributeManager::GetInstance().DeepCopyAttributes(kpShell->GetOcctShell(), copyFixedCell->GetOcctSolid());

		return copyFixedCell;
	}

	void Cell::SharedEdges(const Cell::Ptr& kpAnotherCell, std::list<Edge::Ptr>& rEdges) const
	{
		const TopoDS_Shape& rkOcctShape1 = GetOcctShape();
		TopTools_MapOfShape occtEdges1;
		DownwardNavigation(rkOcctShape1, TopAbs_EDGE, occtEdges1);

		const TopoDS_Shape& rkOcctShape2 = kpAnotherCell->GetOcctShape();
		TopTools_MapOfShape occtEdges2;
		DownwardNavigation(rkOcctShape2, TopAbs_EDGE, occtEdges2);

		for (TopTools_MapIteratorOfMapOfShape occtEdgeIterator1(occtEdges1);
			occtEdgeIterator1.More();
			occtEdgeIterator1.Next())
		{
			for (TopTools_MapIteratorOfMapOfShape occtEdgeIterator2(occtEdges2);
				occtEdgeIterator2.More();
				occtEdgeIterator2.Next())
			{
				if (occtEdgeIterator1.Value().IsSame(occtEdgeIterator2.Value()))
				{
					Edge::Ptr pEdge = std::make_shared<Edge>(TopoDS::Edge(occtEdgeIterator1.Value()));
					rEdges.push_back(pEdge);
				}
			}
		}
	}

	void Cell::SharedFaces(const Cell::Ptr& kpAnotherCell, std::list<Face::Ptr>& rFaces) const
	{
		// Get all Faces of this Cell
		const TopoDS_Shape& rkOcctShape1 = GetOcctShape();
		TopTools_MapOfShape occtFaces1;
		DownwardNavigation(rkOcctShape1, TopAbs_FACE, occtFaces1);

		// Get all Faces of the other Cell
		const TopoDS_Shape& rkOcctShape2 = kpAnotherCell->GetOcctShape();
		TopTools_MapOfShape occtFaces2;
		DownwardNavigation(rkOcctShape2, TopAbs_FACE, occtFaces2);

		// Iterate through both lists of Faces, and identify shared Faces
		for (TopTools_MapIteratorOfMapOfShape occtFaceIterator1(occtFaces1);
			occtFaceIterator1.More();
			occtFaceIterator1.Next())
		{
			for (TopTools_MapIteratorOfMapOfShape occtFaceIterator2(occtFaces2);
				occtFaceIterator2.More();
				occtFaceIterator2.Next())
			{
				if (occtFaceIterator1.Value().IsSame(occtFaceIterator2.Value()))
				{
					Face::Ptr pFace = std::make_shared<Face>(TopoDS::Face(occtFaceIterator1.Value()));
					rFaces.push_back(pFace);
				}
			}
		}
	}

	void Cell::SharedVertices(const Cell::Ptr& kpAnotherCell, std::list<Vertex::Ptr>& rVertices) const
	{
		// Get all Vertices of this Cell
		const TopoDS_Shape& rkOcctShape1 = GetOcctShape();
		TopTools_MapOfShape occtVertices1;
		DownwardNavigation(rkOcctShape1, TopAbs_VERTEX, occtVertices1);

		// Get all Vertices of the other Cell
		const TopoDS_Shape& rkOcctShape2 = kpAnotherCell->GetOcctShape();
		TopTools_MapOfShape occtVertices2;
		DownwardNavigation(rkOcctShape2, TopAbs_VERTEX, occtVertices2);

		// Iterate through both lists of Vertices, and identify shared Vertices
		for (TopTools_MapIteratorOfMapOfShape occtVertexIterator1(occtVertices1);
			occtVertexIterator1.More();
			occtVertexIterator1.Next())
		{
			for (TopTools_MapIteratorOfMapOfShape occtVertexIterator2(occtVertices2);
				occtVertexIterator2.More();
				occtVertexIterator2.Next())
			{
				if (occtVertexIterator1.Value().IsSame(occtVertexIterator2.Value()))
				{
					Vertex::Ptr pVertex = std::make_shared<Vertex>(TopoDS::Vertex(occtVertexIterator1.Value()));
					rVertices.push_back(pVertex);
				}
			}
		}
	}

	Shell::Ptr Cell::ExternalBoundary() const
	{
		TopoDS_Shell occtOuterShell = BRepClass3d::OuterShell(TopoDS::Solid(GetOcctShape()));
		return std::make_shared<Shell>(occtOuterShell);
	}

	void Cell::InternalBoundaries(std::list<Shell::Ptr>& rShells) const
	{
		// Identify the external boundary
		Shell::Ptr pExternalBoundary = ExternalBoundary();

		// Identify the boundaries which are not external boundary
		std::list<Shell::Ptr> shells;
		DownwardNavigation(shells);
		for (const Shell::Ptr& kpShell : shells)
		{
			if (!kpShell->IsSame(pExternalBoundary))
			{
				rShells.push_back(kpShell);
			}
		}
	}

	bool Cell::IsManifold() const
	{
		// TODO: check for internal Faces
		// NOTE: it is not enough to check Edges bordering > 3 Faces
		throw std::runtime_error("Not implemented yet");
	}

	TopoDS_Shape& Cell::GetOcctShape()
	{
		return GetOcctSolid();
	}

	const TopoDS_Shape& Cell::GetOcctShape() const
	{
		return GetOcctSolid();
	}

	TopoDS_Solid& Cell::GetOcctSolid()
	{
		assert(!m_occtSolid.IsNull() && "Cell::m_occtSolid is null.");
		if (m_occtSolid.IsNull())
		{
			throw std::runtime_error("A null Cell is encountered.");
		}

		return m_occtSolid;
	}

	const TopoDS_Solid& Cell::GetOcctSolid() const
	{
		assert(!m_occtSolid.IsNull() && "Cell::m_occtSolid is null.");
		if (m_occtSolid.IsNull())
		{
			throw std::runtime_error("A null Cell is encountered.");
		}

		return m_occtSolid;
	}

	void Cell::SetOcctShape(const TopoDS_Shape & rkOcctShape)
	{
		SetOcctSolid(TopoDS::Solid(rkOcctShape));
	}

	void Cell::SetOcctSolid(const TopoDS_Solid & rkOcctSolid)
	{
		m_occtSolid = rkOcctSolid;
	}

	void Cell::Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const
	{
		// Returns a list of faces
		std::list<Face::Ptr> faces;
		Faces(faces);
		for (const Face::Ptr& kpFace : faces)
		{
			rOcctGeometries.push_back(kpFace->Surface());
		}
	}

	std::string Cell::GetTypeAsString() const
	{
		return std::string("Cell");
	}

	Cell::Cell(const TopoDS_Solid& rkOcctSolid, const std::string& rkGuid)
		: Topology(3, rkOcctSolid, rkGuid.compare("") == 0 ? GetClassGUID() : rkGuid)
		, m_occtSolid(rkOcctSolid)
	{
		RegisterFactory(GetClassGUID(), std::make_shared<CellFactory>());
	}

	Cell::~Cell()
	{

	}

	TopoDS_Solid Cell::OcctShapeFix(const TopoDS_Solid & rkOcctInputSolid)
	{
		ShapeFix_Solid occtSolidFix(rkOcctInputSolid);
		occtSolidFix.Perform();
		return TopoDS::Solid(occtSolidFix.Solid());
	}
}
