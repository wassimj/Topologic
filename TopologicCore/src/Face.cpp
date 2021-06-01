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

#include "Face.h"
#include "Edge.h"
#include "Cell.h"
#include "Shell.h"
#include "Vertex.h"
#include "Wire.h"
#include "FaceFactory.h"
#include "Utilities.h"
#include "GlobalCluster.h"
#include "AttributeManager.h"

#include <Utilities/FaceUtility.h>

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BSplCLib.hxx>
#include <BRepGProp.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools.hxx>
//#include <DBRep_IsoBuilder.hxx>
//#include <DBRep_Face.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_CartesianPoint.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GProp_GProps.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeFix_Face.hxx>
#include <StdFail_NotDone.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopOpeBRepBuild_Tools.hxx>

#include <assert.h>

namespace TopologicCore
{
	void Face::AdjacentFaces(std::list<Face::Ptr>& rFaces) const
	{
		// Iterate through the edges and find the incident faces which are not this face.
		TopTools_IndexedDataMapOfShapeListOfShape occtEdgeFaceMap;
		TopExp::MapShapesAndUniqueAncestors(GlobalCluster::GetInstance().GetOcctCompound(), TopAbs_EDGE, TopAbs_FACE, occtEdgeFaceMap);

		// Find the constituent faces
		TopTools_MapOfShape occtEdges;
		for (TopExp_Explorer occtExplorer(GetOcctShape(), TopAbs_EDGE); occtExplorer.More(); occtExplorer.Next())
		{
			const TopoDS_Shape& occtCurrent = occtExplorer.Current();
			if (!occtEdges.Contains(occtCurrent))
			{
				occtEdges.Add(occtCurrent);
			}
		}

		const TopoDS_Face& rkOcctFace = GetOcctFace();
		TopTools_MapOfShape occtAdjacentFaces;
		for (TopTools_MapOfShape::const_iterator kOcctEdgeIterator = occtEdges.cbegin();
			kOcctEdgeIterator != occtEdges.cend();
			kOcctEdgeIterator++)
		{
			const TopoDS_Shape& rkOcctEdge = *kOcctEdgeIterator;
			const TopTools_ListOfShape& rkIncidentFaces = occtEdgeFaceMap.FindFromKey(rkOcctEdge);

			for (TopTools_ListOfShape::const_iterator kOcctFaceIterator = rkIncidentFaces.cbegin();
				kOcctFaceIterator != rkIncidentFaces.cend();
				kOcctFaceIterator++)
			{
				const TopoDS_Shape& rkIncidentFace = *kOcctFaceIterator;
				if (!rkOcctFace.IsSame(rkIncidentFace))
				{
					occtAdjacentFaces.Add(rkIncidentFace);
				}
			}
		}

		for (TopTools_MapIteratorOfMapOfShape occtAdjacentFaceIterator(occtAdjacentFaces);
			occtAdjacentFaceIterator.More();
			occtAdjacentFaceIterator.Next())
		{
			rFaces.push_back(std::make_shared<Face>(TopoDS::Face(occtAdjacentFaceIterator.Value())));
		}
	}

	void Face::Cells(std::list<Cell::Ptr>& rCells) const
	{
		UpwardNavigation(rCells);
	}

	void Face::Edges(std::list<Edge::Ptr>& rEdges) const
	{
		DownwardNavigation(rEdges);
	}

	void Face::Shells(std::list<Shell::Ptr>& rShells) const
	{
		UpwardNavigation(rShells);
	}

	void Face::Vertices(std::list<Vertex::Ptr>& rVertices) const
	{
		DownwardNavigation(rVertices);
	}

	void Face::Wires(std::list<Wire::Ptr>& rWires) const
	{
		DownwardNavigation(rWires);
	}

	Vertex::Ptr Face::CenterOfMass() const
	{
		TopoDS_Vertex occtCenterOfMass = CenterOfMass(GetOcctFace());
		return std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtCenterOfMass));
	}

	TopoDS_Vertex Face::CenterOfMass(const TopoDS_Face & rkOcctFace)
	{
		GProp_GProps occtShapeProperties;
		//ShapeFix_Face occtShapeFix(rkOcctFace);
		//occtShapeFix.Perform();
		BRepGProp::SurfaceProperties(rkOcctFace, occtShapeProperties);
		return BRepBuilderAPI_MakeVertex(occtShapeProperties.CentreOfMass());
	}

	Face::Ptr Face::ByExternalBoundary(const Wire::Ptr& kpExternalBoundary)
	{
		std::list<Wire::Ptr> internalBoundaries;
		Face::Ptr face = ByExternalInternalBoundaries(kpExternalBoundary, internalBoundaries);
		AttributeManager::GetInstance().DeepCopyAttributes(kpExternalBoundary->GetOcctWire(), face->GetOcctFace());

		return face;
	}

	Face::Ptr Face::ByExternalInternalBoundaries(
		const Wire::Ptr& pkExternalBoundary,
		const std::list<Wire::Ptr>& rkInternalBoundaries)
	{
        if (!pkExternalBoundary->IsClosed())
        {
            throw std::runtime_error("The input Wire is open.");
        }
		TopoDS_Wire occtExternalBoundary = pkExternalBoundary->GetOcctWire();
		BRepBuilderAPI_MakeFace occtMakeFace(occtExternalBoundary);
		if (occtMakeFace.Error() != BRepBuilderAPI_FaceDone)
		{
			Throw(occtMakeFace);
		}

		TopoDS_Face occtFace = occtMakeFace;

		double area = TopologicUtilities::FaceUtility::Area(occtFace);
		if (area <= 0.0)
		{
			occtExternalBoundary.Reverse();
			BRepBuilderAPI_MakeFace occtReverseMakeFace(occtExternalBoundary);
			if (occtReverseMakeFace.Error() != BRepBuilderAPI_FaceDone)
			{
				Throw(occtMakeFace);
			}

			occtFace = occtReverseMakeFace;
		}

		// At this point, occtMakeFace and occtFace are valid Faces with positive area.
		for (const Wire::Ptr& kpInternalBoundary : rkInternalBoundaries)
		{
			// Copy the Face
			BRepBuilderAPI_Copy occtCopier(occtFace);
			TopoDS_Face occtCopyFace = TopoDS::Face(occtCopier.Shape());

			// Add to a new MakeFace
			BRepBuilderAPI_MakeFace occtCopyMakeFace(occtCopyFace);

			// Add the internal Wire to occtCopyMakeFace
			TopoDS_Wire occtInternalWire = kpInternalBoundary->GetOcctWire();
			occtCopyMakeFace.Add(occtInternalWire);
			TopoDS_Face newCopyFace = occtCopyMakeFace.Face();

			// If the new area is larger than the original area, reverse and redo
			double newArea = TopologicUtilities::FaceUtility::Area(newCopyFace);
			if (newArea > area)
			{
				occtInternalWire.Reverse();
			}

			// Add the internal Wire (original or reversed orientation, as above) to the old MakeFace
			occtMakeFace.Add(occtInternalWire);

			// Updating the area
			area = TopologicUtilities::FaceUtility::Area(occtMakeFace);
		}

		TopoDS_Face occtFixedFace = OcctShapeFix(occtMakeFace);

		Face::Ptr pFace = std::make_shared<Face>(occtFixedFace);
		Face::Ptr pCopyFace = std::dynamic_pointer_cast<Face>(pFace->DeepCopy());
		std::list<Topology::Ptr> wiresAsTopologies;
		//AttributeManager::GetInstance().DeepCopyAttributes(pkExternalBoundary->GetOcctWire(), pCopyFace->GetOcctFace());
		wiresAsTopologies.push_back(pkExternalBoundary);
		for (const Wire::Ptr& kpInternalBoundary : rkInternalBoundaries)
		{
			wiresAsTopologies.push_back(kpInternalBoundary);
			//AttributeManager::GetInstance().DeepCopyAttributes(kpInternalBoundary->GetOcctWire(), pCopyFace->GetOcctFace());
		}
		pCopyFace->DeepCopyAttributesFrom(wiresAsTopologies);

		GlobalCluster::GetInstance().AddTopology(pCopyFace->GetOcctFace());
		return pCopyFace;
	}

	Face::Ptr Face::ByEdges(const std::list<Edge::Ptr>& rkEdges)
	{
		if (rkEdges.size() < 3)
		{
			throw std::runtime_error("Fewer than 3 edges are passed.");
		}

		Wire::Ptr pWire = Wire::ByEdges(rkEdges);
		Face::Ptr pFace = ByExternalBoundary(pWire);
		std::list<Topology::Ptr> edgesAsTopologies;
		for (const Edge::Ptr& kpEdge : rkEdges)
		{
			edgesAsTopologies.push_back(kpEdge);
			AttributeManager::GetInstance().DeepCopyAttributes(kpEdge->GetOcctEdge(), pFace->GetOcctFace());
		}
		pFace->DeepCopyAttributesFrom(edgesAsTopologies);

		return pFace;
	}

	Face::Ptr Face::BySurface(Handle(Geom_Surface) pOcctSurface)
	{
		BRepBuilderAPI_MakeFace occtMakeFace;
		try {
			occtMakeFace = BRepBuilderAPI_MakeFace(pOcctSurface, Precision::Confusion());
		}
		catch (StdFail_NotDone&)
		{
			Throw(occtMakeFace);
		}
		ShapeFix_Face occtShapeFix(occtMakeFace);
		occtShapeFix.Perform();
		Face::Ptr pFace = std::make_shared<Face>(TopoDS::Face(occtShapeFix.Result()));
		GlobalCluster::GetInstance().AddTopology(pFace->GetOcctFace());
		return pFace;
	}

	Face::Ptr Face::BySurface(
		const TColgp_Array2OfPnt& rkOcctPoles,
		const TColStd_Array2OfReal& rkOcctWeights,
		const TColStd_Array1OfReal& rkOcctUKnots,
		const TColStd_Array1OfReal& rkOcctVKnots,
		const TColStd_Array1OfInteger& rkOcctUMultiplicities,
		const TColStd_Array1OfInteger& rkOcctVMultiplicities,
		const int kUDegree,
		const int kVDegree,
		const bool kIsUPeriodic,
		const bool kIsVPeriodic,
		const bool kIsRational,
		const Wire::Ptr& kpOuterWire,
		const std::list<Wire::Ptr>& rkInnerWires)
	{
		BRepBuilderAPI_MakeFace occtMakeFace;
		TopoDS_Face occtFace;
		double area = 0.0;
		try {
			Handle(Geom_BSplineSurface) pOcctBSplineSurface = new Geom_BSplineSurface(
				rkOcctPoles,
				rkOcctWeights,
				rkOcctUKnots,
				rkOcctVKnots,
				rkOcctUMultiplicities,
				rkOcctVMultiplicities,
				kUDegree, kVDegree,
				kIsUPeriodic, kIsVPeriodic);
			if (kpOuterWire != nullptr)
			{
				occtMakeFace = BRepBuilderAPI_MakeFace(pOcctBSplineSurface, TopoDS::Wire(kpOuterWire->GetOcctShape()), true);
				ShapeFix_Face occtShapeFix(occtMakeFace);
				occtShapeFix.Perform();

				occtFace = occtShapeFix.Face();
				area = TopologicUtilities::FaceUtility::Area(occtFace);
				if (area <= 0.0)
				{
					occtMakeFace = BRepBuilderAPI_MakeFace(pOcctBSplineSurface, TopoDS::Wire(kpOuterWire->GetOcctShape().Reversed()), true);
					ShapeFix_Face occtShapeFix(occtMakeFace);
					occtShapeFix.Perform();

					occtFace = occtShapeFix.Face();
					area = TopologicUtilities::FaceUtility::Area(occtFace);
				}

			}
			else
			{
				occtMakeFace = BRepBuilderAPI_MakeFace(pOcctBSplineSurface, Precision::Confusion());
				occtFace = occtMakeFace;
			}
		}
		catch (const Standard_ConstructionError&) // from Geom_BSplineCurve's constructor
		{
			int occtMaxDegree = Geom_BSplineSurface::MaxDegree();
			if (kUDegree < 1 || kUDegree > occtMaxDegree) {
				throw std::runtime_error(std::string("The curve's u degree must be between 1 and " + std::to_string(occtMaxDegree) + " (OCCT's max degree).").c_str());
			}
			if (kVDegree < 1 || kVDegree > occtMaxDegree) {
				throw std::runtime_error(std::string("The curve's v degree must be between 1 and " + std::to_string(occtMaxDegree) + " (OCCT's max degree).").c_str());
			}

			if (rkOcctPoles.ColLength() < 2 || rkOcctPoles.RowLength() < 2)
			{
				throw std::runtime_error("There must be more than one control points along each axis.");
			}

			if (rkOcctPoles.ColLength() != rkOcctWeights.ColLength() || rkOcctPoles.RowLength() != rkOcctWeights.RowLength())
			{
				throw std::runtime_error("The lists of control points and weights must have the same numbers of rows and columns.");
			}

			if (rkOcctUKnots.Length() != rkOcctUMultiplicities.Length() || rkOcctVKnots.Length() != rkOcctVMultiplicities.Length())
			{
				throw std::runtime_error("The lists of U and V knots and multiplicities must have the same length.");
			}

			for (int i = rkOcctWeights.LowerRow(); i <= rkOcctWeights.UpperRow(); i++)
			{
				for (int j = rkOcctWeights.LowerCol(); j <= rkOcctWeights.UpperCol(); j++)
				{
					if (rkOcctWeights(i, j) <= gp::Resolution())
					{
						throw std::runtime_error(std::string("The (" + std::to_string(i) + ", " + std::to_string(j) + ")'s weight is smaller than OCCT's resolution (i.e. practically zero).").c_str());
					}
				}
			}

			for (int i = rkOcctUKnots.Lower(); i < rkOcctUKnots.Upper(); i++)
			{
				if (rkOcctUKnots(i + 1) - rkOcctUKnots(i) <= Epsilon(Abs(rkOcctUKnots(i)))) {
					throw std::runtime_error(std::string("The " + std::to_string(i + 1) + "'s U knot must be larger than the " + std::to_string(i) + "'s U knot.").c_str());
				}
			}
			for (int i = rkOcctVKnots.Lower(); i < rkOcctVKnots.Upper(); i++)
			{
				if (rkOcctVKnots(i + 1) - rkOcctVKnots(i) <= Epsilon(Abs(rkOcctVKnots(i)))) {
					throw std::runtime_error(std::string("The " + std::to_string(i + 1) + "'s V knot must be larger than the " + std::to_string(i) + "'s V knot.").c_str());
				}
			}

			int occtNumberOfPoles = BSplCLib::NbPoles(kUDegree, kIsUPeriodic, rkOcctUMultiplicities);
			if (rkOcctPoles.Length() != occtNumberOfPoles)
			{
				if (kIsUPeriodic)
				{
					throw std::runtime_error("The curve is U periodic, so the number of poles must be equal to the sum of the U multiplicities.");
				}
				else
				{
					throw std::runtime_error("The curve is not U periodic, so the number of poles must be equal to the sum of the U multiplicities - U degree - 1, which must be larger than 1.");
				}
			}
			if (rkOcctPoles.Length() != BSplCLib::NbPoles(kVDegree, kIsVPeriodic, rkOcctUMultiplicities))
			{
				if (kIsVPeriodic)
				{
					throw std::runtime_error("The curve is V periodic, so the number of poles must be equal to the sum of the V multiplicities.");
				}
				else
				{
					throw std::runtime_error("The curve is not V periodic, so the number of poles must be equal to the sum of the V multiplicities - V degree - 1, which must be larger than 1.");
				}
			}
		}
		catch (const StdFail_NotDone&) // from BRepBuilderAPI_MakeEdge
		{
			Throw(occtMakeFace);
		}

		// At this point, occtMakeFace and occtFace are valid Faces with positive area.
		for (const Wire::Ptr& kInnerWire : rkInnerWires)
		{
			// Copy the Face
			BRepBuilderAPI_Copy occtCopier(occtFace);
			TopoDS_Face occtCopyFace = TopoDS::Face(occtCopier.Shape());

			// Add to a new MakeFace
			BRepBuilderAPI_MakeFace occtCopyMakeFace(occtCopyFace);

			// Add the internal Wire to occtCopyMakeFace
			TopoDS_Wire occtInternalWire = kInnerWire->GetOcctWire();
			occtCopyMakeFace.Add(occtInternalWire);
			TopoDS_Face newCopyFace = occtCopyMakeFace.Face();

            ShapeFix_Face occtShapeFix(newCopyFace);
            occtShapeFix.Perform();

			// If the new area is larger than the original area, reverse and redo
            double newArea = TopologicUtilities::FaceUtility::Area(occtShapeFix.Face());
			if (newArea > area)
			{
				occtInternalWire.Reverse();
			}

			// Add the internal Wire (original or reversed orientation, as above) to the old MakeFace
			occtMakeFace.Add(occtInternalWire);

            ShapeFix_Face occtShapeFix2(occtMakeFace);
            occtShapeFix2.Perform();

			// Updating the area
			area = TopologicUtilities::FaceUtility::Area(occtShapeFix2.Face());
		}

		ShapeFix_Face occtShapeFix(occtMakeFace);
		occtShapeFix.Perform();

		Face::Ptr pFace = std::make_shared<Face>(TopoDS::Face(occtShapeFix.Result()));
		GlobalCluster::GetInstance().AddTopology(pFace->GetOcctFace());
		return pFace;
	}

	void Face::SharedEdges(const Face::Ptr& kpAnotherFace, std::list<Edge::Ptr>& rEdges) const
	{
		const TopoDS_Shape& rkOcctShape1 = GetOcctShape();
		TopTools_ListOfShape occtEdges1;
		for (TopExp_Explorer occtExplorer(rkOcctShape1, TopAbs_EDGE); occtExplorer.More(); occtExplorer.Next())
		{
			const TopoDS_Shape& occtCurrent = occtExplorer.Current();
			if (!occtEdges1.Contains(occtCurrent))
			{
				occtEdges1.Append(occtCurrent);
			}
		}

		const TopoDS_Shape& rkOcctShape2 = kpAnotherFace->GetOcctShape();
		TopTools_ListOfShape occtEdges2;
		for (TopExp_Explorer occtExplorer(rkOcctShape2, TopAbs_EDGE); occtExplorer.More(); occtExplorer.Next())
		{
			const TopoDS_Shape& occtCurrent = occtExplorer.Current();
			if (!occtEdges2.Contains(occtCurrent))
			{
				occtEdges2.Append(occtCurrent);
			}
		}

		for (TopTools_ListIteratorOfListOfShape occtEdgeIterator1(occtEdges1);
			occtEdgeIterator1.More();
			occtEdgeIterator1.Next())
		{
			for (TopTools_ListIteratorOfListOfShape occtEdgeIterator2(occtEdges2);
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

	void Face::SharedVertices(const Face::Ptr& kpAnotherFace, std::list<Vertex::Ptr>& rVertices) const
	{
		const TopoDS_Shape& rkOcctShape1 = GetOcctShape();
		TopTools_MapOfShape occtVertices1;
		DownwardNavigation(rkOcctShape1, TopAbs_VERTEX, occtVertices1);

		const TopoDS_Shape& rkOcctShape2 = kpAnotherFace->GetOcctShape();
		TopTools_MapOfShape occtVertices2;
		DownwardNavigation(rkOcctShape2, TopAbs_VERTEX, occtVertices2);

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

	Wire::Ptr Face::ExternalBoundary() const
	{
		return std::make_shared<Wire>(ExternalBoundary(GetOcctFace()));
	}

    TopoDS_Wire Face::ExternalBoundary(const TopoDS_Face & rkOcctFace)
    {
        TopoDS_Wire occtOuterWire = BRepTools::OuterWire(rkOcctFace);
        if (occtOuterWire.IsNull())
        {
            occtOuterWire = ShapeAnalysis::OuterWire(rkOcctFace);
        }
        if (rkOcctFace.Orientation() == TopAbs_REVERSED)
        {
            TopoDS_Wire occtReversedOuterWire = TopoDS::Wire(occtOuterWire.Reversed());
            return occtReversedOuterWire;
        }
        return occtOuterWire;
    }

	void Face::InternalBoundaries(std::list<Wire::Ptr>& rInternalBoundaries) const
	{
		const TopoDS_Face& rkFace = GetOcctFace();
        TopoDS_Wire occtOuterWire = ExternalBoundary(rkFace);
		TopoDS_Iterator occtExplorer(rkFace, Standard_False);
		while (occtExplorer.More())
		{
			if (occtExplorer.Value().ShapeType() == TopAbs_WIRE)
			{
				const TopoDS_Wire& rkWire = TopoDS::Wire(occtExplorer.Value());

				if (!rkWire.IsSame(occtOuterWire))
				{
					rInternalBoundaries.push_back(std::make_shared<Wire>(rkWire));
				}
			}
			occtExplorer.Next();
		}
	}

	void Face::AddInternalBoundary(const std::shared_ptr<Wire>& kpWire)
	{
		std::list<Wire::Ptr> wires;
		wires.push_back(kpWire);
		AddInternalBoundaries(wires);
	}

	void Face::AddInternalBoundaries(const std::list<std::shared_ptr<Wire>>& rkWires)
	{
		if (rkWires.empty())
		{
			return;
		}
		BRepBuilderAPI_MakeFace occtMakeFace(GetOcctFace());

		for (const Wire::Ptr& kpWire : rkWires)
		{
			occtMakeFace.Add(TopoDS::Wire(kpWire->GetOcctWire().Reversed()));
		}

		SetInstanceGUID(occtMakeFace, GetInstanceGUID());
		m_occtFace = occtMakeFace;
	}

	void Face::Triangulate(const double kDeflection, const double kAngularDeflection, std::list<TopologicCore::Face::Ptr>& rTriangles) const
	{
		const TopoDS_Face& rkOcctFace = GetOcctFace();

		ShapeFix_Face occtFixFace(rkOcctFace);
		occtFixFace.Perform();
		//BRepMesh_IncrementalMesh occtIncrementalMesh(occtFixFace.Result(), kDeflection, false, kAngularDeflection);
		BRepMesh_IncrementalMesh occtIncrementalMesh(occtFixFace.Result(), kDeflection);// , false, kAngularDeflection);
		TopLoc_Location occtLocation;
		Handle(Poly_Triangulation) pOcctTriangulation = BRep_Tool::Triangulation(TopoDS::Face(occtFixFace.Result()), occtLocation);
		if (pOcctTriangulation.IsNull())
		{
			throw std::runtime_error("No triangulation was produced.");
		}
		int numOfTriangles = pOcctTriangulation->NbTriangles();
		for (int i = 1; i <= numOfTriangles; ++i)
		{
			int index1 = 0, index2 = 0, index3 = 0;
			pOcctTriangulation->Triangle(i).Get(index1, index2, index3);
			gp_Pnt point1 = pOcctTriangulation->Node(index1);
			gp_Pnt point2 = pOcctTriangulation->Node(index2);
			gp_Pnt point3 = pOcctTriangulation->Node(index3);

			TopologicCore::Vertex::Ptr vertex1 = TopologicCore::Vertex::ByPoint(new Geom_CartesianPoint(point1));
			TopologicCore::Vertex::Ptr vertex2 = TopologicCore::Vertex::ByPoint(new Geom_CartesianPoint(point2));
			TopologicCore::Vertex::Ptr vertex3 = TopologicCore::Vertex::ByPoint(new Geom_CartesianPoint(point3));

			TopologicCore::Edge::Ptr edge1 = TopologicCore::Edge::ByStartVertexEndVertex(vertex1, vertex2);
			TopologicCore::Edge::Ptr edge2 = TopologicCore::Edge::ByStartVertexEndVertex(vertex2, vertex3);
			TopologicCore::Edge::Ptr edge3 = TopologicCore::Edge::ByStartVertexEndVertex(vertex3, vertex1);
			std::list<TopologicCore::Edge::Ptr> edges;
			edges.push_back(edge1);
			edges.push_back(edge2);
			edges.push_back(edge3);

			TopologicCore::Face::Ptr face = TopologicCore::Face::ByEdges(edges);
			rTriangles.push_back(face);
		}
	}

	TopoDS_Face Face::OcctShapeFix(const TopoDS_Face & rkOcctInputFace)
	{
		ShapeFix_Face occtShapeFix(rkOcctInputFace);
		occtShapeFix.Perform();
		return TopoDS::Face(rkOcctInputFace);
	}

	bool Face::IsManifold() const
	{
        return IsManifoldToTopology();
	}

    bool Face::IsManifoldToTopology(const Topology::Ptr & kpTopology) const
    {
        std::list<Cell::Ptr> cells;
        if (kpTopology == nullptr)
        {
            Cells(cells);
        }
        else
        {
            TopologicUtilities::FaceUtility::AdjacentCells(this, kpTopology, cells);
        }

        // A manifold face has 0 or 1 cell.
        if (cells.size() < 2)
        {
            return true;
        }

        return false;
    }

	void Face::Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const
	{
		rOcctGeometries.push_back(Surface());
	}

	TopoDS_Shape& Face::GetOcctShape()
	{
		return GetOcctFace();
	}

	const TopoDS_Shape& Face::GetOcctShape() const
	{
		return GetOcctFace();
	}

	TopoDS_Face& Face::GetOcctFace()
	{
		assert(!m_occtFace.IsNull() && "Face::m_occtFace is null.");
		if (m_occtFace.IsNull())
		{
			throw std::runtime_error("A null Face is encountered.");
		}

		return m_occtFace;
	}

	const TopoDS_Face& Face::GetOcctFace() const
	{
		assert(!m_occtFace.IsNull() && "Face::m_occtFace is null.");
		if (m_occtFace.IsNull())
		{
			throw std::runtime_error("A null Face is encountered.");
		}

		return m_occtFace;
	}

	void Face::SetOcctShape(const TopoDS_Shape & rkOcctShape)
	{
		SetOcctFace(TopoDS::Face(rkOcctShape));
	}

	void Face::SetOcctFace(const TopoDS_Face & rkOcctFace)
	{
		m_occtFace = rkOcctFace;
	}

	Face::Face(const TopoDS_Face& rkOcctFace, const std::string& rkGuid)
		: Topology(2, rkOcctFace, rkGuid.compare("") == 0 ? GetClassGUID() : rkGuid)
	{
		m_occtFace = TopoDS::Face(rkOcctFace);
		RegisterFactory(GetClassGUID(), std::make_shared<FaceFactory>());
	}

	Face::~Face()
	{

	}

	Handle(Geom_Surface) Face::Surface() const
	{
		TopoDS_Face occtFace = GetOcctFace();
		return BRep_Tool::Surface(occtFace);
	}

	std::string Face::GetTypeAsString() const
	{
		return std::string("Face");
	}

	void Face::Throw(const BRepBuilderAPI_MakeFace& rkOcctMakeFace)
	{
		// The error messages are based on those in the OCCT documentation.
		// https://www.opencascade.com/doc/occt-7.2.0/refman/html/_b_rep_builder_a_p_i___face_error_8hxx.html#ac7a498a52546f7535a3f73f6bab1599a

		switch (rkOcctMakeFace.Error())
		{
		case BRepBuilderAPI_NoFace:
			throw std::runtime_error("No initialization of the algorithm; only an empty constructor was used.");

		case BRepBuilderAPI_NotPlanar:
			throw std::runtime_error("No surface was given and the wire was not planar.");

		case BRepBuilderAPI_CurveProjectionFailed:
			throw std::runtime_error("Curve projection failed.");

		case BRepBuilderAPI_ParametersOutOfRange:
			throw std::runtime_error("The parameters given to limit the surface are out of its bounds.");

		//default: // i.e. BRepBuilderAPI_FaceDone, do nothing
		}
	}
}