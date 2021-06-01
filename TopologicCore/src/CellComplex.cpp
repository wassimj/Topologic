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

#include "CellComplex.h"
#include "Cluster.h"
#include "Face.h"
#include "Cell.h"
#include "Vertex.h"
#include "Edge.h"
#include "Wire.h"
#include "Shell.h"
#include "CellComplexFactory.h"
#include "GlobalCluster.h"
#include "AttributeManager.h"

#include <BOPAlgo_MakerVolume.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <IntTools_Context.hxx>
#include <ShapeAnalysis_ShapeContents.hxx>
#include <ShapeFix_Shape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_FrozenShape.hxx>
#include <TopoDS_UnCompatibleShapes.hxx>
#include <TopTools_MapOfShape.hxx>

#include <assert.h>

namespace TopologicCore
{
	void CellComplex::Cells(std::list<Cell::Ptr>& rCells) const
	{
		DownwardNavigation(rCells);
	}

	void CellComplex::Faces(std::list<Face::Ptr>& rFaces) const
	{
		DownwardNavigation(rFaces);
	}

	void CellComplex::Shells(std::list<Shell::Ptr>& rShells) const
	{
		DownwardNavigation(rShells);
	}

	void CellComplex::Edges(std::list<Edge::Ptr>& rEdges) const
	{
		DownwardNavigation(rEdges);
	}

	void CellComplex::Vertices(std::list<Vertex::Ptr>& rVertices) const
	{
		DownwardNavigation(rVertices);
	}

	void CellComplex::Wires(std::list<Wire::Ptr>& rWires) const
	{
		DownwardNavigation(rWires);
	}

	CellComplex::Ptr CellComplex::ByCells(const std::list<Cell::Ptr>& rkCells)
	{
		// ByOcctSolids does the actual construction. This method extracts the OCCT structures from the input
		// and wrap the output in a Topologic class.
		TopTools_ListOfShape occtShapes;
		for (const Cell::Ptr& kpCell : rkCells)
		{
			occtShapes.Append(kpCell->GetOcctShape());
		}

		TopoDS_CompSolid occtCompSolid = ByOcctSolids(occtShapes);
		CellComplex::Ptr pCellComplex = std::make_shared<CellComplex>(occtCompSolid);
		//CellComplex::Ptr pCopyCellComplex = std::dynamic_pointer_cast<CellComplex>(pCellComplex->DeepCopy());

		std::list<Topology::Ptr> cellsAsTopologies;
		for (const Cell::Ptr& kpCell : rkCells)
		{
			cellsAsTopologies.push_back(kpCell);
			//AttributeManager::GetInstance().DeepCopyAttributes(kpCell->GetOcctSolid(), pCopyCellComplex->GetOcctCompSolid());
		}
		CellComplex::Ptr pCopyCellComplex = TopologicalQuery::Downcast<CellComplex>(pCellComplex->DeepCopyAttributesFrom(cellsAsTopologies));

		GlobalCluster::GetInstance().AddTopology(pCopyCellComplex->GetOcctCompSolid());

		return pCopyCellComplex;
	}

	TopoDS_CompSolid CellComplex::ByOcctSolids(const TopTools_ListOfShape & rkOcctSolids)
	{
		if (rkOcctSolids.IsEmpty())
		{
			throw std::runtime_error("No cell is passed.");
		}

		TopoDS_CompSolid occtCompSolid;
		BRep_Builder occtBuilder;
		occtBuilder.MakeCompSolid(occtCompSolid);

		TopTools_ListOfShape::iterator occtSolidIterator = rkOcctSolids.begin();
		CellComplex::Ptr pCellComplex = nullptr;
		
		// If there is only one solid, create a CellComplex with only that cells
		if (rkOcctSolids.Size() == 1)
		{
			try {
				occtBuilder.Add(occtCompSolid, *occtSolidIterator);
			}
			catch (TopoDS_FrozenShape&)
			{
				throw std::runtime_error("The Cell is not free and cannot be modified.");
			}
			catch (TopoDS_UnCompatibleShapes&)
			{
				throw std::runtime_error("The Cell and Face are not compatible.");
			}
			
			pCellComplex = std::make_shared<CellComplex>(occtCompSolid);
		}
		else
		{
			// Merge the first cell with the rest.
			Topology::Ptr firstTopology = Topology::ByOcctShape(*occtSolidIterator, "");
			std::list<std::shared_ptr<Topology>> topologies;
			
			occtSolidIterator++;
			for (; occtSolidIterator != rkOcctSolids.end(); occtSolidIterator++)
			{
				topologies.push_back(Topology::ByOcctShape(*occtSolidIterator, ""));
			}
			std::shared_ptr<Cluster> otherCellsAsCluster = Cluster::ByTopologies(topologies);
			std::shared_ptr<Topology> pMergeTopology = firstTopology->Merge(otherCellsAsCluster);

			if (pMergeTopology->GetType() != TOPOLOGY_CELLCOMPLEX)
			{
				throw std::runtime_error("The input Cells do not form a CellComplex.");
			}
			
			pCellComplex = TopologicalQuery::Downcast<CellComplex>(pMergeTopology);
		}

		// Should get us a CellComplex, otherwise an exception.
		CellComplex::Ptr pCopyCellComplex = std::dynamic_pointer_cast<CellComplex>(pCellComplex->DeepCopy());
		return pCopyCellComplex->GetOcctCompSolid();
	}

	CellComplex::Ptr CellComplex::ByFaces(const std::list<Face::Ptr>& rkFaces, const double kTolerance)
	{
		BOPAlgo_MakerVolume occtMakerVolume;
		TopTools_ListOfShape occtShapes;
		for (const Face::Ptr& kpFace: rkFaces)
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
		if (occtMakerVolume.HasWarnings()) {
			throw std::runtime_error("Warnings.");
		}
		if (occtMakerVolume.HasErrors()) {
			throw std::runtime_error("The input Faces do not form a CellComplex.");
		}

		const TopoDS_Shape& rkOcctResult = occtMakerVolume.Shape();

		std::list<Cell::Ptr> cells;

		// The result is either:
		// - A solid
		if (rkOcctResult.ShapeType() == TopAbs_SOLID)
		{
			// Create a cell complex with only this
			cells.push_back(std::make_shared<Cell>(TopoDS::Solid(rkOcctResult)));
		}
		// - A compound, collect the solids
		else if (rkOcctResult.ShapeType() == TopAbs_COMPOUND)
		{
			std::string txt = Topology::Analyze(rkOcctResult);
			TopTools_MapOfShape occtShapes;
			for (TopExp_Explorer occtExplorer(rkOcctResult, TopAbs_SOLID); occtExplorer.More(); occtExplorer.Next())
			{
				const TopoDS_Shape& occtCurrent = occtExplorer.Current();
				if (!occtShapes.Contains(occtCurrent))
				{
					occtShapes.Add(occtCurrent);
					Cell::Ptr cell = std::make_shared<Cell>(TopoDS::Solid(occtCurrent));
					cells.push_back(cell);
				}
			}
		}

		CellComplex::Ptr cellComplex = ByCells(cells);

		TopoDS_CompSolid occtFixedCompSolid = OcctShapeFix(cellComplex->GetOcctCompSolid());

		CellComplex::Ptr fixedCellComplex = std::make_shared<CellComplex>(occtFixedCompSolid);
		CellComplex::Ptr copyFixedCellComplex = TopologicalQuery::Downcast<CellComplex>(fixedCellComplex->DeepCopy());

		GlobalCluster::GetInstance().AddTopology(copyFixedCellComplex->GetOcctCompSolid());
		std::list<Topology::Ptr> facesAsTopologies;
		for (const Face::Ptr& kpFace : rkFaces)
		{
			facesAsTopologies.push_back(kpFace);
			//AttributeManager::GetInstance().DeepCopyAttributes(kpFace->GetOcctFace(), copyFixedCellComplex->GetOcctCompSolid());
		}
		copyFixedCellComplex->DeepCopyAttributesFrom(facesAsTopologies);
		return copyFixedCellComplex;
	}

	Cell::Ptr CellComplex::ExternalBoundary() const
	{
		// Get the Cells
		TopTools_ListOfShape occtCellsBuildersArguments;
		std::list<Cell::Ptr> cells;
		Cells(cells);
		for (const Cell::Ptr& kpCell : cells)
		{
			occtCellsBuildersArguments.Append(kpCell->GetOcctShape());
		}


		// Do a Union
		BOPAlgo_CellsBuilder occtCellsBuilder;
		occtCellsBuilder.SetArguments(occtCellsBuildersArguments);
		occtCellsBuilder.Perform();

		if (occtCellsBuilder.HasErrors())
		{
			std::ostringstream errorStream;
			occtCellsBuilder.DumpErrors(errorStream);
			throw std::runtime_error(errorStream.str().c_str());
		}

		TopTools_ListOfShape occtListToTake;
		TopTools_ListOfShape occtListToAvoid;
		for (TopTools_ListIteratorOfListOfShape occtShapeIterator(occtCellsBuildersArguments);
			occtShapeIterator.More();
			occtShapeIterator.Next())
		{
			occtListToTake.Clear();
			occtListToTake.Append(occtShapeIterator.Value());
			occtCellsBuilder.AddToResult(occtListToTake, occtListToAvoid, 1, true);
		}

		// A cell complex is a contiguous shape, so there can be at maximum only one envelope cell.
		TopoDS_Shape occtEnvelopeShape = occtCellsBuilder.Shape();
		ShapeAnalysis_ShapeContents occtShapeAnalysis;
		occtShapeAnalysis.Perform(occtEnvelopeShape);
		int numberOfSolids = occtShapeAnalysis.NbSharedSolids();
		std::stringstream ssErrorMessage;
		ssErrorMessage << "There can be only 0 or 1 envelope cell, but this cell complex has " << numberOfSolids << " cells.";
		assert(numberOfSolids < 2 && ssErrorMessage.str().c_str());

		// Return the first Cell
		for (TopExp_Explorer occtExplorer(occtEnvelopeShape, TopAbs_SOLID); occtExplorer.More(); occtExplorer.Next())
		{
			 Cell::Ptr pCell = std::make_shared<Cell>(TopoDS::Solid(occtExplorer.Current()));
			 Cell::Ptr pCellCopy = TopologicalQuery::Downcast<TopologicCore::Cell>(pCell->DeepCopy());
			 GlobalCluster::GetInstance().AddTopology(pCellCopy->GetOcctShape());
			 return pCellCopy;
		}
		return nullptr;
	}

	void CellComplex::InternalBoundaries(std::list<Face::Ptr>& rInternalFaces) const
	{
		// Compute the envelope Cell
		Cell::Ptr pEnvelopeCell = ExternalBoundary();

		// Get the envelope Faces
		std::list<Face::Ptr> envelopeFaces;
		pEnvelopeCell->Faces(envelopeFaces);

		// Get the original Faces
		std::list<Face::Ptr> faces;
		Faces(faces);

		// An internal Face can be found in the original Face list, but not in the envelope Face list.
		Handle(IntTools_Context) pOcctIntToolsContext = new IntTools_Context();
		for (const Face::Ptr& kpFace : faces)
		{
			bool isEnvelopeFace = false;
			for (const Face::Ptr& kpEnvelopeFace : envelopeFaces)
			{
				if(BOPTools_AlgoTools::AreFacesSameDomain(kpFace->GetOcctFace(), kpEnvelopeFace->GetOcctFace(), pOcctIntToolsContext))
				{
					isEnvelopeFace = true;
					break;
				}
			}

			if (!isEnvelopeFace)
			{
				rInternalFaces.push_back(kpFace);
			}
		}
	}

	bool CellComplex::IsManifold() const
	{
		throw std::runtime_error("Not implemented yet");
	}

	void CellComplex::NonManifoldFaces(std::list<Face::Ptr>& rNonManifoldFaces) const
	{
		std::list<Face::Ptr> faces;
		Faces(faces);

		for (const Face::Ptr& kpFace : faces)
		{
			if(!kpFace->IsManifold())
			{
				rNonManifoldFaces.push_back(kpFace);
			}
		}
	}

	TopoDS_Shape& CellComplex::GetOcctShape()
	{
		return GetOcctCompSolid();
	}

	const TopoDS_Shape& CellComplex::GetOcctShape() const
	{
		return GetOcctCompSolid();
	}

	TopoDS_CompSolid& CellComplex::GetOcctCompSolid()
	{
		assert(!m_occtCompSolid.IsNull() && "CellComplex::m_occtCompSolid is null.");
		if (m_occtCompSolid.IsNull())
		{
			throw std::runtime_error("A null CellComplex is encountered.");
		}

		return m_occtCompSolid;
	}

	const TopoDS_CompSolid& CellComplex::GetOcctCompSolid() const
	{
		assert(!m_occtCompSolid.IsNull() && "CellComplex::m_occtCompSolid is null.");
		if (m_occtCompSolid.IsNull())
		{
			throw std::runtime_error("A null CellComplex is encountered.");
		}

		return m_occtCompSolid;
	}

	void CellComplex::SetOcctShape(const TopoDS_Shape & rkOcctShape)
	{
		SetOcctCompSolid(TopoDS::CompSolid(rkOcctShape));
	}

	void CellComplex::Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const
	{
		std::list<Face::Ptr> faces;
		Faces(faces);
		for (const Face::Ptr& kpFace : faces)
		{
			rOcctGeometries.push_back(kpFace->Surface());
		}
	}

	std::string CellComplex::GetTypeAsString() const
	{
		return std::string("CellComplex");
	}

	TopoDS_CompSolid CellComplex::OcctShapeFix(const TopoDS_CompSolid & rkOcctInputCompSolid)
	{
		ShapeFix_Shape occtCompSolidFix(rkOcctInputCompSolid);
		occtCompSolidFix.Perform();
		return TopoDS::CompSolid(occtCompSolidFix.Shape());
	}

	void CellComplex::SetOcctCompSolid(const TopoDS_CompSolid & rkOcctCompSolid)
	{
		m_occtCompSolid = rkOcctCompSolid;
	}

	Vertex::Ptr CellComplex::CenterOfMass() const
	{
		TopoDS_Vertex occtCenterOfMass = CenterOfMass(GetOcctCompSolid());
		return std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtCenterOfMass));
	}

	TopoDS_Vertex CellComplex::CenterOfMass(const TopoDS_CompSolid & rkOcctCompSolid)
	{
		GProp_GProps occtShapeProperties;
		BRepGProp::VolumeProperties(rkOcctCompSolid, occtShapeProperties);
		return BRepBuilderAPI_MakeVertex(occtShapeProperties.CentreOfMass());
	}

	CellComplex::CellComplex(const TopoDS_CompSolid& rkOcctCompSolid, const std::string& rkGuid)
		: Topology(3, rkOcctCompSolid, rkGuid.compare("") == 0 ? GetClassGUID() : rkGuid)
		, m_occtCompSolid(rkOcctCompSolid)
	{
		RegisterFactory(GetClassGUID(), std::make_shared<CellComplexFactory>());
	}

	CellComplex::~CellComplex()
	{

	}
}
