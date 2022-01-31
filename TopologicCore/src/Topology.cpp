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

#include "Topology.h"
#include "Cluster.h"
#include "CellComplex.h"
#include "Cell.h"
#include "Shell.h"
#include "Face.h"
#include "Wire.h"
#include "Edge.h"
#include "Vertex.h"
#include "Aperture.h"
#include "Context.h"
#include "ContentManager.h"
#include "ContextManager.h"
#include "InstanceGUIDManager.h"
#include "TopologyFactory.h"
#include "TopologyFactoryManager.h"
#include "GlobalCluster.h"
#include "Bitwise.h"
#include "Attribute.h"
#include "AttributeManager.h"
#include <Utilities/CellUtility.h>
#include <Utilities/VertexUtility.h>
#include <Utilities/FaceUtility.h>

#include <BRepCheck_Analyzer.hxx>
#include <BOPAlgo_MakerVolume.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BOPAlgo_Section.hxx>
#include <BRep_Builder.hxx>
#include <BOPDS_DS.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepCheck_Wire.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepTools.hxx>
#include <GeomAPI_IntCS.hxx>
#include <GeomAPI_IntSS.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_ShapeContents.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeFix_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_FrozenShape.hxx>
#include <TopoDS_UnCompatibleShapes.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <IntTools_EdgeFace.hxx>

#include <array>

#include <assert.h>

namespace TopologicCore
{
	int Topology::m_numOfTopologies = 0;

	void AddOcctListShapeToAnotherList(const TopTools_ListOfShape& rkAList, TopTools_ListOfShape& rAnotherList)
	{
		for (TopTools_ListIteratorOfListOfShape kIterator(rkAList);
			kIterator.More();
			kIterator.Next())
		{
			rAnotherList.Append(kIterator.Value());
		}
	}

	Topology::Ptr Topology::ClosestSimplestSubshape(const Topology::Ptr& kpTopology) const
	{
		TopoDS_Shape occtClosestSubshape;
		double minDistance = std::numeric_limits<double>::max();
		const TopoDS_Shape& kOcctThisShape = GetOcctShape();
		const TopoDS_Shape& kOcctQueryShape = kpTopology->GetOcctShape();
		TopAbs_ShapeEnum occtShapeTypes[4] = { TopAbs_VERTEX, TopAbs_EDGE, TopAbs_FACE, TopAbs_SOLID };
		for (int i = 0; i < 4; ++i)
		{
			TopAbs_ShapeEnum occtShapeType = occtShapeTypes[i];
			TopTools_MapOfShape occtCells;
			for (TopExp_Explorer occtExplorer(kOcctThisShape, occtShapeType); occtExplorer.More(); occtExplorer.Next())
			{
				const TopoDS_Shape rkCurrentChildShape = occtExplorer.Current();
				TopoDS_Shape checkDistanceShape = rkCurrentChildShape;
				/*if (i == 3)
				{
					ShapeFix_Solid occtSolidFix(TopoDS::Solid(rkCurrentChildShape));
					occtSolidFix.Perform();
					checkDistanceShape = occtSolidFix.Shape();
				}*/
				BRepExtrema_DistShapeShape occtDistanceCalculation(checkDistanceShape, kOcctQueryShape);
				bool isDone = occtDistanceCalculation.Perform();
				if (isDone)
				{
					double distance = occtDistanceCalculation.Value();
					if (distance < minDistance)
					{
						minDistance = distance;
						occtClosestSubshape = rkCurrentChildShape;
					}
					else if (minDistance <= distance &&
						distance <= minDistance + Precision::Confusion() &&
						rkCurrentChildShape.ShapeType() > occtClosestSubshape.ShapeType()) // larger value = lower dimension
					{
						TopAbs_ShapeEnum closestShapeType = occtClosestSubshape.ShapeType();
						TopAbs_ShapeEnum currentShapeType = rkCurrentChildShape.ShapeType();

						minDistance = distance;
						occtClosestSubshape = rkCurrentChildShape;
					}
				}
			}
		}

		if (occtClosestSubshape.IsNull())
		{
			return nullptr;
		}

		return Topology::ByOcctShape(occtClosestSubshape, "");
	}

	Topology::Ptr Topology::SelectSubtopology(const Vertex::Ptr& kpSelector, const int kTypeFilter) const
	{
		TopoDS_Shape occtClosestSubshape;
		double minDistance = std::numeric_limits<double>::max();
		const TopoDS_Shape& kOcctThisShape = GetOcctShape();
		const TopoDS_Shape& kOcctSelectorShape = kpSelector->GetOcctShape();
		TopAbs_ShapeEnum occtShapeTypes[4] = { TopAbs_VERTEX, TopAbs_EDGE, TopAbs_FACE, TopAbs_SOLID };
		TopologyType shapeTypes[4] = { TOPOLOGY_VERTEX, TOPOLOGY_EDGE, TOPOLOGY_FACE, TOPOLOGY_CELL };
		for (int i = 0; i < 4; ++i)
		{
			// If this is not the requested topology type, skip.
			int result = kTypeFilter & shapeTypes[i];
			if (result == 0)
			{
				continue;
			}

			TopAbs_ShapeEnum occtShapeType = occtShapeTypes[i];
			TopTools_MapOfShape occtCells;
			for (TopExp_Explorer occtExplorer(kOcctThisShape, occtShapeType); occtExplorer.More(); occtExplorer.Next())
			{
				const TopoDS_Shape rkCurrentChildShape = occtExplorer.Current();
				TopoDS_Shape checkDistanceShape = rkCurrentChildShape;
				Topology::Ptr checkDistanceTopology = Topology::ByOcctShape(checkDistanceShape, "");

				double distance = TopologicUtilities::VertexUtility::Distance(kpSelector, checkDistanceTopology);
				if (distance < minDistance)
				{
					minDistance = distance;
					occtClosestSubshape = rkCurrentChildShape;
				}
				else if (minDistance <= distance &&
					distance <= minDistance + Precision::Confusion() &&
					rkCurrentChildShape.ShapeType() > occtClosestSubshape.ShapeType()) // larger value = lower dimension
				{
					TopAbs_ShapeEnum closestShapeType = occtClosestSubshape.ShapeType();
					TopAbs_ShapeEnum currentShapeType = rkCurrentChildShape.ShapeType();

					minDistance = distance;
					occtClosestSubshape = rkCurrentChildShape;
				}
			}
		}

		if (occtClosestSubshape.IsNull())
		{
			return nullptr;
		}

		return Topology::ByOcctShape(occtClosestSubshape, "");
	}

	TopoDS_Shape Topology::SelectSubtopology(const TopoDS_Shape& rkOcctShape, const TopoDS_Shape& rkOcctSelectorShape, const int kTypeFilter, const double kDistanceThreshold)
	{
		double minDistance = 0.0;
		return SelectSubtopology(rkOcctShape, rkOcctSelectorShape, minDistance, kTypeFilter, kDistanceThreshold);
	}

	TopoDS_Shape Topology::SelectSubtopology(const TopoDS_Shape& rkOcctShape, const TopoDS_Shape& rkOcctSelectorShape, double& rMinDistance, const int kTypeFilter, const double kDistanceThreshold)
	{
		TopoDS_Shape occtClosestSubshape;
		Vertex::Ptr pSelector = TopologicalQuery::Downcast<Vertex>(Topology::ByOcctShape(rkOcctSelectorShape));
		rMinDistance = std::numeric_limits<double>::max();
		for (int i = 0; i < (int)TopAbs_SHAPE; ++i)
		{
			TopAbs_ShapeEnum occtShapeType = (TopAbs_ShapeEnum)i;
			int shapeType = (int)GetTopologyType(occtShapeType);
			if ((shapeType & kTypeFilter) == 0)
			{
				continue;
			}

			TopTools_MapOfShape occtCells;
			for (TopExp_Explorer occtExplorer(rkOcctShape, occtShapeType); occtExplorer.More(); occtExplorer.Next())
			{
				const TopoDS_Shape rkCurrentChildShape = occtExplorer.Current();
				TopoDS_Shape checkDistanceShape = rkCurrentChildShape;
				//BRepExtrema_DistShapeShape occtDistanceCalculation(checkDistanceShape, rkOcctSelectorShape);
				//bool isDone = occtDistanceCalculation.Perform();

				Topology::Ptr checkDistanceTopology = Topology::ByOcctShape(checkDistanceShape, "");

				double distance = TopologicUtilities::VertexUtility::Distance(pSelector, checkDistanceTopology);
				if (distance < rMinDistance)
				{
					rMinDistance = distance;
					occtClosestSubshape = rkCurrentChildShape;
				}
				else if (rMinDistance <= distance &&
					distance <= rMinDistance + Precision::Confusion() &&
					rkCurrentChildShape.ShapeType() > occtClosestSubshape.ShapeType()) // larger value = lower dimension
				{
					TopAbs_ShapeEnum closestShapeType = occtClosestSubshape.ShapeType();
					TopAbs_ShapeEnum currentShapeType = rkCurrentChildShape.ShapeType();

					rMinDistance = distance;
					occtClosestSubshape = rkCurrentChildShape;
				}
			}
		}

		if (rMinDistance < kDistanceThreshold)
		{
			return occtClosestSubshape;
		}
		else
		{
			return TopoDS_Shape();
		}
	}

	TopoDS_Vertex Topology::CenterOfMass(const TopoDS_Shape& rkOcctShape)
	{
		switch (rkOcctShape.ShapeType())
		{
		case TopAbs_VERTEX: return Vertex::CenterOfMass(TopoDS::Vertex(rkOcctShape));
		case TopAbs_EDGE: return Edge::CenterOfMass(TopoDS::Edge(rkOcctShape));
		case TopAbs_WIRE: return Wire::CenterOfMass(TopoDS::Wire(rkOcctShape));
		case TopAbs_FACE: return Face::CenterOfMass(TopoDS::Face(rkOcctShape));
		case TopAbs_SHELL: return Shell::CenterOfMass(TopoDS::Shell(rkOcctShape));
		case TopAbs_SOLID: return Cell::CenterOfMass(TopoDS::Solid(rkOcctShape));
		case TopAbs_COMPSOLID: return CellComplex::CenterOfMass(TopoDS::CompSolid(rkOcctShape));
		case TopAbs_COMPOUND: return Cluster::CenterOfMass(TopoDS::Compound(rkOcctShape));
		default:
			throw std::runtime_error("Unrecognised topology");
		}
	}

	TopologyType Topology::GetTopologyType(const TopAbs_ShapeEnum& rkOcctType)
	{
		switch (rkOcctType)
		{
		case TopAbs_VERTEX: return TOPOLOGY_VERTEX;
		case TopAbs_EDGE: return TOPOLOGY_EDGE;
		case TopAbs_WIRE: return TOPOLOGY_WIRE;
		case TopAbs_FACE: return TOPOLOGY_FACE;
		case TopAbs_SHELL: return TOPOLOGY_SHELL;
		case TopAbs_SOLID: return TOPOLOGY_CELL;
		case TopAbs_COMPSOLID: return TOPOLOGY_CELLCOMPLEX;
		case TopAbs_COMPOUND: return TOPOLOGY_CLUSTER;
		default:
			throw std::runtime_error("Unrecognised topology");
		}
	}

	TopAbs_ShapeEnum Topology::GetOcctTopologyType(const TopologyType& rkType)
	{
		switch (rkType)
		{
		case TOPOLOGY_VERTEX: return TopAbs_VERTEX;
		case TOPOLOGY_EDGE: return TopAbs_EDGE;
		case TOPOLOGY_WIRE: return TopAbs_WIRE;
		case TOPOLOGY_FACE: return TopAbs_FACE;
		case TOPOLOGY_SHELL: return TopAbs_SHELL;
		case TOPOLOGY_CELL: return TopAbs_SOLID;
		case TOPOLOGY_CELLCOMPLEX: return TopAbs_COMPSOLID;
		case TOPOLOGY_CLUSTER: return TopAbs_COMPOUND;
		default:
			throw std::runtime_error("Unrecognised topology");
		}
	}

	Topology::Topology(const int kDimensionality, const TopoDS_Shape& rkOcctShape, const std::string& rkGuid)
		: m_dimensionality(kDimensionality)
	{
		// If no guid is given, use the default class GUID in TopologicCore classes.
		SetInstanceGUID(rkOcctShape, rkGuid);
		m_numOfTopologies++;
	}

	TopoDS_Shape Topology::FixShape(const TopoDS_Shape& rkOcctShape)
	{
		ShapeFix_Shape occtShapeFix(rkOcctShape);
		occtShapeFix.Perform();
		return occtShapeFix.Shape();
	}

	void Topology::RegisterFactory(const std::string& rkGuid, const TopologyFactory::Ptr& kpTopologyFactory)
	{
		TopologyFactoryManager::GetInstance().Add(rkGuid, kpTopologyFactory);
	}

	Topology::Ptr Topology::ByOcctShape(const TopoDS_Shape& rkOcctShape, const std::string& rkInstanceGuid)
	{
		if (rkOcctShape.IsNull())
		{
			return nullptr;
		}
		// If there is no subtopologies, return null.
		/*TopTools_ListOfShape occtSubTopologies;
		SubTopologies(rkOcctShape, occtSubTopologies);
		if (rkOcctShape.ShapeType() != TopAbs_VERTEX && occtSubTopologies.Size() == 0)
		{
			return nullptr;
		}*/

		TopologyFactory::Ptr pTopologyFactory = nullptr;
		if (rkInstanceGuid.compare("") == 0)
		{
			pTopologyFactory = TopologyFactoryManager::GetDefaultFactory(rkOcctShape.ShapeType());
		}
		else
		{
			TopologyFactoryManager::GetInstance().Find(rkInstanceGuid, pTopologyFactory);
		}
		assert(pTopologyFactory != nullptr);
		Topology::Ptr pTopology = pTopologyFactory->Create(rkOcctShape);

		return pTopology;
	}

	TopoDS_CompSolid Topology::MakeCompSolid(const TopoDS_Shape& rkOcctShape)
	{
		TopoDS_CompSolid occtCompSolid;
		BRep_Builder occtBuilder;
		occtBuilder.MakeCompSolid(occtCompSolid);

		TopTools_MapOfShape occtCells;
		for (TopExp_Explorer occtExplorer(rkOcctShape, TopAbs_SOLID); occtExplorer.More(); occtExplorer.Next())
		{
			const TopoDS_Shape& occtCurrent = occtExplorer.Current();
			if (!occtCells.Contains(occtCurrent))
			{
				occtCells.Add(occtCurrent);
				try {
					occtBuilder.Add(occtCompSolid, occtCurrent);
				}
				catch (TopoDS_FrozenShape&)
				{
					throw std::runtime_error("The Cell is not free and cannot be modified.");
				}
				catch (TopoDS_UnCompatibleShapes&)
				{
					throw std::runtime_error("The Cell and Face are not compatible.");
				}
			}
		}

		return occtCompSolid;
	}

	Topology::~Topology()
	{
		m_numOfTopologies--;
		if (m_numOfTopologies < 0)
		{
			m_numOfTopologies = 0;
		}
		if (m_numOfTopologies == 0)
		{
			GlobalCluster::GetInstance().Clear();
		}
	}

	Topology::Ptr Topology::ByGeometry(Handle(Geom_Geometry) pGeometry)
	{
		return nullptr;
	}

	Topology::Ptr Topology::ByContext(const std::shared_ptr<Context>& kpContext)
	{
		return nullptr;
	}

	void Topology::ByVertexIndex(const std::vector<Vertex::Ptr>& rkVertices, const std::list<std::list<int>>& rkVertexIndices, std::list<Topology::Ptr>& rTopologies)
	{
		if (rkVertices.empty() || rkVertexIndices.empty())
		{
			return;
		}

		for (const std::list<int>& rkVertex1DIndices : rkVertexIndices)
		{
			if (rkVertex1DIndices.empty())
				continue;

			std::list<TopoDS_Vertex> occtVertices;
			for (int vertexIndex : rkVertex1DIndices)
			{
				occtVertices.push_back(rkVertices[vertexIndex]->GetOcctVertex());
			}

			if (occtVertices.size() > 2)
			{
				// Create edges
				TopTools_ListOfShape occtEdges;
				std::list<TopoDS_Vertex>::const_iterator kEndVertexIterator = occtVertices.end();
				kEndVertexIterator--;

				for (std::list<TopoDS_Vertex>::const_iterator kVertexIterator = occtVertices.begin();
					kVertexIterator != kEndVertexIterator;
					kVertexIterator++)
				{
					std::list<TopoDS_Vertex>::const_iterator kNextVertexIterator = kVertexIterator;
					kNextVertexIterator++;

					BRepBuilderAPI_MakeEdge occtMakeEdge(TopoDS::Vertex(*kVertexIterator), TopoDS::Vertex(*kNextVertexIterator));
					occtEdges.Append(occtMakeEdge);
				}
				// No need to connect the first and last vertices

				// Creating a face
				BRepBuilderAPI_MakeWire occtMakeWire;
				occtMakeWire.Add(occtEdges);
				if (occtMakeWire.Error() == BRepBuilderAPI_WireDone)
				{
					const TopoDS_Wire& rkOcctWire = occtMakeWire.Wire();

					if (BRepCheck_Wire(rkOcctWire).Closed() == BRepCheck_NoError)
					{
						BRepBuilderAPI_MakeFace occtMakeFace(rkOcctWire);
						if (occtMakeFace.Error() == BRepBuilderAPI_FaceDone)
						{
							rTopologies.push_back(std::make_shared<Face>(occtMakeFace)->DeepCopy());
						}
						else
						{
							// Add the closed wire
							rTopologies.push_back(std::make_shared<Wire>(rkOcctWire)->DeepCopy());
						}
					}
					else
					{
						// Add the open wire
						rTopologies.push_back(std::make_shared<Wire>(rkOcctWire)->DeepCopy());
					}
				}
				else
				{
					// Add the edges
					for (TopTools_ListIteratorOfListOfShape occtEdgeIterator(occtEdges);
						occtEdgeIterator.More();
						occtEdgeIterator.Next())
					{
						rTopologies.push_back(std::make_shared<Edge>(TopoDS::Edge(occtEdgeIterator.Value()))->DeepCopy());
					}
				}
			}
			else if (occtVertices.size() == 2)
			{
				// Try creating an edge
				BRepBuilderAPI_MakeEdge occtMakeEdge(TopoDS::Vertex(occtVertices.front()),
					TopoDS::Vertex(occtVertices.back()));
				rTopologies.push_back(std::make_shared<Edge>(occtMakeEdge)->DeepCopy());
			}
			else if (occtVertices.size() == 1)
			{
				// Insert the vertices
				rTopologies.push_back(std::make_shared<Vertex>(occtVertices.front())->DeepCopy());
			}
		}
	}

	std::shared_ptr<Topology> Topology::ByFaces(const std::list<std::shared_ptr<Face>>& rkFaces, const double kTolerance)
	{
		if (rkFaces.empty())
		{
			return nullptr;
		}

		TopTools_ListOfShape occtShapes;
		for (const Face::Ptr& kpFace : rkFaces)
		{
			occtShapes.Append(kpFace->GetOcctShape());
		}

		TopoDS_Shape occtShape = OcctSewFaces(occtShapes, kTolerance);
		Topology::Ptr pTopology = Topology::ByOcctShape(occtShape, "");
		Topology::Ptr pCopyTopology = std::dynamic_pointer_cast<Topology>(pTopology->DeepCopy());
		std::list<Topology::Ptr> facesAsTopologies;
		for (const Face::Ptr& kpFace : rkFaces)
		{
			facesAsTopologies.push_back(kpFace);
			//AttributeManager::GetInstance().DeepCopyAttributes(kpFace->GetOcctFace(), pCopyTopology->GetOcctShape());
		}
		pCopyTopology->DeepCopyAttributesFrom(facesAsTopologies);
		//GlobalCluster::GetInstance().AddTopology(pTopology->GetOcctShape());
		return pTopology;
	}

	void Topology::AddContent(const Topology::Ptr& rkTopology)
	{
		bool hasContent = ContentManager::GetInstance().HasContent(GetOcctShape(), rkTopology->GetOcctShape());
		if (hasContent)
		{
			return;
		}

		const double kDefaultParameter = 0.0;
		Topology::Ptr pCopyTopology = std::dynamic_pointer_cast<Topology>(DeepCopy());
		//GlobalCluster::GetInstance().AddTopology(pCopyTopology->GetOcctShape());
		Topology::Ptr pCopyContentTopology = std::dynamic_pointer_cast<Topology>(rkTopology->DeepCopy());
		//GlobalCluster::GetInstance().AddTopology(pCopyContentTopology->GetOcctShape());

		ContentManager::GetInstance().Add(GetOcctShape(), rkTopology);
		ContextManager::GetInstance().Add(
			rkTopology->GetOcctShape(),
			Context::ByTopologyParameters(Topology::ByOcctShape(GetOcctShape()),
				kDefaultParameter,
				kDefaultParameter,
				kDefaultParameter)
		);
	}

	Topology::Ptr Topology::AddContent(const Topology::Ptr& rkTopology, const int kTypeFilter)
	{
		std::list<Topology::Ptr> contents;
		contents.push_back(rkTopology);

		return AddContents(contents, kTypeFilter);
	}

	Topology::Ptr Topology::AddContents(const std::list<Topology::Ptr>& rkContentTopologies, const int kTypeFilter)
	{
		// Deep copy this topology
		Topology::Ptr pCopyTopology = std::dynamic_pointer_cast<Topology>(DeepCopy());

		// For all contents:
		for (const Topology::Ptr& kpContentTopology : rkContentTopologies)
		{
			Topology::Ptr selectedSubtopology;
			if (kTypeFilter == 0 || ((kTypeFilter & GetType()) != 0))
			{
				bool hasContent = ContentManager::GetInstance().HasContent(GetOcctShape(), kpContentTopology->GetOcctShape());
				if (hasContent)
				{
					continue;
				}
				selectedSubtopology = pCopyTopology;
			}
			else
			{
				Vertex::Ptr pCenterOfMass = kpContentTopology->CenterOfMass();
				if ((kTypeFilter & Cell::Type()) != 0)
				{
					// Iterate over all Cells of the original Topology. If any Cell contains this content,
					// continue
					bool hasContent = false;
					std::list<Cell::Ptr> cells;
					Cells(nullptr,cells);
					for (auto cell : cells)
					{
						std::list<Topology::Ptr> cellContents;
						cell->Contents(cellContents);

						for (auto cellContent : cellContents)
						{
							if (cellContent->IsSame(kpContentTopology))
							{
								hasContent = true;
								break;
							}
						}

						if (hasContent)
						{
							break;
						}
					}

					if (hasContent)
					{
						continue;
					}

					// Select the closest Face from the copy Topology
					Face::Ptr face = TopologicalQuery::Downcast<Face>(
						pCopyTopology->SelectSubtopology(pCenterOfMass, Face::Type()));

					std::list<Cell::Ptr> adjacentCells;
					TopologicUtilities::FaceUtility::AdjacentCells(face, pCopyTopology, adjacentCells);

					for (const Cell::Ptr& kpCell : adjacentCells)
					{
						//ShapeFix_Solid occtShapeFixSolid(kpCell->GetOcctSolid());
						//occtShapeFixSolid.Perform();
						BRepClass3d_SolidClassifier occtSolidClassifier(kpCell->GetOcctSolid(), pCenterOfMass->Point()->Pnt(), 0.1);
						TopAbs_State occtState = occtSolidClassifier.State();

						if (occtState == TopAbs_IN || occtState == TopAbs_ON)
						{
							selectedSubtopology = kpCell;
							break;
						}
					}

					// If selectedSubtopology is still null, try with the rest of the Cells.
					if (selectedSubtopology == nullptr)
					{
						std::list<Cell::Ptr> cells;
						//face->Cells(adjacentCells);
						pCopyTopology->Cells(nullptr,cells);

						for (const Cell::Ptr& kpCell : cells)
						{
							//ShapeFix_Solid occtShapeFixSolid(kpCell->GetOcctSolid());
							//occtShapeFixSolid.Perform();
							BRepClass3d_SolidClassifier occtSolidClassifier(kpCell->GetOcctSolid(), pCenterOfMass->Point()->Pnt(), 0.1);
							TopAbs_State occtState = occtSolidClassifier.State();

							if (occtState == TopAbs_IN)
							{
								selectedSubtopology = kpCell;
								break;
							}
						}
					}
				}
				else
				{
					selectedSubtopology = pCopyTopology->SelectSubtopology(pCenterOfMass, kTypeFilter);
				}
			}

			if (selectedSubtopology != nullptr)
			{
				Topology::Ptr pCopyContentTopology = std::dynamic_pointer_cast<Topology>(kpContentTopology->DeepCopy());
				//GlobalCluster::GetInstance().AddTopology(pCopyContentTopology->GetOcctShape());

				ContentManager::GetInstance().Add(selectedSubtopology->GetOcctShape(), pCopyContentTopology);

				const double kDefaultParameter = 0.0; // TODO: calculate the parameters
				ContextManager::GetInstance().Add(
					pCopyContentTopology->GetOcctShape(),
					TopologicCore::Context::ByTopologyParameters(
						selectedSubtopology,
						kDefaultParameter, kDefaultParameter, kDefaultParameter
					));
			}
		}

		//GlobalCluster::GetInstance().AddTopology(pCopyTopology->GetOcctShape());

		return pCopyTopology;
	}

	void Topology::RemoveContent(const Topology::Ptr& rkTopology)
	{
		ContentManager::GetInstance().Remove(GetOcctShape(), rkTopology->GetOcctShape());

		ContextManager::GetInstance().Remove(rkTopology->GetOcctShape(), GetOcctShape());
	}

	Vertex::Ptr Topology::Centroid() const
	{
		std::list<Vertex::Ptr> vertices;
		Vertices(nullptr, vertices);

		if (vertices.empty())
		{
			return nullptr;
		}

		double averageX = 0.0;
		double averageY = 0.0;
		double averageZ = 0.0;
		for (const Vertex::Ptr &kpVertex : vertices)
		{
			std::tuple<double, double, double> coordinates = kpVertex->Coordinates();
			averageX += std::get<0>(coordinates);
			averageY += std::get<1>(coordinates);
			averageZ += std::get<2>(coordinates);
		}

		double numOfVertices = (double)vertices.size();
		averageX /= numOfVertices;
		averageY /= numOfVertices;
		averageZ /= numOfVertices;

		Vertex::Ptr centroid = Vertex::ByCoordinates(averageX, averageY, averageZ);
		return centroid;
	}

	Topology::Ptr Topology::RemoveContents(const std::list<Topology::Ptr>& rkTopologies)
	{
		std::list<Topology::Ptr> contents;
		Contents(contents);

		std::list<Topology::Ptr> addedContents;
		for (const Topology::Ptr& kpContent : contents)
		{
			bool isRemoved = false;
			for (const Topology::Ptr& kpRemovedContent : rkTopologies)
			{
				if (kpContent->IsSame(kpRemovedContent))
				{
					isRemoved = true;
					break;
				}
			}

			if (!isRemoved)
			{
				Topology::Ptr copyContent = kpContent->DeepCopy();
				addedContents.push_back(copyContent);
			}
		}

		Topology::Ptr copyTopology = ShallowCopy()->AddContents(addedContents, 0);
		//GlobalCluster::GetInstance().AddTopology(copyTopology);
		return copyTopology;
	}

	void Topology::AddContext(const std::shared_ptr<Context>& rkContext)
	{
		// 1. Register to ContextManager
		ContextManager::GetInstance().Add(GetOcctShape(), rkContext);

		// 2. Register to ContentManager
		ContentManager::GetInstance().Add(rkContext->Topology()->GetOcctShape(), Topology::ByOcctShape(GetOcctShape(), GetInstanceGUID()));
	}

	Topology::Ptr Topology::AddContexts(const std::list<std::shared_ptr<Context>>& rkContexts)
	{
		Topology::Ptr pCopyTopology = std::dynamic_pointer_cast<Topology>(DeepCopy());
		std::string contentInstanceGUID;

		for (const Context::Ptr& kpContext : rkContexts)
		{
			bool hasContent = ContentManager::GetInstance().HasContent(kpContext->Topology()->GetOcctShape(), GetOcctShape());
			if (hasContent)
			{
				continue;
			}

			TopoDS_Shape occtCopyContentShape = pCopyTopology->GetOcctShape();
			contentInstanceGUID = pCopyTopology->GetInstanceGUID();

			Topology::Ptr pCopyContextTopology = std::dynamic_pointer_cast<Topology>(kpContext->Topology()->DeepCopy());
			//GlobalCluster::GetInstance().AddTopology(pCopyContextTopology->GetOcctShape());

			ContentManager::GetInstance().Add(pCopyContextTopology->GetOcctShape(), pCopyTopology);

			const double kDefaultParameter = 0.0; // TODO: calculate the parameters
			ContextManager::GetInstance().Add(
				pCopyTopology->GetOcctShape(),
				TopologicCore::Context::ByTopologyParameters(
					pCopyContextTopology,
					kDefaultParameter, kDefaultParameter, kDefaultParameter
				));
		}

		//GlobalCluster::GetInstance().AddTopology(pCopyTopology->GetOcctShape());

		return pCopyTopology;
	}

	void Topology::RemoveContext(const std::shared_ptr<Context>& rkContext)
	{
		// 1. Remove from ContextManager
		ContextManager::GetInstance().Remove(GetOcctShape(), rkContext->Topology()->GetOcctShape());

		// 2. Remove from ContentManager
		ContentManager::GetInstance().Remove(rkContext->Topology()->GetOcctShape(), GetOcctShape());
	}

	Topology::Ptr Topology::RemoveContexts(const std::list<Context::Ptr>& rkContexts)
	{
		std::list<Context::Ptr> contexts;
		Contexts(contexts);

		Topology::Ptr copyTopology = ShallowCopy();
		for (const Context::Ptr& kpContext : contexts)
		{
			bool isRemoved = false;
			for (const Context::Ptr& kpRemovedContext : rkContexts)
			{
				if (kpContext->Topology()->IsSame(kpRemovedContext->Topology()))
				{
					isRemoved = true;
					break;
				}
			}

			if (!isRemoved)
			{
				Topology::Ptr copyContextTopology = kpContext->Topology()->DeepCopy();
				Context::Ptr copyContext = Context::ByTopologyParameters(copyContextTopology, kpContext->U(), kpContext->V(), kpContext->W());
				copyTopology->AddContext(copyContext);
			}
		}

		//GlobalCluster::GetInstance().AddTopology(copyTopology);
		return copyTopology;
	}

	void Topology::SharedTopologies(const Topology::Ptr& kpTopology, const int kFilterType, std::list<Topology::Ptr>& rSharedTopologies) const
	{
		const TopoDS_Shape& rkOcctShape1 = GetOcctShape();
		const TopoDS_Shape& rkOcctShape2 = kpTopology->GetOcctShape();

		// Bitwise shift
		for (int i = 0; i < 9; ++i)
		{
			int intTopologyType = 1 << i;

			if ((kFilterType & intTopologyType) == 0)
			{
				continue;
			}
			TopAbs_ShapeEnum occtSubtopologyType = GetOcctTopologyType((TopologyType)intTopologyType);
			TopTools_MapOfShape occtSubtopologies1;
			DownwardNavigation(rkOcctShape1, occtSubtopologyType, occtSubtopologies1);

			TopTools_MapOfShape occtSubtopologies2;
			DownwardNavigation(rkOcctShape2, occtSubtopologyType, occtSubtopologies2);

			for (TopTools_MapIteratorOfMapOfShape occtSubtopologyIterator1(occtSubtopologies1);
				occtSubtopologyIterator1.More();
				occtSubtopologyIterator1.Next())
			{
				for (TopTools_MapIteratorOfMapOfShape occtSubtopologyIterator2(occtSubtopologies2);
					occtSubtopologyIterator2.More();
					occtSubtopologyIterator2.Next())
				{
					if (occtSubtopologyIterator1.Value().IsSame(occtSubtopologyIterator2.Value()))
					{
						Topology::Ptr topology = Topology::ByOcctShape(occtSubtopologyIterator1.Value(), "");
						rSharedTopologies.push_back(topology);
					}
				}
			}
		}
	}

	Topology::Ptr Topology::SetDictionaries(
		const std::list<Vertex::Ptr>& rkSelectors,
		const std::list<std::map<std::string, Attribute::Ptr>>& rkDictionaries,
		const int kTypeFilter)
	{
		int selectorSize = (int)rkSelectors.size();
		std::list<int> typeFilters(selectorSize, kTypeFilter);
		return SetDictionaries(rkSelectors, rkDictionaries, typeFilters);
	}

	Topology::Ptr Topology::SetDictionaries(
		const std::list<Vertex::Ptr>& rkSelectors,
		const std::list<std::map<std::string, Attribute::Ptr>>& rkDictionaries,
		const std::list<int>& rkTypeFilters, const bool expectDuplicateTopologies)
	{
		if (rkSelectors.size() != rkDictionaries.size())
		{
			throw std::runtime_error("The lists of selectors and dictionaries do not have the same length.");
		}

		if (rkSelectors.size() != rkTypeFilters.size())
		{
			throw std::runtime_error("The lists of selectors and type filters do not have the same length.");
		}

		Topology::Ptr pCopyTopology = std::dynamic_pointer_cast<Topology>(DeepCopy());
		std::string contextInstanceGUID;

		std::list<Topology::Ptr> selectedSubtopologies;
		std::list<Vertex::Ptr>::const_iterator kSelectorIterator = rkSelectors.begin();
		std::list<int>::const_iterator kTypeFilterIterator = rkTypeFilters.begin();
		for (; kSelectorIterator != rkSelectors.end() && kTypeFilterIterator != rkTypeFilters.end();
			kSelectorIterator++, kTypeFilterIterator++)
		{
			const Vertex::Ptr& kpSelector = *kSelectorIterator;
			const int kTypeFilter = *kTypeFilterIterator;
			if (kTypeFilter == 0)
			{
				throw std::runtime_error("No type filter specified.");
			}

			Topology::Ptr selectedSubtopology = nullptr;
			if ((kTypeFilter & Cell::Type()) != 0)
			{
				Topology::Ptr closestFaceAsTopology = pCopyTopology->SelectSubtopology(kpSelector, Face::Type());
				// Select the closest Face. Note: if there is no Face, there is no Cell.
				Face::Ptr closestFace = nullptr;
				try {
					closestFace = TopologicalQuery::Downcast<Face>(closestFaceAsTopology);
				}
				catch (const std::runtime_error&)
				{

				}

				// Only continue if there is a closestFace.
				if (closestFace != nullptr)
				{
					std::list<Cell::Ptr> adjacentCells;
					TopologicUtilities::FaceUtility::AdjacentCells(closestFace, pCopyTopology, adjacentCells);

					for (const Cell::Ptr& kpCell : adjacentCells)
					{
						BRepClass3d_SolidClassifier occtSolidClassifier(kpCell->GetOcctSolid(), kpSelector->Point()->Pnt(), 0.1);
						TopAbs_State occtState = occtSolidClassifier.State();

						if (occtState == TopAbs_IN)
						{
							selectedSubtopology = kpCell;
							break;
						}
					}

					// If selectedSubtopology is still null, try with the rest of the Cells.
					if (selectedSubtopology == nullptr)
					{
						std::list<Cell::Ptr> cells;
						pCopyTopology->Cells(nullptr, cells);

						for (const Cell::Ptr& kpCell : cells)
						{
							BRepClass3d_SolidClassifier occtSolidClassifier(kpCell->GetOcctSolid(), kpSelector->Point()->Pnt(), 0.1);
							TopAbs_State occtState = occtSolidClassifier.State();

							if (occtState == TopAbs_IN)
							{
								selectedSubtopology = kpCell;
								break;
							}
						}
					}
				}
			}
			else
			{
				selectedSubtopology = pCopyTopology->SelectSubtopology(kpSelector, kTypeFilter);
			}

			if (selectedSubtopology != nullptr && !expectDuplicateTopologies)
			{
				for (const Topology::Ptr& kpSelectedSubtopology : selectedSubtopologies)
				{
					if (kpSelectedSubtopology != nullptr && kpSelectedSubtopology->IsSame(selectedSubtopology))
					{
						throw std::runtime_error("Another selector has selected the same member of the input Topology.");
					}
				}
			}

			selectedSubtopologies.push_back(selectedSubtopology);
		}

		auto rkDictionaryIterator = rkDictionaries.begin();
		for (const Topology::Ptr& kpSelectedSubtopology : selectedSubtopologies)
		{
			if (kpSelectedSubtopology != nullptr)
			{
				AttributeManager::GetInstance().ClearOne(kpSelectedSubtopology->GetOcctShape());
				for (const auto &kpAttributePair : *rkDictionaryIterator)
				{
					AttributeManager::GetInstance().Add(kpSelectedSubtopology->GetOcctShape(), kpAttributePair.first, kpAttributePair.second);
				}
			}

			rkDictionaryIterator++;
		}

		//GlobalCluster::GetInstance().AddTopology(pCopyTopology->GetOcctShape());

		return pCopyTopology;
	}

	TOPOLOGIC_API Topology::Ptr Topology::SetDictionaries(
		const std::list<std::shared_ptr<Vertex>>& rkSelectors,
		const std::list<Dictionary>& rkDictionaries,
		const int kTypeFilter)
	{
		std::list<std::map<std::string, Attribute::Ptr>> newRkDictionaries;
		for (auto i : rkDictionaries)
			newRkDictionaries.push_back(i);

		return SetDictionaries(rkSelectors, newRkDictionaries, kTypeFilter);
	}

	Topology::Ptr Topology::SetDictionaries(
		const std::list<std::shared_ptr<Vertex>>& rkSelectors,
		const std::list<Dictionary>& rkDictionaries,
		const std::list<int>& rkTypeFilters,
		const bool expectDuplicateTopologies)
	{
		std::list<std::map<std::string, Attribute::Ptr>> newRkDictionaries;
		for (auto i : rkDictionaries)
			newRkDictionaries.push_back(i);

		return SetDictionaries(rkSelectors, newRkDictionaries, rkTypeFilters, expectDuplicateTopologies);
	}

	TopoDS_Shape Topology::OcctSewFaces(const TopTools_ListOfShape & rkOcctFaces, const double kTolerance)
	{
		BRepBuilderAPI_Sewing occtSewing(kTolerance, true, true, true, true);
		for (TopTools_ListIteratorOfListOfShape occtEdgeIterator(rkOcctFaces);
			occtEdgeIterator.More();
			occtEdgeIterator.Next())
		{
			occtSewing.Add(occtEdgeIterator.Value());
		}

		occtSewing.Perform();
		if (occtSewing.SewedShape().IsNull())
		{
			throw std::runtime_error("A null shape is created.");
		}
		TopAbs_ShapeEnum type = occtSewing.SewedShape().ShapeType();

		for (TopTools_ListIteratorOfListOfShape occtEdgeIterator(rkOcctFaces);
			occtEdgeIterator.More();
			occtEdgeIterator.Next())
		{
			const TopoDS_Shape& rkModifiedShape = occtSewing.Modified(occtEdgeIterator.Value());
			Topology::Ptr pChildTopology = Topology::ByOcctShape(rkModifiedShape, "");

			// Map the aperture to the modifed shell faces.
			std::list<Topology::Ptr> contents;
			ContentManager::GetInstance().Find(occtEdgeIterator.Value(), contents);
			for (const Topology::Ptr& rkContent : contents)
			{
				if (rkContent->GetType() != TOPOLOGY_APERTURE)
				{
					continue;
				}

				std::shared_ptr<Aperture> aperture = TopologicalQuery::Downcast<Aperture>(rkContent);

				if (aperture->Topology()->GetType() != TOPOLOGY_FACE)
				{
					continue;
				}

				Face::Ptr pApertureFace = TopologicalQuery::Downcast<Face>(aperture->Topology());
				Topology::Ptr pUpcastApertureFace = TopologicalQuery::Upcast<Topology>(pApertureFace);
			}
		}

		return occtSewing.SewedShape();
	}

	void BooleanTransferDictionary(Topology const * const kpkOriginTopology1, Topology const * const kpkOriginTopology2, Topology const * const kpkDestinationTopology, bool initClearDictionary)
	{
		TopoDS_Shape occtOriginShape1 = kpkOriginTopology1->GetOcctShape();
		TopoDS_Shape occtOriginShape2 = kpkOriginTopology2->GetOcctShape();
		TopoDS_Shape occtDestinationShape = kpkDestinationTopology->GetOcctShape();

		// Get vertices, edges, faces, cells, cellComplexes from kpkDestinationTopology, and map them to the originTopology
		TopoDS_Shape occtOriginShape;
		if (kpkOriginTopology1 == nullptr && kpkOriginTopology2 == nullptr)
		{
			throw std::runtime_error("Fails to transfer dictionari in a Boolean operation because the original Topologies are null.");
		}
		else if (kpkOriginTopology1 == nullptr && kpkOriginTopology2 != nullptr)
		{
			occtOriginShape = kpkOriginTopology2->GetOcctShape();
		}
		else if (kpkOriginTopology1 != nullptr && kpkOriginTopology2 == nullptr)
		{
			occtOriginShape = kpkOriginTopology1->GetOcctShape();
		}
		else
		{
			TopTools_MapOfShape occtOriginShapes;
			occtOriginShapes.Add(kpkOriginTopology1->GetOcctShape());
			occtOriginShapes.Add(kpkOriginTopology2->GetOcctShape());
			occtOriginShape = Cluster::ByOcctTopologies(occtOriginShapes);
		}

		// Get vertices, edges, faces, cells, cellComplexes from kpkDestinationTopology, and map them to the originTopology
		TopologyType topologyTypes[5] = { TOPOLOGY_VERTEX, TOPOLOGY_EDGE, TOPOLOGY_FACE, TOPOLOGY_CELL, TOPOLOGY_CELLCOMPLEX };
		TopAbs_ShapeEnum occtTopologyTypes[5] = { TopAbs_VERTEX, TopAbs_EDGE, TopAbs_FACE, TopAbs_SOLID, TopAbs_COMPSOLID };
		for (int i = 0; i < 5; ++i)
		{
			TopTools_MapOfShape occtDestinationMembers;
			kpkDestinationTopology->DownwardNavigation(occtDestinationShape, occtTopologyTypes[i], occtDestinationMembers);

			for (TopTools_MapIteratorOfMapOfShape occtDestinationMemberIterator(occtDestinationMembers);
				occtDestinationMemberIterator.More();
				occtDestinationMemberIterator.Next())
			{
				TopoDS_Shape occtDestinationMember = occtDestinationMemberIterator.Value();

				if (initClearDictionary)
				{
					AttributeManager::GetInstance().ClearOne(occtDestinationMember);
				}

				// Find the member in originTopology
				TopoDS_Shape occtDestinationMemberCenterOfMass = Topology::CenterOfMass(occtDestinationMember);
				double minDistance1 = 0.0;
				TopoDS_Shape occtOriginMember1 = Topology::SelectSubtopology(
					occtOriginShape1, occtDestinationMemberCenterOfMass, minDistance1, topologyTypes[i], 0.0001);
				double minDistance2 = 0.0;
				TopoDS_Shape occtOriginMember2 = Topology::SelectSubtopology(
					occtOriginShape2, occtDestinationMemberCenterOfMass, minDistance2, topologyTypes[i], 0.0001);

				if (!occtDestinationMemberCenterOfMass.IsNull() && !occtOriginMember1.IsNull())
				{
					AttributeManager::GetInstance().CopyAttributes(occtOriginMember1, occtDestinationMember, true);
				}
				if (!occtDestinationMemberCenterOfMass.IsNull() && !occtOriginMember2.IsNull())
				{
					AttributeManager::GetInstance().CopyAttributes(occtOriginMember2, occtDestinationMember, true);
				}
			}
		}
	}

	Topology::Ptr Topology::Difference(const Topology::Ptr & kpOtherTopology, const bool kTransferDictionary)
	{
		if (kpOtherTopology == nullptr)
		{
			return Topology::ByOcctShape(GetOcctShape(), GetInstanceGUID());
		}

		TopTools_ListOfShape occtArgumentsA;
		TopTools_ListOfShape occtArgumentsB;
		AddBooleanOperands(kpOtherTopology, occtArgumentsA, occtArgumentsB);

		BOPAlgo_CellsBuilder occtCellsBuilder;
		NonRegularBooleanOperation(occtArgumentsA, occtArgumentsB, occtCellsBuilder);

		// 2. Select the parts to be included in the final result.
		TopTools_ListOfShape occtListToTake;
		TopTools_ListOfShape occtListToAvoid;

		for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorA(occtArgumentsA);
			kOcctShapeIteratorA.More();
			kOcctShapeIteratorA.Next())
		{
			occtListToTake.Clear();
			occtListToAvoid.Clear();
			occtListToTake.Append(kOcctShapeIteratorA.Value());
			for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorB(occtArgumentsB);
				kOcctShapeIteratorB.More();
				kOcctShapeIteratorB.Next())
			{
				occtListToAvoid.Append(kOcctShapeIteratorB.Value());
			}
			occtCellsBuilder.AddToResult(occtListToTake, occtListToAvoid);
		}

		occtCellsBuilder.MakeContainers();

		TopoDS_Shape occtResultShape = occtCellsBuilder.Shape();
		TopoDS_Shape occtPostprocessedShape = occtResultShape.IsNull()? occtResultShape : PostprocessBooleanResult(occtResultShape);
		Topology::Ptr pPostprocessedShape = Topology::ByOcctShape(occtPostprocessedShape, ""); 
		if (pPostprocessedShape == nullptr)
		{
			return nullptr;
		}

		TransferContents(GetOcctShape(), pPostprocessedShape);
		TransferContents(kpOtherTopology->GetOcctShape(), pPostprocessedShape);
		//AttributeManager::GetInstance().DeepCopyAttributes(GetOcctShape(), occtPostprocessedShape);
		Topology::Ptr pCopyPostprocessedShape = pPostprocessedShape->DeepCopy();

		if (kTransferDictionary)
		{
			BooleanTransferDictionary(this, kpOtherTopology.get(), pCopyPostprocessedShape.get(), true);
		}
		//GlobalCluster::GetInstance().AddTopology(pCopyPostprocessedShape->GetOcctShape());
		return pCopyPostprocessedShape;
	}

	void Topology::Contents(std::list<Topology::Ptr>& rContents) const
	{
		Contents(GetOcctShape(), rContents);
	}

	void Topology::Contents(const TopoDS_Shape & rkOcctShape, std::list<Topology::Ptr>& rContents)
	{
		ContentManager::GetInstance().Find(rkOcctShape, rContents);
	}

	void Topology::Apertures(std::list<Aperture::Ptr>& rApertures) const
	{
		Apertures(GetOcctShape(), rApertures);
	}

	void Topology::Apertures(const TopoDS_Shape & rkOcctShape, std::list<Aperture::Ptr>& rApertures)
	{
		std::list<Topology::Ptr> contents;
		ContentManager::GetInstance().Find(rkOcctShape, contents);

		for (const Topology::Ptr& kpContent : contents)
		{
			if (kpContent->GetType() == TOPOLOGY_APERTURE)
			{
				std::shared_ptr<Aperture> aperture = TopologicalQuery::Downcast<Aperture>(kpContent);
				rApertures.push_back(aperture);
			}
		}
	}

	void Topology::SubContents(std::list<Topology::Ptr>& rSubContents) const
	{
		SubContents(GetOcctShape(), rSubContents);
	}
	
	void Topology::SubContents(const TopoDS_Shape & rkOcctShape, std::list<Topology::Ptr>& rSubContents)
	{
		Contents(rkOcctShape, rSubContents);
		TopAbs_ShapeEnum occtType = rkOcctShape.ShapeType();
		int occtTypeInt = (int)occtType + 1; // +1 for the next lower type
		for (int occtTypeIntIteration = occtTypeInt; occtTypeIntIteration != (int)TopAbs_SHAPE; occtTypeIntIteration++)
		{
			// Get members in each level
			TopAbs_ShapeEnum occtTypeIteration = (TopAbs_ShapeEnum)occtTypeIntIteration;
			TopTools_MapOfShape occtMembers;
			DownwardNavigation(rkOcctShape, occtTypeIteration, occtMembers);

			// For each member, get the contents
			for (TopTools_MapIteratorOfMapOfShape occtMemberIterator(occtMembers);
				occtMemberIterator.More();
				occtMemberIterator.Next())
			{
				ContentManager::GetInstance().Find(occtMemberIterator.Value(), rSubContents);
			}
		}
	}

	bool Topology::Contexts(std::list<std::shared_ptr<Context>>& rContexts) const
	{
		return Contexts(GetOcctShape(), rContexts);
	}

	bool Topology::Contexts(const TopoDS_Shape & rkOcctShape, std::list<std::shared_ptr<Context>>& rContexts)
	{
		return ContextManager::GetInstance().Find(rkOcctShape, rContexts);
	}

	bool Topology::ExportToBRep(const std::string & rkFilePath) const
	{
		return BRepTools::Write(GetOcctShape(), rkFilePath.c_str());;
	}

	Topology::Ptr Topology::ByImportedBRep(const std::string & rkFilePath)
	{
		TopoDS_Shape occtShape;
		BRep_Builder occtBRepBuilder;
		bool returnValue = BRepTools::Read(occtShape, rkFilePath.c_str(), occtBRepBuilder);
		Topology::Ptr pTopology = Topology::ByOcctShape(occtShape, "");

		//GlobalCluster::GetInstance().AddTopology(pTopology);
		return pTopology;
	}

	Topology::Ptr Topology::ByString(const std::string& rkBrepString)
	{
		TopoDS_Shape occtShape;
		BRep_Builder brepBuilder;
		std::istringstream iss(rkBrepString, std::ios_base::in);
		BRepTools::Read(occtShape, iss, brepBuilder);
		Topology::Ptr pTopology = Topology::ByOcctShape(occtShape, "");
		return pTopology;
	}

	std::string Topology::String() const
	{
		std::ostringstream oss(std::ios_base::out);
		BRepTools::Write(GetOcctShape(), oss);
		return oss.str();
	}

	void Topology::Filter(const std::list<Topology::Ptr>& rkTopologies, const int kTypeFilter, std::list<Topology::Ptr>& rFilteredTopologies)
	{
		for (const Topology::Ptr& kpTopology : rkTopologies)
		{
			int shapeType = (int)kpTopology->GetType();
			if ((shapeType & kTypeFilter) == 0)
			{
				continue;
			}

			rFilteredTopologies.push_back(kpTopology);
		}
	}

	std::string Topology::Analyze(const TopoDS_Shape& rkShape, const int kLevel)
	{
		TopTools_ListOfShape occtSubTopologies;
		SubTopologies(rkShape, occtSubTopologies);

		std::array<std::string, 8> occtShapeNameSingular;
		occtShapeNameSingular[0] = "a cluster";
		occtShapeNameSingular[1] = "a cellComplex";
		occtShapeNameSingular[2] = "a cell";
		occtShapeNameSingular[3] = "a shell";
		occtShapeNameSingular[4] = "a face";
		occtShapeNameSingular[5] = "a wire";
		occtShapeNameSingular[6] = "an edge";
		occtShapeNameSingular[7] = "a vertex";

		std::array<std::string, 8> occtShapeNamePlural;
		occtShapeNamePlural[0] = "clusters";
		occtShapeNamePlural[1] = "cellComplexes";
		occtShapeNamePlural[2] = "cells";
		occtShapeNamePlural[3] = "shells";
		occtShapeNamePlural[4] = "faces";
		occtShapeNamePlural[5] = "wires";
		occtShapeNamePlural[6] = "edges";
		occtShapeNamePlural[7] = "vertices";

		TopAbs_ShapeEnum occtShapeType = rkShape.ShapeType();
		std::stringstream ssCurrentIndent;
		for (int i = 0; i < kLevel; ++i)
		{
			ssCurrentIndent << "  ";
		}
		std::string currentIndent = ssCurrentIndent.str();
		int numberOfSubentities[8] = { 0,0,0,0,0,0,0,0 };
		for (TopTools_ListIteratorOfListOfShape kMemberIterator(occtSubTopologies);
			kMemberIterator.More();
			kMemberIterator.Next())
		{
			const TopoDS_Shape& rkMemberTopology = kMemberIterator.Value();
			TopAbs_ShapeEnum occtShapeMemberType = rkMemberTopology.ShapeType();
			numberOfSubentities[occtShapeMemberType]++;
		}

		std::stringstream ssCurrentResult;

		// For the topmost level only, print the overall subentities result
		if (kLevel == 0)
		{
			ShapeAnalysis_ShapeContents occtShapeAnalysis;
			occtShapeAnalysis.Perform(rkShape);

			// No method is provided in ShapeAnalysis_ShapeContents to compute the number of CompSolids.
			// Do this manually.
			int numberCompSolids = 0;
			TopTools_ListOfShape occtCompSolids;
			for (TopExp_Explorer occtExplorer(rkShape, TopAbs_COMPSOLID); occtExplorer.More(); occtExplorer.Next())
			{
				const TopoDS_Shape& rkOcctCurrent = occtExplorer.Current();
				if (!occtCompSolids.Contains(rkOcctCurrent))
				{
					occtCompSolids.Append(rkOcctCurrent);
					numberCompSolids++;
				}
			}
			ssCurrentResult <<
				"OVERALL ANALYSIS" << std::endl <<
				"================" << std::endl <<
				"The shape is " << occtShapeNameSingular[occtShapeType] << "." << std::endl;

			if (occtShapeType == 0) // Only for cluster
			{
				ssCurrentResult << "Number of cell complexes = " << numberCompSolids << std::endl;
			}

			if (occtShapeType <= 1) // Only up to cellcomplex
			{
				ssCurrentResult << "Number of cells = " << occtShapeAnalysis.NbSharedSolids() << std::endl;
			}

			if (occtShapeType <= 2) // Only up to cell
			{
				ssCurrentResult << "Number of shells = " << occtShapeAnalysis.NbSharedShells() << std::endl;
			}

			if (occtShapeType <= 3) // Only up to shell
			{
				ssCurrentResult << "Number of faces = " << occtShapeAnalysis.NbSharedFaces() << std::endl;
			}
			if (occtShapeType <= 4) // Only up to face
			{
				ssCurrentResult << "Number of wires = " << occtShapeAnalysis.NbSharedWires() << std::endl;
			}

			if (occtShapeType <= 5) // Only up to wire
			{
				ssCurrentResult << "Number of edges = " << occtShapeAnalysis.NbSharedEdges() << std::endl;
			}

			if (occtShapeType <= 6) // Only up to shell
			{
				ssCurrentResult << "Number of vertices = " << occtShapeAnalysis.NbSharedVertices() << std::endl;
			}

			ssCurrentResult << std::endl << std::endl <<
				"INDIVIDUAL ANALYSIS" << std::endl <<
				"================" << std::endl;
		}

		ssCurrentResult << currentIndent << "The shape is " << occtShapeNameSingular[occtShapeType] << "." << std::endl;

		for (int i = occtShapeType + 1; i < 8; ++i)
		{
			if(numberOfSubentities[i] > 0)
			{
				ssCurrentResult << currentIndent << "Number of " << occtShapeNamePlural[i] << " = " << numberOfSubentities[i] << std::endl;
			}
		}
		ssCurrentResult << currentIndent << "================" << std::endl;

		for (TopTools_ListIteratorOfListOfShape kMemberIterator(occtSubTopologies);
			kMemberIterator.More();
			kMemberIterator.Next())
		{
			const TopoDS_Shape& rkMemberTopology = kMemberIterator.Value();
			std::string strMemberAnalyze = Analyze(rkMemberTopology, kLevel + 1);
			ssCurrentResult << strMemberAnalyze;
		}

		return ssCurrentResult.str();
	}

	TopoDS_Shape Topology::Simplify(TopoDS_Shape & rOcctShape)
	{
		// Simplify needs to do the followings.
		// 1. The input is a container type, otherwise return itself.
		// 2. If the input is an empty cluster: return null
		// 3. Else if the input just contains one container element: recursively dive deeper until a non-container element OR
		//    a container with more than one elements is found.
		// 4. Else if the input contains more than elements:
		//    a. For a non-container element: leave it.
		//    b. For a container element: recursively dive deeper until a non-container element OR
		//       a container with more than one elements is found.
		if (!IsContainerType(rOcctShape))
			return rOcctShape;

		TopTools_ListOfShape occtSubTopologies;
		SubTopologies(rOcctShape, occtSubTopologies);

		if (occtSubTopologies.Size() == 0)
		{
			return TopoDS_Shape();
		}
		else if (occtSubTopologies.Size() == 1)
		{
			TopoDS_Shape occtCurrentShape = rOcctShape;
			TopTools_ListOfShape occtShapes;
			bool isSimplestShapeFound = false;
			do
			{
				// Only do this for wire, shell, cellcomplex, cluster
				if (!IsContainerType(occtCurrentShape))
				{
					break;
				}

				SubTopologies(occtCurrentShape, occtShapes);

				int numOfSubTopologies = occtShapes.Size();
				if (numOfSubTopologies != 1)
				{
					// occtCurrentShape does not change.
					isSimplestShapeFound = true;
				}
				else // if (occtShapes.Size() == 1)
				{
					// Go deeper
					occtCurrentShape = *occtShapes.begin();
				}
				occtShapes.Clear();
			} while (!isSimplestShapeFound);
			return occtCurrentShape;
		}
		else // occtSubTopologies.Size() > 1
		{
			TopTools_ListOfShape occtShapesToRemove;
			TopTools_ListOfShape occtShapesToAdd;

			for (TopTools_ListIteratorOfListOfShape occtSubTopologyIterator(occtSubTopologies);
				occtSubTopologyIterator.More();
				occtSubTopologyIterator.Next())
			{
				TopoDS_Shape& rOcctSubShape = occtSubTopologyIterator.Value();
				if (!IsContainerType(rOcctSubShape))
				{
					continue;
				}

				TopoDS_Shape occtCurrentShape = rOcctSubShape;
				TopTools_ListOfShape occtShapes;
				bool isSimplestShapeFound = false;
				do
				{
					// Only do this for wire, shell, cellcomplex, cluster
					if (!IsContainerType(occtCurrentShape))
					{
						break;
					}

					SubTopologies(occtCurrentShape, occtShapes);

					int numOfSubTopologies = occtShapes.Size();
					if (numOfSubTopologies != 1)
					{
						// occtCurrentShape does not change.
						isSimplestShapeFound = true;
					}
					else // if (occtShapes.Size() == 1)
					{
						// Go deeper
						occtCurrentShape = *occtShapes.begin();
					}
					occtShapes.Clear();
				} while (!isSimplestShapeFound);

				if (!occtCurrentShape.IsSame(rOcctSubShape))
				{
					// Do this so as to not modify the list in the iteration.
					occtShapesToRemove.Append(rOcctSubShape);
					occtShapesToAdd.Append(occtCurrentShape);
				}
			}

			for (TopTools_ListIteratorOfListOfShape occtSubTopologyToRemoveIterator(occtShapesToRemove);
				occtSubTopologyToRemoveIterator.More();
				occtSubTopologyToRemoveIterator.Next())
			{
				TopoDS_Builder occtBuilder;
				try {
					occtBuilder.Remove(rOcctShape, occtSubTopologyToRemoveIterator.Value());
				}
				catch (TopoDS_FrozenShape)
				{
					throw std::runtime_error("Topology is locked, cannot remove subtopology. Please contact the developer.");
				}
			}

			for (TopTools_ListIteratorOfListOfShape occtSubTopologyToAddIterator(occtShapesToAdd);
				occtSubTopologyToAddIterator.More();
				occtSubTopologyToAddIterator.Next())
			{
				TopoDS_Builder occtBuilder;
				try {
					occtBuilder.Add(rOcctShape, occtSubTopologyToAddIterator.Value());
				}
				catch (TopoDS_FrozenShape)
				{
					throw std::runtime_error("Topology is locked, cannot remove subtopology. Please contact the developer.");
				}
				catch (TopoDS_UnCompatibleShapes)
				{
					throw std::runtime_error("Cannot add incompatible subtopology.");
				}
			}

			return rOcctShape;
		}
	}

	TopoDS_Shape Topology::BooleanSubTopologyContainment(TopoDS_Shape & rOcctShape)
	{
		// 1. Only for cluster
		// 2. If the input is an empty cluster: return null
		// 3. For each subtopology A, check against each other subtopology B. If A is inside B, remove A.

		if (rOcctShape.ShapeType() != TopAbs_COMPOUND)
		{
			return rOcctShape;
		}

		TopTools_ListOfShape occtSubTopologies;
		SubTopologies(rOcctShape, occtSubTopologies);

		if (occtSubTopologies.Size() == 0)
		{
			return TopoDS_Shape();
		}

		TopTools_MapOfShape occtShapesToRemove;
		for (TopTools_ListIteratorOfListOfShape occtSubTopologyIteratorA(occtSubTopologies);
			occtSubTopologyIteratorA.More();
			occtSubTopologyIteratorA.Next())
		{
			TopoDS_Shape& rOcctSubtopologyA = occtSubTopologyIteratorA.Value();	

			bool isShapeAToRemove = false;
			for (TopTools_ListIteratorOfListOfShape occtSubTopologyIteratorB(occtSubTopologies);
				occtSubTopologyIteratorB.More() && !isShapeAToRemove;
				occtSubTopologyIteratorB.Next())
			{
				TopoDS_Shape& rOcctSubtopologyB = occtSubTopologyIteratorB.Value();

				if (rOcctSubtopologyA.IsSame(rOcctSubtopologyB))
				{
					continue;
				}

				// Does B contain A?
				TopTools_MapOfShape occtSubTopologiesB;
				DownwardNavigation(rOcctSubtopologyB, rOcctSubtopologyA.ShapeType(), occtSubTopologiesB);
				if (occtSubTopologiesB.Contains(rOcctSubtopologyA))
				{
					isShapeAToRemove = true;
					occtShapesToRemove.Add(rOcctSubtopologyA);
				}
			}
		}
		
		// Remove the shapes
		for (TopTools_MapIteratorOfMapOfShape occtShapesToRemoveIterator(occtShapesToRemove);
			occtShapesToRemoveIterator.More();
			occtShapesToRemoveIterator.Next())
		{
			TopoDS_Builder occtBuilder;
			try {
				occtBuilder.Remove(rOcctShape, occtShapesToRemoveIterator.Value());
			}
			catch (TopoDS_FrozenShape)
			{
				throw std::runtime_error("Topology is locked, cannot remove subtopology. Please contact the developer.");
			}
		}

		return rOcctShape;
	}

	std::string Topology::Analyze()
	{
		return Analyze(GetOcctShape(), 0);
	}

	void Topology::NonRegularBooleanOperation(
		const Topology::Ptr& kpOtherTopology,
		BOPAlgo_CellsBuilder& rOcctCellsBuilder,
		TopTools_ListOfShape& rOcctCellsBuildersOperandsA,
		TopTools_ListOfShape& rOcctCellsBuildersOperandsB,
		TopTools_DataMapOfShapeShape& rOcctMapFaceToFixedFaceA,
		TopTools_DataMapOfShapeShape& rOcctMapFaceToFixedFaceB)
	{
		AddBooleanOperands(kpOtherTopology, rOcctCellsBuilder, rOcctCellsBuildersOperandsA, rOcctCellsBuildersOperandsB, rOcctMapFaceToFixedFaceA, rOcctMapFaceToFixedFaceB);

		// Split the arguments and tools
		try {
			rOcctCellsBuilder.Perform();
		}
		catch (Standard_Failure& e)
		{
			const char* str = e.GetMessageString();
			std::string stlStr(str);
		}
		catch (std::runtime_error& e)
		{
			const char* str = e.what();
			std::string stlStr(str);
		}


		if (rOcctCellsBuilder.HasErrors())
		{
			std::ostringstream errorStream;
			rOcctCellsBuilder.DumpErrors(errorStream);
			throw std::runtime_error(errorStream.str().c_str());
		}
	}

	void Topology::NonRegularBooleanOperation(
		const TopTools_ListOfShape & rkOcctArgumentsA,
		const TopTools_ListOfShape & rkOcctArgumentsB,
		BOPAlgo_CellsBuilder & rOcctCellsBuilder)
	{
		TopTools_ListOfShape occtArguments;
		for (TopTools_ListIteratorOfListOfShape occtArgumentIteratorA(rkOcctArgumentsA);
			occtArgumentIteratorA.More();
			occtArgumentIteratorA.Next())
		{
			occtArguments.Append(occtArgumentIteratorA.Value());
		}
		for (TopTools_ListIteratorOfListOfShape occtArgumentIteratorB(rkOcctArgumentsB);
			occtArgumentIteratorB.More();
			occtArgumentIteratorB.Next())
		{
			occtArguments.Append(occtArgumentIteratorB.Value());
		}
		rOcctCellsBuilder.SetArguments(occtArguments);
		//rOcctCellsBuilder.SetNonDestructive(false);

		// Split the arguments and tools
		try {
			rOcctCellsBuilder.Perform();
		}
		catch (Standard_Failure&)
		{
		}
		catch (std::runtime_error&)
		{
		}

		if (rOcctCellsBuilder.HasErrors())
		{
			std::ostringstream errorStream;
			rOcctCellsBuilder.DumpErrors(errorStream);
			throw std::runtime_error(errorStream.str().c_str());
		}
	}

	void Topology::TransferContents(const TopoDS_Shape& rkOcctShape1, const Topology::Ptr& kpTopology2)
	{
		std::list<Topology::Ptr> subContents;
		SubContents(rkOcctShape1, subContents);

		for (const Topology::Ptr &kpSubContent : subContents)
		{
			// Attach to the same context type
			int contextType = 0;
			std::list<Context::Ptr> contexts;
			kpSubContent->Contexts(contexts);
			for (const Context::Ptr& kpContext : contexts)
			{
				Topology::Ptr pContextTopology = kpContext->Topology();
				TopologyType contextTopologyType = pContextTopology->GetType();
				contextType = contextType | contextTopologyType;

				// Remove content from current contexts
				pContextTopology->RemoveContent(kpSubContent);
				kpSubContent->RemoveContext(kpContext);
			}

			kpTopology2->AddContent(kpSubContent, contextType);
		}
	}

	void Topology::TransferContents(const TopoDS_Shape & rkOcctShape1, const Topology::Ptr & kpTopology2, const TopTools_ListOfShape & rkOcctDeletedSubshapes)
	{
		std::list<Topology::Ptr> subContents;
		SubContents(rkOcctShape1, subContents);

		for (const Topology::Ptr &kpSubContent : subContents)
		{
			// Check if the context topology is part of kpTopology2. Use OCCT IsDeleted()
			bool allContextsDisappear = true;
			std::list<Context::Ptr> contexts;
			kpSubContent->Contexts(contexts);
			for (const Context::Ptr& kpContext : contexts)
			{
				if (!rkOcctDeletedSubshapes.Contains(kpContext->Topology()->GetOcctShape()))
				{
					allContextsDisappear = false;
					break;
				}
			}
			if (allContextsDisappear)
			{
				continue;
			}
			
			// Attach to the same context type
			int contextType = 0;
			for (const Context::Ptr& kpContext : contexts)
			{
				Topology::Ptr pContextTopology = kpContext->Topology();
				TopologyType contextTopologyType = pContextTopology->GetType();
				contextType = contextType | contextTopologyType;

				// Remove content from current contexts
				pContextTopology->RemoveContent(kpSubContent);
				kpSubContent->RemoveContext(kpContext);
			}

			kpTopology2->AddContent(kpSubContent, contextType);
		}
	}

	void Topology::RegularBooleanOperation(
		const TopTools_ListOfShape & rkOcctArgumentsA, 
		const TopTools_ListOfShape & rkOcctArgumentsB, 
		BRepAlgoAPI_BooleanOperation& rOcctBooleanOperation)
	{
		rOcctBooleanOperation.SetArguments(rkOcctArgumentsA);
		rOcctBooleanOperation.SetTools(rkOcctArgumentsB);
		rOcctBooleanOperation.SetNonDestructive(true);
		rOcctBooleanOperation.Build();
	}

	TopoDS_Shape Topology::PostprocessBooleanResult(TopoDS_Shape & rOcctBooleanResult)
	{
		TopoDS_Shape occtPostprocessedShape = Simplify(rOcctBooleanResult);
		if (!occtPostprocessedShape.IsNull())
		{
			occtPostprocessedShape = BooleanSubTopologyContainment(occtPostprocessedShape);
		}
		if (!occtPostprocessedShape.IsNull())
		{
			occtPostprocessedShape = Simplify(occtPostprocessedShape);
		}
	
		return occtPostprocessedShape;
	}

	void Topology::TransferMakeShapeContents(BRepBuilderAPI_MakeShape & rkOcctMakeShape, const std::list<Topology::Ptr>& rkShapes)
	{
		TopTools_ListOfShape occtShapes;
		for (const Topology::Ptr& kpShape : rkShapes)
		{
			occtShapes.Append(kpShape->GetOcctShape());
		}
		TransferMakeShapeContents(rkOcctMakeShape, occtShapes);
	}

	void Topology::TransferMakeShapeContents(BRepBuilderAPI_MakeShape & rkOcctMakeShape, const TopTools_ListOfShape & rkOcctShapes)
	{
		// 1. For each shape in rkOcctShapes, find the generated shapes in rkOcctMakeShape
		for (TopTools_ListIteratorOfListOfShape kOcctShapeIterator(rkOcctShapes);
			kOcctShapeIterator.More();
			kOcctShapeIterator.Next())
		{
			const TopoDS_Shape& rkOcctOriginalShape = kOcctShapeIterator.Value();
			Topology::Ptr pOriginalShape = Topology::ByOcctShape(rkOcctOriginalShape, "");
			TopTools_ListOfShape occtGeneratedShapes = rkOcctMakeShape.Modified(rkOcctOriginalShape);

			// 2. Transfer the contents from the original shape to the generated shapes
			std::list<Topology::Ptr> contents;
			pOriginalShape->Contents(contents);
			for (TopTools_ListIteratorOfListOfShape kOcctGeneratedShapeIterator(occtGeneratedShapes);
				kOcctGeneratedShapeIterator.More();
				kOcctGeneratedShapeIterator.Next())
			{
				const TopoDS_Shape& rkOcctGeneratedShape = kOcctGeneratedShapeIterator.Value();
				Topology::Ptr pGeneratedShape = Topology::ByOcctShape(rkOcctGeneratedShape, "");

				for (const Topology::Ptr& kpContent : contents)
				{
					pGeneratedShape->AddContent(kpContent);
				}
			}
		}
	}

	Topology::Ptr Topology::Impose(const Topology::Ptr & kpTool, const bool kTransferDictionary)
	{
		if (kpTool == nullptr)
		{
			return Topology::ByOcctShape(GetOcctShape(), GetInstanceGUID());
		}

		TopTools_ListOfShape occtArgumentsA;
		TopTools_ListOfShape occtArgumentsB;
		AddBooleanOperands(kpTool, occtArgumentsA, occtArgumentsB);

		BOPAlgo_CellsBuilder occtCellsBuilder;
		NonRegularBooleanOperation(occtArgumentsA, occtArgumentsB, occtCellsBuilder);

		// 2. Select the parts to be included in the final result.
		TopTools_ListOfShape occtListToTake;
		TopTools_ListOfShape occtListToAvoid;

		// Get part of A not in B
		for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorA(occtArgumentsA);
			kOcctShapeIteratorA.More();
			kOcctShapeIteratorA.Next())
		{
			occtListToTake.Clear();
			occtListToAvoid.Clear();
			occtListToTake.Append(kOcctShapeIteratorA.Value());
			for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorB(occtArgumentsB);
				kOcctShapeIteratorB.More();
				kOcctShapeIteratorB.Next())
			{
				occtListToAvoid.Append(kOcctShapeIteratorB.Value());
			}
			occtCellsBuilder.AddToResult(occtListToTake, occtListToAvoid);
		}

		// Add B
		int i = 1;
		for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorB(occtArgumentsB);
			kOcctShapeIteratorB.More();
			kOcctShapeIteratorB.Next())
		{
			occtListToTake.Clear();
			occtListToAvoid.Clear();
			occtListToTake.Append(kOcctShapeIteratorB.Value());
			occtCellsBuilder.AddToResult(occtListToTake, occtListToAvoid, i++, true);
		}

		occtCellsBuilder.MakeContainers();

		TopoDS_Shape occtResultShape = occtCellsBuilder.Shape();
		TopoDS_Shape occtPostprocessedShape = occtResultShape.IsNull() ? occtResultShape : PostprocessBooleanResult(occtResultShape);
		Topology::Ptr pPostprocessedShape = Topology::ByOcctShape(occtPostprocessedShape, "");
		if (pPostprocessedShape == nullptr)
		{
			return nullptr;
		}

		Topology::Ptr pCopyPostprocessedShape = pPostprocessedShape->DeepCopy();
		TransferContents(GetOcctShape(), pCopyPostprocessedShape);
		TransferContents(kpTool->GetOcctShape(), pCopyPostprocessedShape);
		//AttributeManager::GetInstance().DeepCopyAttributes(GetOcctShape(), pCopyPostprocessedShape->GetOcctShape());
		//AttributeManager::GetInstance().DeepCopyAttributes(kpTool->GetOcctShape(), pCopyPostprocessedShape->GetOcctShape());
		if (kTransferDictionary)
		{
			BooleanTransferDictionary(this, kpTool.get(), pCopyPostprocessedShape.get(), true);
		}
		//GlobalCluster::GetInstance().AddTopology(pCopyPostprocessedShape);
		return pCopyPostprocessedShape;
	}

	Topology::Ptr Topology::Imprint(const Topology::Ptr & kpTool, const bool kTransferDictionary)
	{
		if (kpTool == nullptr)
		{
			return Topology::ByOcctShape(GetOcctShape(), GetInstanceGUID());
		}

		TopTools_ListOfShape occtArgumentsA;
		TopTools_ListOfShape occtArgumentsB;
		AddBooleanOperands(kpTool, occtArgumentsA, occtArgumentsB);

		BOPAlgo_CellsBuilder occtCellsBuilder;
		NonRegularBooleanOperation(occtArgumentsA, occtArgumentsB, occtCellsBuilder);

		// 2. Select the parts to be included in the final result.

		TopTools_ListOfShape occtListToTake;
		TopTools_ListOfShape occtListToAvoid;
		for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorA(occtArgumentsA);
			kOcctShapeIteratorA.More();
			kOcctShapeIteratorA.Next())
		{
			for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorB(occtArgumentsB);
				kOcctShapeIteratorB.More();
				kOcctShapeIteratorB.Next())
			{
				occtListToTake.Clear();
				occtListToAvoid.Clear();
				occtListToTake.Append(kOcctShapeIteratorA.Value());
				occtListToTake.Append(kOcctShapeIteratorB.Value());
				occtCellsBuilder.AddToResult(occtListToTake, occtListToAvoid);
			}
		}

		for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorA(occtArgumentsA);
			kOcctShapeIteratorA.More();
			kOcctShapeIteratorA.Next())
		{
			occtListToTake.Clear();
			occtListToAvoid.Clear();
			occtListToTake.Append(kOcctShapeIteratorA.Value());
			occtListToAvoid.Append(occtArgumentsB);
			occtCellsBuilder.AddToResult(occtListToTake, occtListToAvoid);
		}

		occtCellsBuilder.MakeContainers();

		TopoDS_Shape occtResultShape = occtCellsBuilder.Shape();
		TopoDS_Shape occtPostprocessedShape = occtResultShape.IsNull() ? occtResultShape : PostprocessBooleanResult(occtResultShape);
		Topology::Ptr pPostprocessedShape = Topology::ByOcctShape(occtPostprocessedShape, "");
		if (pPostprocessedShape == nullptr)
		{
			return nullptr;
		}
		Topology::Ptr pCopyPostprocessedShape = pPostprocessedShape->DeepCopy();
		TransferContents(GetOcctShape(), pCopyPostprocessedShape);
		TransferContents(kpTool->GetOcctShape(), pCopyPostprocessedShape);
		//AttributeManager::GetInstance().DeepCopyAttributes(GetOcctShape(), pCopyPostprocessedShape->GetOcctShape());
		//AttributeManager::GetInstance().DeepCopyAttributes(kpTool->GetOcctShape(), pCopyPostprocessedShape->GetOcctShape());
		if (kTransferDictionary)
		{
			BooleanTransferDictionary(this, kpTool.get(), pCopyPostprocessedShape.get(), true);
		}
		//GlobalCluster::GetInstance().AddTopology(pCopyPostprocessedShape);
		return pCopyPostprocessedShape;
	}

	Topology::Ptr Topology::Intersect(const Topology::Ptr & kpOtherTopology, const bool kTransferDictionary)
	{
		if (kpOtherTopology == nullptr)
		{
			return Topology::ByOcctShape(GetOcctShape(), GetInstanceGUID());
		}

		// Intersect = Common + Section
		// - Common gives intersection component with at least the same dimension
		// - Section gives the Vertices and Edges
		// NOTE: potential pitfall: 2 Cells intersecting on a Face
		TopTools_ListOfShape occtArgumentsA;
		TopTools_ListOfShape occtArgumentsB;
		AddBooleanOperands(kpOtherTopology, occtArgumentsA, occtArgumentsB);

		BRepAlgoAPI_Common occtCommon;
		RegularBooleanOperation(occtArgumentsA, occtArgumentsB, occtCommon);

		BRepAlgoAPI_Section occtSection;
		RegularBooleanOperation(occtArgumentsA, occtArgumentsB, occtSection);

		// Create topology
		Topology::Ptr commonTopology = Topology::ByOcctShape(occtCommon);
		Topology::Ptr sectionTopology = Topology::ByOcctShape(occtSection);

		// Check isPracticallyEmpty: either nullptr, or (not a Vertex and there is no subtopologies)
		bool isCommonPracticallyEmpty = (commonTopology == nullptr) 
			|| (/*commonTopology->GetType() != TOPOLOGY_VERTEX &&*/ commonTopology->NumOfSubTopologies() == 0);
		bool isSectionPracticallyEmpty = (sectionTopology == nullptr)
			|| (/*sectionTopology->GetType() != TOPOLOGY_VERTEX &&*/ sectionTopology->NumOfSubTopologies() == 0);
		// Cases
		Topology::Ptr mergeTopology = nullptr;
		if (isCommonPracticallyEmpty)
		{
			if (isSectionPracticallyEmpty)
			{
				return nullptr;
			}
			else
			{
				mergeTopology = sectionTopology;
			}
		}
		else
		{
			if (isSectionPracticallyEmpty)
			{
				mergeTopology = commonTopology;
			}
			else
			{
				mergeTopology = commonTopology->Merge(sectionTopology);
			}
		}

		if (mergeTopology == nullptr)
		{
			return nullptr;
		}

		TopoDS_Shape occtResultMergeShape = mergeTopology->GetOcctShape();
		TopoDS_Shape occtPostprocessedShape = occtResultMergeShape.IsNull() ? occtResultMergeShape : PostprocessBooleanResult(occtResultMergeShape);
		Topology::Ptr pPostprocessedShape = Topology::ByOcctShape(occtPostprocessedShape, "");
		if (pPostprocessedShape == nullptr)
		{
			return nullptr;
		}
		Topology::Ptr pCopyPostprocessedShape = pPostprocessedShape->DeepCopy();
		TransferContents(GetOcctShape(), pCopyPostprocessedShape);
		TransferContents(kpOtherTopology->GetOcctShape(), pCopyPostprocessedShape);
		//GlobalCluster::GetInstance().AddTopology(pCopyPostprocessedShape);
		if (kTransferDictionary)
		{
			BooleanTransferDictionary(this, kpOtherTopology.get(), pCopyPostprocessedShape.get(), true);
		}
		return pCopyPostprocessedShape;


		// NOTE: OCCT does not really have a one-size-fits-all intersection solution.
		// Available high level solutions:
		// 1. BRepAlgoAPI_Common --> gives only intersections with at least the same dimension of the input topologies
		//    Reference: 
		// 2. BRepAlgoAPI_Section --> gives only Vertex and Edges intersections
		// 3. CellsBuilder --> non-regular Boolean operations, gives an empty cluster when two Faces intersect on an Edge.

		// Low level solutions (but does not):
		// 1. BOPAlgo_PaveFiller --> this class intersects the, but it does not seem to 
		// For a custom solution, we need methods to determine:
		// 
		//if (kpOtherTopology == nullptr)
		//{
		//	return Topology::ByOcctShape(GetOcctShape(), GetInstanceGUID());
		//}

		//TopTools_ListOfShape occtArgumentsA;
		//TopTools_ListOfShape occtArgumentsB;
		//AddBooleanOperands(kpOtherTopology, occtArgumentsA, occtArgumentsB);

		//BOPAlgo_CellsBuilder occtCellsBuilder;
		//NonRegularBooleanOperation(occtArgumentsA, occtArgumentsB, occtCellsBuilder);

		//// 2. Select the parts to be included in the final result.

		//TopTools_ListOfShape occtListToTake;
		//TopTools_ListOfShape occtListToAvoid;
		//for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorA(occtArgumentsA);
		//	kOcctShapeIteratorA.More();
		//	kOcctShapeIteratorA.Next())
		//{
		//	for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorB(occtArgumentsB);
		//		kOcctShapeIteratorB.More();
		//		kOcctShapeIteratorB.Next())
		//	{
		//		occtListToTake.Clear();
		//		occtListToAvoid.Clear();
		//		occtListToTake.Append(kOcctShapeIteratorA.Value());
		//		occtListToTake.Append(kOcctShapeIteratorB.Value());
		//		occtCellsBuilder.AddToResult(occtListToTake, occtListToAvoid);
		//	}
		//}

		//TopoDS_Shape occtResultShapeA = occtCellsBuilder.Shape();
		//occtCellsBuilder.MakeContainers();
		//
		//TopoDS_Shape occtResultShape = occtCellsBuilder.Shape();
		//TopoDS_Shape occtPostprocessedShape = occtResultShape.IsNull() ? occtResultShape : PostprocessBooleanResult(occtResultShape);

		//// Special case for Edge vs Face
		//if (occtPostprocessedShape.IsNull())
		//{
		//	if (GetType() == TOPOLOGY_FACE && kpOtherTopology->GetType() == TOPOLOGY_EDGE)
		//	{
		//		Topology::Ptr mergeTopology = Merge(kpOtherTopology);
		//		Topology::Ptr result = IntersectEdgeFace(
		//			mergeTopology,
		//			dynamic_cast<TopologicCore::Edge*>(kpOtherTopology.get()),
		//			dynamic_cast<Face*>(this));
		//		occtResultShape = result == nullptr? TopoDS_Shape() : result->GetOcctShape();
		//	}
		//	if (GetType() == TOPOLOGY_EDGE && kpOtherTopology->GetType() == TOPOLOGY_FACE)
		//	{
		//		Topology::Ptr mergeTopology = Merge(kpOtherTopology);
		//		Topology::Ptr result = IntersectEdgeFace(
		//			mergeTopology,
		//			dynamic_cast<TopologicCore::Edge*>(this),
		//			dynamic_cast<Face*>(kpOtherTopology.get()));
		//		occtResultShape = result == nullptr ? TopoDS_Shape() : result->GetOcctShape();
		//	}
		//	if (GetType() == TOPOLOGY_FACE && kpOtherTopology->GetType() == TOPOLOGY_FACE)
		//	{
		//		// TODO
		//		// Need a method to get the Vertex, Edge (straight and curved), and Face (planar and undulating) intersections.

		//		/*Topology::Ptr mergeTopology = Merge(kpOtherTopology);
		//		Topology::Ptr result = IntersectFaceFace(
		//			mergeTopology,
		//			dynamic_cast<TopologicCore::Face*>(this),
		//			dynamic_cast<Face*>(kpOtherTopology.get()));
		//		occtResultShape = result == nullptr ? TopoDS_Shape() : result->GetOcctShape();*/
		//	}
		//	if (GetType() == TOPOLOGY_SHELL && kpOtherTopology->GetType() == TOPOLOGY_EDGE)
		//	{
		//		Topology::Ptr result = IntersectEdgeShell(
		//			dynamic_cast<TopologicCore::Edge*>(kpOtherTopology.get()),
		//			dynamic_cast<Shell*>(this));
		//		occtResultShape = result == nullptr ? TopoDS_Shape() : result->GetOcctShape();
		//	}
		//	if (GetType() == TOPOLOGY_EDGE && kpOtherTopology->GetType() == TOPOLOGY_SHELL)
		//	{
		//		Topology::Ptr result = IntersectEdgeShell(
		//			dynamic_cast<TopologicCore::Edge*>(this),
		//			dynamic_cast<Shell*>(kpOtherTopology.get()));
		//		occtResultShape = result == nullptr ? TopoDS_Shape() : result->GetOcctShape();
		//	}

		//	occtPostprocessedShape = occtResultShape.IsNull() ? occtResultShape : PostprocessBooleanResult(occtResultShape);
		//}

		//Topology::Ptr pPostprocessedShape = Topology::ByOcctShape(occtPostprocessedShape, "");
		//if (pPostprocessedShape == nullptr)
		//{
		//	return pPostprocessedShape;
		//}
		//Topology::Ptr pCopyPostprocessedShape = pPostprocessedShape->DeepCopy();
		//
		//TransferContents(GetOcctShape(), pCopyPostprocessedShape);
		//TransferContents(kpOtherTopology->GetOcctShape(), pCopyPostprocessedShape);
		////AttributeManager::GetInstance().DeepCopyAttributes(GetOcctShape(), pCopyPostprocessedShape->GetOcctShape());
		////AttributeManager::GetInstance().DeepCopyAttributes(kpOtherTopology->GetOcctShape(), pCopyPostprocessedShape->GetOcctShape());
		//if (kTransferDictionary)
		//{
		//	BooleanTransferDictionary(this, kpOtherTopology.get(), pCopyPostprocessedShape.get(), true);
		//}
		//GlobalCluster::GetInstance().AddTopology(pCopyPostprocessedShape);
		//return pCopyPostprocessedShape;
	}

	Topology::Ptr Topology::Merge(const Topology::Ptr & kpOtherTopology, const bool kTransferDictionary)
	{
		if (kpOtherTopology == nullptr)
		{
			return Topology::ByOcctShape(GetOcctShape(), GetInstanceGUID());
		}

		TopTools_ListOfShape occtArgumentsA;
		TopTools_ListOfShape occtArgumentsB;
		AddBooleanOperands(kpOtherTopology, occtArgumentsA, occtArgumentsB);

		BOPAlgo_CellsBuilder occtCellsBuilder;
		NonRegularBooleanOperation(occtArgumentsA, occtArgumentsB, occtCellsBuilder);

		// 2. Select the parts to be included in the final result.
		TopTools_ListOfShape occtListToTake;
		TopTools_ListOfShape occtListToAvoid;

		for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorA(occtArgumentsA);
			kOcctShapeIteratorA.More();
			kOcctShapeIteratorA.Next())
		{
			occtListToTake.Clear();
			occtListToAvoid.Clear();
			occtListToTake.Append(kOcctShapeIteratorA.Value());
			occtCellsBuilder.AddToResult(occtListToTake, occtListToAvoid);
		}

		for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorB(occtArgumentsB);
			kOcctShapeIteratorB.More();
			kOcctShapeIteratorB.Next())
		{
			occtListToTake.Clear();
			occtListToAvoid.Clear();
			occtListToTake.Append(kOcctShapeIteratorB.Value());
			occtCellsBuilder.AddToResult(occtListToTake, occtListToAvoid);
		}

		occtCellsBuilder.MakeContainers();

		TopoDS_Shape occtResultShape = occtCellsBuilder.Shape();
		TopoDS_Shape occtPostprocessedShape = occtResultShape.IsNull() ? occtResultShape : PostprocessBooleanResult(occtResultShape);
		Topology::Ptr pPostprocessedShape = Topology::ByOcctShape(occtPostprocessedShape, "");
		if (pPostprocessedShape == nullptr)
		{
			return nullptr;
		}
		Topology::Ptr pCopyPostprocessedShape = pPostprocessedShape->DeepCopy();
		TransferContents(GetOcctShape(), pCopyPostprocessedShape);
		TransferContents(kpOtherTopology->GetOcctShape(), pCopyPostprocessedShape);

		//GlobalCluster::GetInstance().AddTopology(pCopyPostprocessedShape);
		//AttributeManager::GetInstance().DeepCopyAttributes(GetOcctShape(), pCopyPostprocessedShape->GetOcctShape());
		//AttributeManager::GetInstance().DeepCopyAttributes(kpOtherTopology->GetOcctShape(), pCopyPostprocessedShape->GetOcctShape());
		if (kTransferDictionary)
		{
			BooleanTransferDictionary(this, kpOtherTopology.get(), pCopyPostprocessedShape.get(), true);
		}
		return pCopyPostprocessedShape;
	}

	Topology::Ptr Topology::SelfMerge()
	{
		// 1
		TopTools_ListOfShape occtShapes;
		SubTopologies(GetOcctShape(), occtShapes);

		// 2
		BOPAlgo_CellsBuilder occtCellsBuilder;
		occtCellsBuilder.SetArguments(occtShapes);

		try {
			occtCellsBuilder.Perform();
		}
		catch (Standard_Failure& e)
		{
			const char* str = e.GetMessageString();
			std::string stlStr(str);
		}
		catch (std::runtime_error& e)
		{
			const char* str = e.what();
			std::string stlStr(str);
		}

		if (occtCellsBuilder.HasErrors() || occtCellsBuilder.HasWarnings())
		{
			std::ostringstream errorStream;
			occtCellsBuilder.DumpErrors(errorStream);
			std::ostringstream warningStream;
			occtCellsBuilder.DumpWarnings(warningStream);

			// Exit here and return occtShapes as a cluster.
			TopoDS_Compound occtCompound;
			BRep_Builder occtBuilder;
			occtBuilder.MakeCompound(occtCompound);
			for (TopTools_ListIteratorOfListOfShape occtShapeIterator(occtShapes);
				occtShapeIterator.More();
				occtShapeIterator.Next())
			{
				occtBuilder.Add(occtCompound, occtShapeIterator.Value());
			}
			return Topology::ByOcctShape(occtCompound, "");
		}
		occtCellsBuilder.AddAllToResult();

		// 2b. Get discarded faces from Cells Builder
		TopTools_ListOfShape occtDiscardedFaces;
		TopoDS_Compound occtCompound;
		BRep_Builder occtBuilder;
		occtBuilder.MakeCompound(occtCompound);
		for (TopTools_ListIteratorOfListOfShape occtFaceIterator(occtShapes);
			occtFaceIterator.More();
			occtFaceIterator.Next())
		{
			TopoDS_Shape& rCurrent = occtFaceIterator.Value();
			if (occtCellsBuilder.IsDeleted(rCurrent))
			{
				occtBuilder.Add(occtCompound, rCurrent);
				occtDiscardedFaces.Append(rCurrent);
			}
		}

		// 3. Get Face[] from Topology[]
		TopTools_ListOfShape occtFaces;
		TopoDS_Compound occtCompound3;
		BRep_Builder occtBuilder3;
		occtBuilder3.MakeCompound(occtCompound3);
		for (TopExp_Explorer occtExplorer(occtCellsBuilder.Shape(), TopAbs_FACE); occtExplorer.More(); occtExplorer.Next())
		{
			const TopoDS_Shape& rkOcctCurrent = occtExplorer.Current();
			if (!occtFaces.Contains(rkOcctCurrent))
			{
				occtFaces.Append(rkOcctCurrent);
				occtBuilder3.Add(occtCompound3, rkOcctCurrent);
			}
		}

		// 5. Topology = VolumeMaker(Face[])--> first result
		BOPAlgo_MakerVolume occtVolumeMaker;
		Standard_Boolean bRunParallel = Standard_False; /* parallel or single mode (the default value is FALSE)*/
		Standard_Boolean bIntersect = Standard_True; /* intersect or not the arguments (the default value is TRUE)*/
		Standard_Real aTol = 0.0; /* fuzzy option (default value is 0)*/
								  //
		occtVolumeMaker.SetArguments(occtFaces);
		occtVolumeMaker.SetRunParallel(bRunParallel);
		occtVolumeMaker.SetIntersect(bIntersect);
		occtVolumeMaker.SetFuzzyValue(aTol);
		//
		occtVolumeMaker.Perform(); //perform the operation

		if (occtVolumeMaker.HasErrors() || occtVolumeMaker.HasWarnings()) { //check error status
			
		}else
		{
			// 6. Get discarded faces from VolumeMaker--> second result
			TopoDS_Compound occtCompound2;
			BRep_Builder occtBuilder2;
			occtBuilder2.MakeCompound(occtCompound2);
			for (TopTools_ListIteratorOfListOfShape occtFaceIterator(occtFaces);
				occtFaceIterator.More();
				occtFaceIterator.Next())
			{
				TopoDS_Shape& rCurrent = occtFaceIterator.Value();
				if (occtVolumeMaker.IsDeleted(rCurrent))
				{
					occtDiscardedFaces.Append(rCurrent);
					occtBuilder2.Add(occtCompound2, rCurrent);
				}
			}
		}

		// 7. Get the rest from Topology[] --> third result
		TopTools_ListOfShape occtOtherShapes; // for step #7
		for (TopTools_ListIteratorOfListOfShape occtShapeIterator(occtShapes);
			occtShapeIterator.More();
			occtShapeIterator.Next())
		{
			if (occtShapeIterator.Value().ShapeType() != TopAbs_FACE)
			{
				occtOtherShapes.Append(occtShapeIterator.Value());
			}
		}

		// 8. Merge results #1 #2 #3
		TopTools_ListOfShape occtFinalArguments;
		if (!occtVolumeMaker.HasErrors() && !occtVolumeMaker.HasWarnings()) { //check error status
			occtFinalArguments.Append(occtVolumeMaker.Shape());
		}
		occtFinalArguments.Append(occtDiscardedFaces);
		occtFinalArguments.Append(occtOtherShapes);

		if (occtFinalArguments.Size() == 1)
		{
			return Topology::ByOcctShape(occtVolumeMaker.Shape(), "");
		}
		BOPAlgo_CellsBuilder occtCellsBuilder2;
		occtCellsBuilder2.SetArguments(occtFinalArguments);
		try {
			occtCellsBuilder2.Perform();
		}
		catch (Standard_Failure& e)
		{
			const char* str = e.GetMessageString();
			std::string stlStr(str);
		}
		catch (std::runtime_error& e)
		{
			const char* str = e.what();
			std::string stlStr(str);
		}

		if (occtCellsBuilder2.HasErrors())
		{
			std::ostringstream errorStream;
			occtCellsBuilder2.DumpErrors(errorStream);
			throw std::runtime_error(errorStream.str().c_str());
		}

		occtCellsBuilder2.AddAllToResult();
		occtCellsBuilder2.MakeContainers();

		// 9. If there is still a discarded face, add to merge2Topology as a cluster.
		TopTools_ListOfShape clusterCandidates;
		for (TopTools_ListIteratorOfListOfShape merge2Topologies(occtFinalArguments);
			merge2Topologies.More();
			merge2Topologies.Next())
		{
			if (occtCellsBuilder2.IsDeleted(merge2Topologies.Value()) &&
				merge2Topologies.Value().ShapeType() == TopAbs_FACE) // currently only face
			{
				TopTools_ListOfShape modifiedShapes = occtCellsBuilder2.Modified(merge2Topologies.Value());
				TopTools_ListOfShape generatedShapes = occtCellsBuilder2.Generated(merge2Topologies.Value());
				clusterCandidates.Append(merge2Topologies.Value());
			}
		}

		TopoDS_Shape occtFinalResult;
		if (clusterCandidates.Size() > 0)
		{
			SubTopologies(occtCellsBuilder2.Shape(), clusterCandidates);
			TopoDS_Compound occtFinalCompound;
			BRep_Builder occtFinalBuilder;
			occtFinalBuilder.MakeCompound(occtFinalCompound);
			for (TopTools_ListIteratorOfListOfShape clusterCandidateIterator(clusterCandidates);
				clusterCandidateIterator.More();
				clusterCandidateIterator.Next())
			{
				occtFinalBuilder.Add(occtFinalCompound, clusterCandidateIterator.Value());
			}

			occtFinalResult = occtFinalCompound;
		}
		else
		{
			TopoDS_Shape occtCurrentShape = occtCellsBuilder2.Shape();
			TopoDS_Shape occtPostprocessedShape = occtCurrentShape.IsNull() ? occtCurrentShape : PostprocessBooleanResult(occtCurrentShape);
			occtFinalResult = occtPostprocessedShape;
		}

		// Shape fix
		ShapeFix_Shape occtShapeFix(occtFinalResult);
		occtShapeFix.Perform();
		TopoDS_Shape fixedFinalShape = occtShapeFix.Shape();

		Topology::Ptr finalTopology = Topology::ByOcctShape(fixedFinalShape, "")->DeepCopy();

		// Copy dictionaries
		AttributeManager::GetInstance().DeepCopyAttributes(GetOcctShape(), finalTopology->GetOcctShape());

		// Copy contents
		TransferContents(GetOcctShape(), finalTopology);

		return finalTopology;
	}

	Topology::Ptr Topology::Slice(const Topology::Ptr & kpTool, const bool kTransferDictionary)
	{
		if (kpTool == nullptr)
		{
			return Topology::ByOcctShape(GetOcctShape(), GetInstanceGUID());
		}

		TopTools_ListOfShape occtArgumentsA;
		TopTools_ListOfShape occtArgumentsB;
		AddBooleanOperands(kpTool, occtArgumentsA, occtArgumentsB);

		BOPAlgo_CellsBuilder occtCellsBuilder;
		NonRegularBooleanOperation(occtArgumentsA, occtArgumentsB, occtCellsBuilder);

		// 2. Select the parts to be included in the final result.
		TopTools_ListOfShape occtListToTake;
		TopTools_ListOfShape occtListToAvoid;

		for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorA(occtArgumentsA);
			kOcctShapeIteratorA.More();
			kOcctShapeIteratorA.Next())
		{
			occtListToTake.Clear();
			occtListToAvoid.Clear();
			occtListToTake.Append(kOcctShapeIteratorA.Value());
			occtCellsBuilder.AddToResult(occtListToTake, occtListToAvoid);
		}

		occtCellsBuilder.MakeContainers();

		TopoDS_Shape occtResultShape = occtCellsBuilder.Shape();
		TopoDS_Shape occtPostprocessedShape = occtResultShape.IsNull() ? occtResultShape : PostprocessBooleanResult(occtResultShape);
		Topology::Ptr pPostprocessedShape = Topology::ByOcctShape(occtPostprocessedShape, "");
		if (pPostprocessedShape == nullptr)
		{
			return nullptr;
		}
		Topology::Ptr pCopyPostprocessedShape = pPostprocessedShape->DeepCopy();
		TransferContents(GetOcctShape(), pCopyPostprocessedShape);
		//AttributeManager::GetInstance().DeepCopyAttributes(GetOcctShape(), occtPostprocessedShape);
		//GlobalCluster::GetInstance().AddTopology(pCopyPostprocessedShape);
		if (kTransferDictionary)
		{
			BooleanTransferDictionary(this, kpTool.get(), pCopyPostprocessedShape.get(), true);
		}
		return pCopyPostprocessedShape;
	}

	Topology::Ptr Topology::Union(const Topology::Ptr & kpOtherTopology, const bool kTransferDictionary)
	{
		if (kpOtherTopology == nullptr)
		{
			return Topology::ByOcctShape(GetOcctShape(), GetInstanceGUID());
		}

		TopTools_ListOfShape occtArgumentsA;
		TopTools_ListOfShape occtArgumentsB;
		AddBooleanOperands(kpOtherTopology, occtArgumentsA, occtArgumentsB);

		BRepAlgoAPI_Fuse occtFuse;
		RegularBooleanOperation(occtArgumentsA, occtArgumentsB, occtFuse);

		TopoDS_Shape occtResultShape = occtFuse.Shape();
		TopoDS_Shape occtPostprocessedShape = occtResultShape.IsNull() ? occtResultShape : PostprocessBooleanResult(occtResultShape);
		Topology::Ptr pPostprocessedShape = Topology::ByOcctShape(occtPostprocessedShape, "");
		if (pPostprocessedShape == nullptr)
		{
			return nullptr;
		}
		Topology::Ptr pCopyPostprocessedShape = pPostprocessedShape->DeepCopy();
		TransferContents(GetOcctShape(), pCopyPostprocessedShape);
		TransferContents(kpOtherTopology->GetOcctShape(), pCopyPostprocessedShape);
		//AttributeManager::GetInstance().DeepCopyAttributes(GetOcctShape(), pCopyPostprocessedShape->GetOcctShape());
		//AttributeManager::GetInstance().DeepCopyAttributes(kpOtherTopology->GetOcctShape(), pCopyPostprocessedShape->GetOcctShape());
		//GlobalCluster::GetInstance().AddTopology(pCopyPostprocessedShape);
		if (kTransferDictionary)
		{
			BooleanTransferDictionary(this, kpOtherTopology.get(), pCopyPostprocessedShape.get(), true);
		}
		return pCopyPostprocessedShape;
	}

	TopoDS_Shape Topology::FixBooleanOperandCell(const TopoDS_Shape& rkOcctShape)
	{
		TopTools_MapOfShape occtCells;
		TopoDS_Shape occtNewShape(rkOcctShape);
		for (TopExp_Explorer occtExplorer(rkOcctShape, TopAbs_SOLID); occtExplorer.More(); occtExplorer.Next())
		{
			const TopoDS_Solid &occtCurrentSolid = TopoDS::Solid(occtExplorer.Current());
			//create tools for fixing a face 
			Handle(ShapeFix_Solid) occtShapeFixSolid = new ShapeFix_Solid();

			// create tool for rebuilding a shape and initialize it by shape 
			Handle(ShapeBuild_ReShape) occtContext = new ShapeBuild_ReShape();
			occtContext->Apply(occtNewShape);

			//set a tool for rebuilding a shape in the tool for fixing 
			occtShapeFixSolid->SetContext(occtContext);

			//initialize the fixing tool by one face 
			occtShapeFixSolid->Init(occtCurrentSolid);
			//fix the set face 
			occtShapeFixSolid->Perform();

			//get the result 
			occtNewShape = occtContext->Apply(occtNewShape);
		}

		return occtNewShape;
	}
    void Topology::AddUnionInternalStructure(const TopoDS_Shape& rkOcctShape, TopTools_ListOfShape& rUnionArguments)
    {
        TopAbs_ShapeEnum occtShapeType = rkOcctShape.ShapeType();
        Topology::Ptr pTopology = Topology::ByOcctShape(rkOcctShape, "");
        std::list<Face::Ptr> faces;
        // Cell complex -> faces not part of the envelope
        // Cell -> inner shells
        // Shell --> inner wires of the faces
        // Face --> inner wires
        // Wire --> n/a
        // Edge --> n/a
        // Vertex --> n/a
        if (occtShapeType == TopAbs_COMPOUND)
        {
            std::shared_ptr<Cluster> pCluster = Topology::Downcast<Cluster>(pTopology);
            std::list<Topology::Ptr> immediateMembers;
            pCluster->SubTopologies(immediateMembers);
            for (const Topology::Ptr& kpImmediateMember : immediateMembers)
            {
                AddUnionInternalStructure(kpImmediateMember->GetOcctShape(), rUnionArguments);
            }
        }
        else if (occtShapeType == TopAbs_COMPSOLID)
        {
            CellComplex::Ptr pCellComplex = Topology::Downcast<CellComplex>(pTopology);
            pCellComplex->InternalBoundaries(faces);
            for (const Face::Ptr& kpInternalFace : faces)
            {
                rUnionArguments.Append(kpInternalFace->GetOcctShape());
            }
        }
        else if (occtShapeType == TopAbs_SOLID)
        {
            Cell::Ptr pCell = Topology::Downcast<Cell>(pTopology);
            std::list<Shell::Ptr> shells;
            pCell->InternalBoundaries(shells);
            for (const Shell::Ptr& kpInternalShell : shells)
            {
                rUnionArguments.Append(kpInternalShell->GetOcctShape());
            }
        }
        else if (occtShapeType == TopAbs_SHELL)
        {
            for (TopExp_Explorer occtShellExplorer(rkOcctShape, TopAbs_FACE); occtShellExplorer.More(); occtShellExplorer.Next())
            {
                const TopoDS_Shape& rkOcctCurrentFace = occtShellExplorer.Current();
                TopoDS_Wire occtOuterWire = BRepTools::OuterWire(TopoDS::Face(rkOcctCurrentFace));

                for (TopExp_Explorer occtFaceExplorer(rkOcctShape, TopAbs_WIRE); occtFaceExplorer.More(); occtFaceExplorer.Next())
                {
                    const TopoDS_Shape& rkOcctCurrentFace = occtFaceExplorer.Current();
                    if (!rUnionArguments.Contains(rkOcctCurrentFace) && !rkOcctCurrentFace.IsSame(occtOuterWire))
                    {
                        rUnionArguments.Append(rkOcctCurrentFace);
                    }
                }
            }
        }
        else if (occtShapeType == TopAbs_FACE)
        {
            TopoDS_Wire occtOuterWire = BRepTools::OuterWire(TopoDS::Face(rkOcctShape));

            for (TopExp_Explorer occtExplorer(rkOcctShape, TopAbs_WIRE); occtExplorer.More(); occtExplorer.Next())
            {
                const TopoDS_Shape& occtCurrent = occtExplorer.Current();
                if (!rUnionArguments.Contains(occtCurrent) && !occtCurrent.IsSame(occtOuterWire))
                {
                    rUnionArguments.Append(occtCurrent);
                }
            }
        }
    }
	TopoDS_Shape Topology::FixBooleanOperandShell(const TopoDS_Shape& rkOcctShape)
	{
		TopTools_MapOfShape occtCells;
		TopoDS_Shape occtNewShape(rkOcctShape);
		for (TopExp_Explorer occtExplorer(rkOcctShape, TopAbs_SHELL); occtExplorer.More(); occtExplorer.Next())
		{
			const TopoDS_Shell& rkOcctCurrentShell = TopoDS::Shell(occtExplorer.Current());
			//create tools for fixing a face 
			Handle(ShapeFix_Shell) occtShapeFixShell = new ShapeFix_Shell();

			// create tool for rebuilding a shape and initialize it by shape 
			Handle(ShapeBuild_ReShape) occtContext = new ShapeBuild_ReShape();
			occtContext->Apply(occtNewShape);

			//set a tool for rebuilding a shape in the tool for fixing 
			occtShapeFixShell->SetContext(occtContext);

			//initialize the fixing tool by one face 
			occtShapeFixShell->Init(rkOcctCurrentShell);
			//fix the set face 
			occtShapeFixShell->Perform();

			//get the result 
			occtNewShape = occtContext->Apply(occtNewShape);
		}

		return occtNewShape;
	}

	TopoDS_Shape Topology::FixBooleanOperandFace(const TopoDS_Shape& rkOcctShape, TopTools_DataMapOfShapeShape& rMapFaceToFixedFace)
	{
		TopTools_MapOfShape occtCells;
		TopoDS_Shape occtNewShape(rkOcctShape);
		for (TopExp_Explorer occtExplorer(rkOcctShape, TopAbs_FACE); occtExplorer.More(); occtExplorer.Next())
		{
			const TopoDS_Face& occtCurrentFace = TopoDS::Face(occtExplorer.Current());

			//create tools for fixing a face 
			Handle(ShapeFix_Face) occtShapeFixFace = new ShapeFix_Face();

			// create tool for rebuilding a shape and initialize it by shape 
			Handle(ShapeBuild_ReShape) occtContext = new ShapeBuild_ReShape();
			occtContext->Apply(occtNewShape);

			//set a tool for rebuilding a shape in the tool for fixing 
			occtShapeFixFace->SetContext(occtContext);

			//initialize the fixing tool by one face 
			occtShapeFixFace->Init(occtCurrentFace);
			//fix the set face 
			occtShapeFixFace->Perform();

			// Map occtCurrentFace and occtShapeFixFace.Shape()
			rMapFaceToFixedFace.Bind(occtCurrentFace, occtShapeFixFace->Face());

			//get the result 
			occtNewShape = occtContext->Apply(occtNewShape);
		}

		return occtNewShape;
	}

	TopoDS_Shape Topology::FixBooleanOperandFace(const TopoDS_Shape & rkOcctShape)
	{
		TopTools_DataMapOfShapeShape mapFaceToFixedFace;
		return FixBooleanOperandFace(rkOcctShape, mapFaceToFixedFace);
	}

	void Topology::GetDeletedBooleanSubtopologies(const TopoDS_Shape& rkOcctShape, BOPAlgo_CellsBuilder & rOcctCellsBuilder, TopTools_ListOfShape & rOcctDeletedShapes)
	{
		TopAbs_ShapeEnum subshapeTypes[3] = {TopAbs_VERTEX, TopAbs_EDGE, TopAbs_FACE};
		for (int i = 0; i < 3; ++i)
		{
			if (rkOcctShape.ShapeType() == subshapeTypes[i])
			{
				if (rOcctCellsBuilder.IsDeleted(rkOcctShape))
				{
					rOcctDeletedShapes.Append(rkOcctShape);
				}

				const TopTools_ListOfShape& rkOcctModifiedShapes = rOcctCellsBuilder.Modified(rkOcctShape);
				if (!rkOcctModifiedShapes.IsEmpty())
				{
					rOcctDeletedShapes.Append(rkOcctShape);
				}
			}

			TopTools_MapOfShape occtSubshapes;
			DownwardNavigation(rkOcctShape, subshapeTypes[i], occtSubshapes);
			for (TopTools_MapIteratorOfMapOfShape occtSubshapeIterator(occtSubshapes);
				occtSubshapeIterator.More();
				occtSubshapeIterator.Next())
			{
				if (rOcctCellsBuilder.IsDeleted(occtSubshapeIterator.Value()))
				{
					rOcctDeletedShapes.Append(occtSubshapeIterator.Value());
				}
				const TopTools_ListOfShape& rkOcctModifiedShapes = rOcctCellsBuilder.Modified(occtSubshapeIterator.Value());
				if (!rkOcctModifiedShapes.IsEmpty())
				{
					rOcctDeletedShapes.Append(occtSubshapeIterator.Value());
				}
			}
		}
	}

	void Topology::GetDeletedBooleanSubtopologies(const TopoDS_Shape& rkOcctShape, BRepAlgoAPI_BooleanOperation & rOcctBooleanOperation, TopTools_ListOfShape & rOcctDeletedShapes)
	{
		TopAbs_ShapeEnum subshapeTypes[3] = { TopAbs_VERTEX, TopAbs_EDGE, TopAbs_FACE };
		for (int i = 0; i < 3; ++i)
		{
			if (rkOcctShape.ShapeType() == subshapeTypes[i])
			{
				if (rOcctBooleanOperation.IsDeleted(rkOcctShape))
				{
					rOcctDeletedShapes.Append(rkOcctShape);
				}
			}

			TopTools_MapOfShape occtSubshapes;
			DownwardNavigation(rkOcctShape, subshapeTypes[i], occtSubshapes);
			for (TopTools_MapIteratorOfMapOfShape occtSubshapeIterator(occtSubshapes);
				occtSubshapeIterator.More();
				occtSubshapeIterator.Next())
			{
				if (rOcctBooleanOperation.IsDeleted(occtSubshapeIterator.Value()))
				{
					rOcctDeletedShapes.Append(occtSubshapeIterator.Value());
				}
			}
		}
	}

	Topology::Ptr Topology::TrackContextAncestor()
	{
		std::list<Context::Ptr> contexts;
		Contexts(contexts);

		if (contexts.size() == 1)
		{
			// Go farther
			return (*contexts.begin())->Topology()->TrackContextAncestor();
		}

		// If empty or > 2
		return shared_from_this();
	}

	Topology::Ptr Topology::IntersectEdgeShell(Edge * const kpkEdge, Shell const * const kpkShell)
	{
		std::list<Face::Ptr> faces;
		kpkShell->Faces(nullptr, faces);

		std::list<Topology::Ptr> intersectionVertices;
		for (const Face::Ptr& kpFace : faces)
		{
			Topology::Ptr mergeTopology = kpkEdge->Merge(kpFace);
			Topology::Ptr cluster = IntersectEdgeFace(mergeTopology, kpkEdge, kpFace.get());
			if (cluster == nullptr)
			{
				continue;
			}
			std::list<Vertex::Ptr> clusterVertices;
			cluster->Vertices(nullptr, clusterVertices);
			intersectionVertices.insert(intersectionVertices.end(), clusterVertices.begin(), clusterVertices.end());
		}

		Cluster::Ptr cluster = Cluster::ByTopologies(intersectionVertices);
		Topology::Ptr mergedCluster = cluster->SelfMerge();
		return mergedCluster;
	}

	bool IsInList(const Vertex::Ptr kpNewVertex, const std::list<Vertex::Ptr>& rkOldVertices, const double kTolerance)
	{
		for (const Vertex::Ptr& kpOldVertex : rkOldVertices)
		{
			BRepExtrema_DistShapeShape occtEdgeDistance(kpOldVertex->GetOcctShape(), kpNewVertex->GetOcctShape(), Extrema_ExtFlag_MINMAX);
			double distance = occtEdgeDistance.Value();

			if (distance < kTolerance)
			{
				return true;
			}
		}

		return false;
	}

	Topology::Ptr Topology::IntersectEdgeFace(const Topology::Ptr kpMergeTopology, Edge const * const kpkEdge, Face const * const kpkFace)
	{
		double kTolerance = 0.0001;
		std::list<Vertex::Ptr> edgeVertices;
		kpkEdge->Vertices(nullptr, edgeVertices);

		std::list<Vertex::Ptr> faceVertices;
		kpkFace->Vertices(nullptr, faceVertices);

		std::list<Vertex::Ptr> mergeVertices;
		kpMergeTopology->Vertices(nullptr, mergeVertices);

		std::list<Topology::Ptr> intersectionVertices;
		for (const Vertex::Ptr &kpMergeVertex : mergeVertices)
		{
			bool isInEdgeVertices = IsInList(kpMergeVertex, edgeVertices, kTolerance);
			bool isInFaceVertices = IsInList(kpMergeVertex, faceVertices, kTolerance);
			if ((!isInEdgeVertices && !isInFaceVertices) ||
				(isInEdgeVertices && isInFaceVertices))
			{
				intersectionVertices.push_back(kpMergeVertex);
			}
			else
			{
				BRepExtrema_DistShapeShape occtEdgeDistance(kpMergeVertex->GetOcctShape(), kpkEdge->GetOcctEdge(), Extrema_ExtFlag_MINMAX);
				double edgeDistance = occtEdgeDistance.Value();

				BRepExtrema_DistShapeShape occtFaceDistance(kpMergeVertex->GetOcctShape(), kpkFace->GetOcctFace(), Extrema_ExtFlag_MINMAX);
				double faceDistance = occtFaceDistance.Value();

				if (edgeDistance < kTolerance && faceDistance < kTolerance)
				{
					intersectionVertices.push_back(kpMergeVertex);
				}
			}
		}

		Cluster::Ptr cluster = Cluster::ByTopologies(intersectionVertices);
		return cluster;
	}

	Topology::Ptr Topology::IntersectFaceFace(const Topology::Ptr kpMergeTopology, Face const* const kpkFace, Face const* const kpkOtherFace)
	{
		// OCCT does not seem to have a robust Face-Face 
		throw std::runtime_error("Not yet implemented");
	}

	void Topology::AddBooleanOperands(
		const Topology::Ptr& kpOtherTopology,
		BOPAlgo_CellsBuilder& rOcctCellsBuilder,
		TopTools_ListOfShape& rOcctCellsBuildersOperandsA,
		TopTools_ListOfShape& rOcctCellsBuildersOperandsB,
		TopTools_DataMapOfShapeShape& rOcctMapFaceToFixedFaceA,
		TopTools_DataMapOfShapeShape& rOcctMapFaceToFixedFaceB)
	{
		// Buffer lists are currently provided for cell complexes to be processed per cells.
		TopTools_ListOfShape occtCellsBuildersArguments;
		TopologyType typeA = GetType();
		if (typeA == TOPOLOGY_CELLCOMPLEX || typeA == TOPOLOGY_CLUSTER)
		{
			std::list<Topology::Ptr> members;
			SubTopologies(members);
			for (const Topology::Ptr& kpMember : members)
			{
				/*TopoDS_Shape occtNewShape = FixBooleanOperandFace(kpMember->GetOcctShape(), rOcctMapFaceToFixedFaceA);
				occtNewShape = FixBooleanOperandShell(kpMember->GetOcctShape());
				occtNewShape = FixBooleanOperandCell(kpMember->GetOcctShape());*/
				rOcctCellsBuildersOperandsA.Append(kpMember->GetOcctShape());
				occtCellsBuildersArguments.Append(kpMember->GetOcctShape());
			}
		}
		else
		{
			/*TopTools_DataMapOfShapeShape occtMapFaceToFixedFace;
			TopoDS_Shape occtNewShape = FixBooleanOperandFace(GetOcctShape(), rOcctMapFaceToFixedFaceA);
			occtNewShape = FixBooleanOperandShell(occtNewShape);
			occtNewShape = FixBooleanOperandCell(occtNewShape);*/
			rOcctCellsBuildersOperandsA.Append(GetOcctShape());
			occtCellsBuildersArguments.Append(GetOcctShape());
		}

		TopologyType typeB = kpOtherTopology->GetType();
		if (typeB == TOPOLOGY_CELLCOMPLEX || typeB == TOPOLOGY_CLUSTER)
		{
			std::list<Topology::Ptr> members;
			kpOtherTopology->SubTopologies(members);
			for (const Topology::Ptr& kpMember : members)
			{
				/*TopoDS_Shape occtNewShape = FixBooleanOperandFace(kpMember->GetOcctShape(), rOcctMapFaceToFixedFaceA);
				occtNewShape = FixBooleanOperandShell(kpMember->GetOcctShape());
				occtNewShape = FixBooleanOperandCell(kpMember->GetOcctShape());*/
				rOcctCellsBuildersOperandsB.Append(kpMember->GetOcctShape());
				occtCellsBuildersArguments.Append(kpMember->GetOcctShape());
			}
		}
		else
		{
			/*TopoDS_Shape occtNewShape = FixBooleanOperandFace(kpOtherTopology->GetOcctShape(), rOcctMapFaceToFixedFaceB);
			occtNewShape = FixBooleanOperandShell(kpOtherTopology->GetOcctShape());
			occtNewShape = FixBooleanOperandCell(kpOtherTopology->GetOcctShape());*/
			rOcctCellsBuildersOperandsB.Append(kpOtherTopology->GetOcctShape());
			occtCellsBuildersArguments.Append(kpOtherTopology->GetOcctShape());
		}

		rOcctCellsBuilder.SetArguments(occtCellsBuildersArguments);
	}

	void Topology::AddBooleanOperands(const Topology::Ptr & kpOtherTopology, TopTools_ListOfShape & rOcctCellsBuildersOperandsA, TopTools_ListOfShape & rOcctCellsBuildersOperandsB)
	{
		{
			//TopTools_ListOfShape occtOperandsA;
			if (IsContainerType())
			{
				std::list<Topology::Ptr> subTopologies;
				SubTopologies(subTopologies);
				for (const Topology::Ptr &kpTopology: subTopologies)
				{
                    rOcctCellsBuildersOperandsA.Append(kpTopology->GetOcctShape());
				}
			}
			else
			{
                rOcctCellsBuildersOperandsA.Append(GetOcctShape());
			}

			//for (TopTools_ListIteratorOfListOfShape occtOperandAIterator(occtOperandsA);
			//	occtOperandAIterator.More();
			//	occtOperandAIterator.Next())
			//{
			//	/*TopoDS_Shape occtNewShape = FixBooleanOperandFace(occtOperandAIterator.Value());
			//	occtNewShape = FixBooleanOperandShell(occtNewShape);
			//	occtNewShape = FixBooleanOperandCell(occtNewShape);
			//	rOcctCellsBuildersOperandsA.Append(occtNewShape);*/
   //             rOcctCellsBuildersOperandsA.Append(occtOperandAIterator.Value());
			//}
		}

		{
			//TopTools_ListOfShape occtOperandsB;
			if (kpOtherTopology->IsContainerType())
			{
				std::list<Topology::Ptr> subTopologies;
				kpOtherTopology->SubTopologies(subTopologies);
				for (const Topology::Ptr &kpTopology : subTopologies)
				{
                    rOcctCellsBuildersOperandsB.Append(kpTopology->GetOcctShape());
				}
			}
			else
			{
                rOcctCellsBuildersOperandsB.Append(kpOtherTopology->GetOcctShape());
			}

			//for (TopTools_ListIteratorOfListOfShape occtOperandBIterator(occtOperandsB);
			//	occtOperandBIterator.More();
			//	occtOperandBIterator.Next())
			//{
			//	/*TopoDS_Shape occtNewShape = FixBooleanOperandFace(occtOperandBIterator.Value());
			//	occtNewShape = FixBooleanOperandShell(occtNewShape);
			//	occtNewShape = FixBooleanOperandCell(occtNewShape);
			//	rOcctCellsBuildersOperandsB.Append(occtNewShape);*/
   //             rOcctCellsBuildersOperandsB.Append(occtOperandBIterator.Value());
			//}
		}
	}

	Topology::Ptr Topology::XOR(const Topology::Ptr & kpOtherTopology, const bool kTransferDictionary)
	{
		if (kpOtherTopology == nullptr)
		{
			return Topology::ByOcctShape(GetOcctShape(), GetInstanceGUID());
		}

		TopTools_ListOfShape occtArgumentsA;
		TopTools_ListOfShape occtArgumentsB;
		AddBooleanOperands(kpOtherTopology, occtArgumentsA, occtArgumentsB);

		BOPAlgo_CellsBuilder occtCellsBuilder;
		NonRegularBooleanOperation(occtArgumentsA, occtArgumentsB, occtCellsBuilder);

		// 2. Select the parts to be included in the final result.
		TopTools_ListOfShape occtListToTake;
		TopTools_ListOfShape occtListToAvoid;

		for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorA(occtArgumentsA);
			kOcctShapeIteratorA.More();
			kOcctShapeIteratorA.Next())
		{
			occtListToTake.Clear();
			occtListToAvoid.Clear();
			occtListToTake.Append(kOcctShapeIteratorA.Value());
			for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorB(occtArgumentsB);
				kOcctShapeIteratorB.More();
				kOcctShapeIteratorB.Next())
			{
				occtListToAvoid.Append(kOcctShapeIteratorB.Value());
			}
			occtCellsBuilder.AddToResult(occtListToTake, occtListToAvoid);
		}


		for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorB(occtArgumentsB);
			kOcctShapeIteratorB.More();
			kOcctShapeIteratorB.Next())
		{
			occtListToTake.Clear();
			occtListToAvoid.Clear();
			occtListToTake.Append(kOcctShapeIteratorB.Value());
			for (TopTools_ListIteratorOfListOfShape kOcctShapeIteratorA(occtArgumentsA);
				kOcctShapeIteratorA.More();
				kOcctShapeIteratorA.Next())
			{
				occtListToAvoid.Append(kOcctShapeIteratorA.Value());
			}
			occtCellsBuilder.AddToResult(occtListToTake, occtListToAvoid);
		}

		occtCellsBuilder.MakeContainers();

		TopoDS_Shape occtResultShape = occtCellsBuilder.Shape();
		TopoDS_Shape occtPostprocessedShape = occtResultShape.IsNull() ? occtResultShape : PostprocessBooleanResult(occtResultShape);
		Topology::Ptr pPostprocessedShape = Topology::ByOcctShape(occtPostprocessedShape, "");
		if (pPostprocessedShape == nullptr)
		{
			return nullptr;
		}
		TransferContents(GetOcctShape(), pPostprocessedShape);
		TransferContents(kpOtherTopology->GetOcctShape(), pPostprocessedShape);

		Topology::Ptr pCopyPostprocessedShape = pPostprocessedShape->DeepCopy();

		//AttributeManager::GetInstance().DeepCopyAttributes(GetOcctShape(), occtPostprocessedShape);
		if (kTransferDictionary)
		{
			BooleanTransferDictionary(this, kpOtherTopology.get(), pCopyPostprocessedShape.get(), true);
		}

		//GlobalCluster::GetInstance().AddTopology(pCopyPostprocessedShape->GetOcctShape());
		return pCopyPostprocessedShape;
	}

	Topology::Ptr Topology::Divide(const Topology::Ptr & kpTool, const bool kTransferDictionary)
	{
		if (kpTool == nullptr)
		{
			return Topology::ByOcctShape(GetOcctShape(), GetInstanceGUID());
		}

		// For now, only works if this topology is a cell
		TopologyType topologyType = GetType();

		if (topologyType != TOPOLOGY_CELL && topologyType != TOPOLOGY_FACE && topologyType != TOPOLOGY_EDGE)
		{
			return nullptr;
		}

		Topology::Ptr pSlicedTopology = Slice(kpTool);

		if (topologyType == TOPOLOGY_CELL)
		{
			std::list<Cell::Ptr> cells;
			pSlicedTopology->DownwardNavigation(cells);
			for (const Cell::Ptr& kpCell : cells)
			{
				AddContent(kpCell);
			}
		}else if (topologyType == TOPOLOGY_FACE)
		{
			std::list<Face::Ptr> faces;
			pSlicedTopology->DownwardNavigation(faces);
			for (const Face::Ptr& kpFace : faces)
			{
				AddContent(kpFace);
			}
		}else //if (topologyType == TOPOLOGY_EDGE)
		{
			std::list<Edge::Ptr> edges;
			pSlicedTopology->DownwardNavigation(edges);
			for (const Edge::Ptr& kpEdge : edges)
			{
				AddContent(kpEdge);
			}
		}

		Topology::Ptr farthestTopology = TrackContextAncestor();

		Topology::Ptr finalTopology = farthestTopology->DeepCopy();

		//AttributeManager::GetInstance().DeepCopyAttributes(GetOcctShape(), finalTopology->GetOcctShape());
		if (kTransferDictionary)
		{
			BooleanTransferDictionary(this, nullptr, finalTopology.get(), true);
		}

		return finalTopology;
	}

	void Topology::SubTopologies(const TopoDS_Shape& rkShape, TopTools_ListOfShape& rSubTopologies)
	{
		for (TopoDS_Iterator occtShapeIterator(rkShape); occtShapeIterator.More(); occtShapeIterator.Next())
		{
			rSubTopologies.Append(occtShapeIterator.Value());
		}
	}

	void Topology::SubTopologies(std::list<Topology::Ptr>& rSubTopologies) const
	{
		TopTools_ListOfShape occtListMembers;
		Topology::SubTopologies(GetOcctShape(), occtListMembers);
		for (TopTools_ListIteratorOfListOfShape occtIterator(occtListMembers);
			occtIterator.More();
			occtIterator.Next())
		{
			Topology::Ptr pMemberTopology = Topology::ByOcctShape(occtIterator.Value(), "");
			rSubTopologies.push_back(pMemberTopology);
		}
	}

	int Topology::NumOfSubTopologies() const
	{
		std::list<Topology::Ptr> subTopologies;
		SubTopologies(subTopologies);
		return (int) subTopologies.size();
	}

	void Topology::Shells(const Topology::Ptr& kpHostTopology, std::list<std::shared_ptr<Shell>>& rShells) const
	{
		Navigate(kpHostTopology, rShells);
	}

	void Topology::Edges(const Topology::Ptr& kpHostTopology, std::list<std::shared_ptr<Edge>>& rEdges) const
	{
		Navigate(kpHostTopology, rEdges);
	}

	void Topology::Faces(const Topology::Ptr& kpHostTopology, std::list<std::shared_ptr<Face>>& rFaces) const
	{
		Navigate(kpHostTopology, rFaces);
	}

	void Topology::Vertices(const Topology::Ptr& kpHostTopology, std::list<std::shared_ptr<Vertex>>& rVertices) const
	{
		Navigate(kpHostTopology, rVertices);
	}

	void Topology::Wires(const Topology::Ptr& kpHostTopology, std::list<std::shared_ptr<Wire>>& rWires) const
	{
		Navigate(kpHostTopology, rWires);
	}

	void Topology::Cells(const Topology::Ptr& kpHostTopology, std::list<std::shared_ptr<Cell>>& rCells) const
	{
		Navigate(kpHostTopology, rCells);
	}

	void Topology::CellComplexes(const Topology::Ptr& kpHostTopology, std::list<std::shared_ptr<CellComplex>>& rCellComplexes) const
	{
		Navigate(kpHostTopology, rCellComplexes);
	}

	bool Topology::IsContainerType(const TopoDS_Shape& rkOcctShape)
	{
		TopAbs_ShapeEnum occtShapeType = rkOcctShape.ShapeType();
		if (occtShapeType == TopAbs_WIRE || occtShapeType == TopAbs_SHELL || occtShapeType == TopAbs_COMPSOLID || occtShapeType == TopAbs_COMPOUND)
		{
			return true;
		}

		return false;
	}

// #ifdef _DEBUG
	void Topology::GlobalClusterSubTopologies(std::list<Topology::Ptr>& rSubTopologies) const
	{
		TopTools_ListOfShape occtListMembers;
		Topology::SubTopologies(GlobalCluster::GetInstance().GetOcctCompound(), occtListMembers);
		for (TopTools_ListIteratorOfListOfShape occtIterator(occtListMembers);
			occtIterator.More();
			occtIterator.Next())
		{
			Topology::Ptr pMemberTopology = Topology::ByOcctShape(occtIterator.Value(), "");
			rSubTopologies.push_back(pMemberTopology);
		}
	}
// #endif

	void Topology::UpwardNavigation(const TopoDS_Shape & rkOcctHostTopology, const int kTopologyType, std::list<std::shared_ptr<Topology>>& rAncestors) const
	{
		switch (kTopologyType)
		{
			case TOPOLOGY_VERTEX:
				return; // no lower dimensional topology
			case TOPOLOGY_EDGE:
			{
				std::list<Edge::Ptr> edges;
				UpwardNavigation(rkOcctHostTopology, edges);
				std::for_each(edges.begin(), edges.end(), [&rAncestors](Topology::Ptr n) { rAncestors.push_back(n); });
				return;
			}
			case TOPOLOGY_WIRE:
			{
				std::list<Wire::Ptr> wires;
				UpwardNavigation(rkOcctHostTopology, wires);
				std::for_each(wires.begin(), wires.end(), [&rAncestors](Topology::Ptr n) { rAncestors.push_back(n); });
				return;
			}
			case TOPOLOGY_FACE:
			{
				std::list<Face::Ptr> faces;
				UpwardNavigation(rkOcctHostTopology, faces);
				std::for_each(faces.begin(), faces.end(), [&rAncestors](Topology::Ptr n) { rAncestors.push_back(n); });
				return;
			}
			case TOPOLOGY_SHELL:
			{
				std::list<Shell::Ptr> shells;
				UpwardNavigation(rkOcctHostTopology, shells);
				std::for_each(shells.begin(), shells.end(), [&rAncestors](Topology::Ptr n) { rAncestors.push_back(n); });
				return;
			}
			case TOPOLOGY_CELL:
			{
				std::list<Cell::Ptr> cells;
				UpwardNavigation(rkOcctHostTopology, cells);
				std::for_each(cells.begin(), cells.end(), [&rAncestors](Topology::Ptr n) { rAncestors.push_back(n); });
				return;
			}
			case TOPOLOGY_CELLCOMPLEX:
			{
				std::list<CellComplex::Ptr> cellComplexes;
				UpwardNavigation(rkOcctHostTopology, cellComplexes);
				std::for_each(cellComplexes.begin(), cellComplexes.end(), [&rAncestors](Topology::Ptr n) { rAncestors.push_back(n); });
				return;
			}
			case TOPOLOGY_CLUSTER:
			{
				std::list<Cluster::Ptr> clusters;
				UpwardNavigation(rkOcctHostTopology, clusters);
				std::for_each(clusters.begin(), clusters.end(), [&rAncestors](Topology::Ptr n) { rAncestors.push_back(n); });
				return;
			}
		}
	}

	void Topology::DownwardNavigation(const TopoDS_Shape& rkOcctShape, const TopAbs_ShapeEnum & rkShapeEnum, TopTools_MapOfShape & rOcctMembers)
	{
		for (TopExp_Explorer occtExplorer(rkOcctShape, rkShapeEnum); occtExplorer.More(); occtExplorer.Next())
		{
			const TopoDS_Shape& occtCurrent = occtExplorer.Current();
			if (!rOcctMembers.Contains(occtCurrent))
			{
				rOcctMembers.Add(occtCurrent);
			}
		}
	}

	void DeepCopyExplodeShape(const TopoDS_Shape& rkOcctOriginalShape, BRepBuilderAPI_Copy& rOcctCopy, TopTools_DataMapOfShapeShape& rOcctShapeCopyShapeMap)
	{
		rOcctShapeCopyShapeMap.Bind(rkOcctOriginalShape, rOcctCopy.Shape());
		rOcctShapeCopyShapeMap.Bind(rOcctCopy.Shape(), rkOcctOriginalShape); // 2-way

		TopTools_ListOfShape occtMembers;
		Topology::Members(rkOcctOriginalShape, occtMembers);
		for (TopTools_ListIteratorOfListOfShape occtMemberIterator(occtMembers);
			occtMemberIterator.More();
			occtMemberIterator.Next())
		{
			TopoDS_Shape& rOcctMember = occtMemberIterator.Value();
			try{
				TopoDS_Shape occtMemberCopy = rOcctCopy.ModifiedShape(rOcctMember);
				rOcctShapeCopyShapeMap.Bind(rOcctMember, occtMemberCopy);
				rOcctShapeCopyShapeMap.Bind(occtMemberCopy, rOcctMember);

				AttributeManager::GetInstance().CopyAttributes(rOcctMember, occtMemberCopy);
			}
			catch (Standard_NoSuchObject)
			{

			}
		}
	}

	// Based on https://stackoverflow.com/questions/14536702/algorithm-to-clone-a-graph
	Topology::Ptr DeepCopyImpl(const TopoDS_Shape& rkOcctShape, TopTools_DataMapOfShapeShape& rOcctShapeCopyShapeMap)
	{
		BRepBuilderAPI_Copy occtShapeCopier(rkOcctShape);
		TopoDS_Shape occtShapeCopy = occtShapeCopier.Shape();

		//AttributeManager::GetInstance().DeepCopyAttributes(rkOcctShape, occtShapeCopy);
        AttributeManager::GetInstance().CopyAttributes(rkOcctShape, occtShapeCopy);
		DeepCopyExplodeShape(rkOcctShape, occtShapeCopier, rOcctShapeCopyShapeMap);

		// Explode
		Topology::Ptr pShapeCopy = Topology::ByOcctShape(occtShapeCopy, Topology::GetInstanceGUID(rkOcctShape));

		std::list<Context::Ptr> contexts;
		Topology::Contexts(rkOcctShape, contexts);
		for (const Context::Ptr& kpContext : contexts)
		{
			Topology::Ptr pContextTopology = kpContext->Topology();
			
			TopoDS_Shape occtCopyShape;
			bool isContextCopied = rOcctShapeCopyShapeMap.Find(pContextTopology->GetOcctShape(), occtCopyShape);
			Topology::Ptr pCopyContextTopology;
			if (isContextCopied)
			{
				pCopyContextTopology = Topology::ByOcctShape(occtCopyShape, Topology::GetInstanceGUID(pContextTopology->GetOcctShape()));
			}else
			{
				pCopyContextTopology = DeepCopyImpl(pContextTopology->GetOcctShape(), rOcctShapeCopyShapeMap);
			}

			pCopyContextTopology->AddContent(pShapeCopy);
		}

		std::list<Topology::Ptr> subContents;
		Topology::SubContents(rkOcctShape, subContents);
		for (const Topology::Ptr& kpSubContent : subContents)
		{
			TopoDS_Shape occtCopyShape;
			bool isContentCopied = rOcctShapeCopyShapeMap.Find(kpSubContent->GetOcctShape(), occtCopyShape);
			Topology::Ptr pCopyContentTopology;
			if (isContentCopied)
			{
				pCopyContentTopology = Topology::ByOcctShape(occtCopyShape, Topology::GetInstanceGUID(kpSubContent->GetOcctShape()));
			}else
			{
				pCopyContentTopology = DeepCopyImpl(kpSubContent->GetOcctShape(), rOcctShapeCopyShapeMap);
			}
			int filterType = 0;
			std::list<Context::Ptr> contexts;
			kpSubContent->Contexts(contexts);
			for (const Context::Ptr& pContext : contexts)
			{
				int contextType = pContext->Topology()->GetType();
				filterType = filterType | contextType;
			}
			//pShapeCopy->AddContent(pCopyContentTopology, filterType);
		}
		// GlobalCluster::GetInstance().AddTopology(pShapeCopy);
		return pShapeCopy;
	}

	Topology::Ptr Topology::DeepCopy()
	{
		TopTools_DataMapOfShapeShape occtShapeCopyShapeMap;
		Topology::Ptr pShapeCopy = DeepCopyImpl(GetOcctShape(), occtShapeCopyShapeMap);
		return pShapeCopy;
	}

	Topology::Ptr Topology::ShallowCopy()
	{
		BRepBuilderAPI_Copy occtShapeCopier(GetOcctShape());
		AttributeManager::GetInstance().DeepCopyAttributes(GetOcctShape(), occtShapeCopier.Shape());
		return Topology::ByOcctShape(occtShapeCopier.Shape(), GetInstanceGUID());
	}

	TopoDS_Shape Topology::CopyOcct(const TopoDS_Shape& rkOcctShape)
	{
		BRepBuilderAPI_Copy occtShapeCopy(rkOcctShape);
		TopoDS_Shape occtCopyShape = occtShapeCopy.Shape();

		//GlobalCluster::GetInstance().AddTopology(occtCopyShape);
		return occtCopyShape;
	}

	void Topology::ReplaceSubentity(const Topology::Ptr& rkOriginalSubshape, const Topology::Ptr& rkNewSubshape)
	{
		ReplaceSubentity(rkOriginalSubshape->GetOcctShape(), rkNewSubshape->GetOcctShape());
	}

	void Topology::ReplaceSubentity(const TopoDS_Shape& rkOcctOriginalSubshape, const TopoDS_Shape& rkOcctNewSubshape)
	{
		Handle(ShapeBuild_ReShape) occtReshaper = new ShapeBuild_ReShape();
		occtReshaper->Replace(rkOcctOriginalSubshape, rkOcctNewSubshape);
		TopoDS_Shape newShape = occtReshaper->Apply(GetOcctShape());
	}

	bool Topology::IsSame(const Topology::Ptr& kpTopology) const
	{
		bool isSame = GetOcctShape().IsSame(kpTopology->GetOcctShape());
		return isSame;
	}

	bool Topology::IsReversed()
	{
		TopAbs_Orientation occtOrientation = GetOcctShape().Orientation();
		return occtOrientation == TopAbs_REVERSED;
	}

	Topology::Ptr Topology::DeepCopyAttributesFrom(const std::list<Topology::Ptr>& kpOriginTopologies)
	{
		// 1. Get all dictionaries stored in rkOcctOriginShape
		// NOTE: this map will map the subshapes to the attributes
		AttributeManager::ShapeToAttributesMap originAttributeMap;

        std::list<int> typeFilters;
		for (const Topology::Ptr& kpOriginTopology : kpOriginTopologies)
		{
			AttributeManager::GetInstance().GetAttributesInSubshapes(kpOriginTopology->GetOcctShape(), originAttributeMap);
		}

		std::list<Vertex::Ptr> selectors;
		std::list<AttributeManager::AttributeMap> dictionaries;
        TopTools_MapOfShape occtProcessedSubshapes;
		for (auto shapeToAttributes : originAttributeMap)
		{
            Vertex::Ptr referenceVertex = nullptr;
			TopoDS_Shape occtSubshape = shapeToAttributes.first;

            // Check if already processed
            for (TopTools_MapIteratorOfMapOfShape occtProccessedSubshapeIterator(occtProcessedSubshapes);
                occtProccessedSubshapeIterator.More();
                occtProccessedSubshapeIterator.Next())
            {
                if (occtSubshape.IsSame(occtProccessedSubshapeIterator.Value()))
                {
                    continue;
                }
            }
            occtProcessedSubshapes.Add(occtSubshape);

            if (occtSubshape.ShapeType() == TopAbs_SOLID)
            {
                referenceVertex = TopologicUtilities::CellUtility::InternalVertex(std::make_shared<TopologicCore::Cell>(TopoDS::Solid(occtSubshape)));
            }
            else if(occtSubshape.ShapeType() == TopAbs_FACE)
            {
                referenceVertex = TopologicUtilities::FaceUtility::InternalVertex(std::make_shared<TopologicCore::Face>(TopoDS::Face(occtSubshape)));
            }
            else
            {
                referenceVertex = TopologicalQuery::Downcast<Vertex>(Topology::ByOcctShape(CenterOfMass(occtSubshape)));
            }
			selectors.push_back(referenceVertex);

			AttributeManager::AttributeMap& dictionary = shapeToAttributes.second;
			dictionaries.push_back(dictionary);
            typeFilters.push_back((int)Topology::GetTopologyType(occtSubshape.ShapeType()));
		}

		Topology::Ptr pCopyTopology = SetDictionaries(selectors, dictionaries, typeFilters, true);
		return pCopyTopology;
	}

	void Topology::Members(TopTools_ListOfShape& rOcctMembers) const
	{
		const TopoDS_Shape& rkOcctShape = GetOcctShape();
		Members(rkOcctShape, rOcctMembers);
	}

	void Topology::Members(std::list<Topology::Ptr>& rMembers) const
	{
		TopTools_ListOfShape occtMembers;
		Members(occtMembers);
		for (TopTools_ListIteratorOfListOfShape occtMemberIterator(occtMembers); occtMemberIterator.More(); occtMemberIterator.Next())
		{
			rMembers.push_back(Topology::ByOcctShape(occtMemberIterator.Value(), ""));
		}
	}

	void Topology::Members(const TopoDS_Shape & rkOcctShape, TopTools_ListOfShape & rOcctMembers)
	{
		// Store the children
		for (int i = ((int)rkOcctShape.ShapeType()) + 1; i < (int)TopAbs_SHAPE; ++i)
		{
			TopAbs_ShapeEnum occtShapeEnum = (TopAbs_ShapeEnum)i;
			TopTools_MapOfShape occtMembers;
			Topology::DownwardNavigation(rkOcctShape, occtShapeEnum, occtMembers);
			for (TopTools_MapIteratorOfMapOfShape occtMembersIterator(occtMembers);
				occtMembersIterator.More();
				occtMembersIterator.Next())
			{
				rOcctMembers.Append(occtMembersIterator.Value());
			}
		}
	}

	const std::string Topology::GetInstanceGUID() const
	{
		return GetInstanceGUID(GetOcctShape());
	}

	void Topology::SetInstanceGUID(const TopoDS_Shape& rkOcctShape, const std::string& rkGuid)
	{
		InstanceGUIDManager::GetInstance().Add(rkOcctShape, rkGuid);
	}

	const std::string Topology::GetInstanceGUID(const TopoDS_Shape & rkOcctShape)
	{
		std::string guid;
		bool value = InstanceGUIDManager::GetInstance().Find(rkOcctShape, guid);
		assert(value);
		return guid;
	}

	void Topology::SetDictionary(const TopologicCore::Dictionary& attributes)
	{
		auto occtShape = GetOcctShape();
	
		AttributeManager& attrManager = AttributeManager::GetInstance();

		attrManager.ClearOne(occtShape);

		for (auto entry : attributes)
		{
			attrManager.Add(occtShape, entry.first, entry.second);
		}
		
	}

	Dictionary Topology::GetDictionary()
	{
		TopologicCore::Dictionary dict;
		AttributeManager::GetInstance().FindAll(GetOcctShape(), dict);
		return dict;
	}

}