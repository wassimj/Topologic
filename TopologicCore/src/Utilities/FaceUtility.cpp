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

#include <Utilities/FaceUtility.h>
#include <Utilities/TopologyUtility.h>
#include <Utilities/VertexUtility.h>

#include <Edge.h>
#include <Vertex.h>
#include <GlobalCluster.h>
#include <Cluster.h>

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepGProp.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomConvert.hxx>
#include <GeomLProp_SLProps.hxx>
#include <GProp_GProps.hxx>
#include <Poly_Triangulation.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_Shape.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeFix.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Face.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <BRepExtrema_DistShapeShape.hxx>

namespace TopologicUtilities
{
	double FaceUtility::Area(const TopologicCore::Face::Ptr& kpFace)
	{
		return Area(kpFace->GetOcctFace());
	}

	double FaceUtility::Area(const TopoDS_Face & rkOcctFace)
	{
		GProp_GProps occtShapeProperties;
		BRepGProp::SurfaceProperties(rkOcctFace, occtShapeProperties);
		return occtShapeProperties.Mass();
	}

	TopologicCore::Face::Ptr FaceUtility::ByVertices(const std::list<std::list<TopologicCore::Vertex::Ptr>>& rkVertices)
	{
		// Check if all vertex lists have the same length
		int rowLength = 0;
		bool firstRow = true;
		for (const std::list<TopologicCore::Vertex::Ptr>& rkVerticesRow : rkVertices)
		{
			if (firstRow)
			{
				rowLength = (int)rkVerticesRow.size();
				firstRow = false;
			}
			else
			{
				if (rowLength != rkVerticesRow.size())
				{
					throw std::runtime_error("Rows have inequal lengths");
				}
			}
		}
		int columnLength = (int)rkVertices.size();

		TColgp_Array2OfPnt occtPoints(1, columnLength, 1, rowLength);
		int i = 1;
		for (const std::list<TopologicCore::Vertex::Ptr>& rkVerticesRow : rkVertices)
		{
			int j = 1;
			for (const TopologicCore::Vertex::Ptr& kpVertex : rkVerticesRow)
			{
				occtPoints.SetValue(i, j, kpVertex->Point()->Pnt());
				j++;
			}
			i++;
		}
		try {
#ifdef _WIN32
			Handle(Geom_Surface) pSurface = GeomAPI_PointsToBSplineSurface(occtPoints).Surface();
			return TopologicCore::Face::BySurface(pSurface);
#else
			/* doesn't compile with gcc: error: conversion from ‘handle<Geom_BSplineSurface>’ to non-scalar type ‘handle<Geom_Surface>’ requested
			 */
#endif
		}
		catch (...)
		{
			throw std::runtime_error("Fails to create a face by vertices");
		}
	}

	void FaceUtility::ParametersAtVertex(const TopologicCore::Face::Ptr& kpFace, const TopologicCore::Vertex::Ptr& kpVertex, double& rU, double& rV)
	{
		Handle(Geom_Surface) pOcctSurface = kpFace->Surface();
		ShapeAnalysis_Surface occtSurfaceAnalysis(pOcctSurface);
		gp_Pnt2d occtUV = occtSurfaceAnalysis.ValueOfUV(kpVertex->Point()->Pnt(), Precision::Confusion());

		// Non-normalized UV
		double occtU = occtUV.X();
		double occtV = occtUV.Y();

		NormalizeUV(kpFace, occtU, occtV, rU, rV);
	}

	TopologicCore::Vertex::Ptr FaceUtility::VertexAtParameters(const TopologicCore::Face::Ptr& kpFace, const double kU, const double kV)
	{
		Handle(Geom_Surface) pOcctSurface = kpFace->Surface();
		double occtU = 0.0, occtV = 0.0;
		NonNormalizeUV(kpFace, kU, kV, occtU, occtV);

		double occtMinU = 0.0, occtMaxU = 0.0, occtMinV = 0.0, occtMaxV = 0.0;
		ShapeAnalysis::GetFaceUVBounds(kpFace->GetOcctFace(), occtMinU, occtMaxU, occtMinV, occtMaxV);
		ShapeAnalysis_Surface occtSurfaceAnalysis(kpFace->Surface());
		const Handle(Geom_Surface)& rSurface = occtSurfaceAnalysis.Surface();
		Handle(Geom_RectangularTrimmedSurface) pTrimmedSurface = new Geom_RectangularTrimmedSurface(rSurface, occtMinU + 0.0001, occtMaxU - 0.0001, occtMinV + 0.0001, occtMaxV - 0.0001);
		Handle(Geom_BSplineSurface) pBSplineSurface = GeomConvert::SurfaceToBSplineSurface(pTrimmedSurface);

		gp_Pnt occtPoint = pBSplineSurface->Value(occtU, occtV);
		TopologicCore::Vertex::Ptr vertex = TopologicCore::Vertex::ByPoint(new Geom_CartesianPoint(occtPoint));

		return TopologicCore::Vertex::ByPoint(new Geom_CartesianPoint(occtPoint));
	}

	gp_Dir FaceUtility::NormalAtParameters(const TopologicCore::Face::Ptr& kpFace, const double kU, const double kV)
	{
		Handle(Geom_Surface) pOcctSurface = kpFace->Surface();
		double occtU = 0.0, occtV = 0.0;
		NonNormalizeUV(kpFace, kU, kV, occtU, occtV);

		GeomLProp_SLProps occtProperties(kpFace->Surface(), occtU, occtV, 1, Precision::Confusion());

		gp_Dir occtNormal = occtProperties.Normal();
		if (kpFace->IsReversed())
		{
			occtNormal.Reverse();
		}
		return occtNormal;

	}

	void FaceUtility::UVSamplePoints(
		const TopologicCore::Face::Ptr& kpFace,
		const std::list<double>& rkUValues,
		const std::list<double>& rkVValues,
		std::list<std::list<gp_Pnt>>& rSamplesPoints,
		std::list<double>& rOcctUValues,
		std::list<double>& rOcctVValues,
		int& rNumUPoints,
		int& rNumVPoints,
		int& rNumUPanels,
		int& rNumVPanels,
		bool& rIsUClosed,
		bool& rIsVClosed)
	{
		// Subdivide the face in the UV space and find the points
		double occtUMin = 0.0, occtUMax = 0.0, occtVMin = 0.0, occtVMax = 0.0;
		//ShapeFix_Face occtShapeFix(kpFace->GetOcctFace());
		//occtShapeFix.Perform();
		ShapeAnalysis::GetFaceUVBounds(kpFace->GetOcctFace(), occtUMin, occtUMax, occtVMin, occtVMax);
		double occtURange = occtUMax - occtUMin;
		double occtVRange = occtVMax - occtVMin;
		rNumUPanels = (int)rkUValues.size() - 1;
		rNumVPanels = (int)rkVValues.size() - 1;

		Handle(Geom_Surface) pOcctWallSurface = kpFace->Surface();
		rIsUClosed = pOcctWallSurface->IsUClosed();
		rNumUPoints = rNumUPanels;
		if (!rIsUClosed)
		{
			rNumUPoints += 1;
		}
		rIsVClosed = pOcctWallSurface->IsVClosed();
		rNumVPoints = rNumVPanels;
		if (!rIsVClosed)
		{
			rNumVPoints += 1;
		}

		// Compute OCCT's non-normalized UV values
		// At the same time, get the isolines
		TopTools_ListOfShape occtIsolines;
		
		for (double u : rkUValues)
		{
			double occtU = occtUMin + occtURange * u;
			if (occtU < occtUMin) {
				occtU = occtUMin;
			}
			else if (occtU > occtUMax)
			{
				occtU = occtUMax;
			}
			rOcctUValues.push_back(occtU);
		};
		
		for (double v : rkVValues)
		{
			double occtV = occtVMin + occtVRange * v;
			if (occtV < occtVMin) {
				occtV = occtVMin;
			}
			else if (occtV > occtVMax)
			{
				occtV = occtVMax;
			}
			rOcctVValues.push_back(occtV);
		};

		// Sample the points
		int numOfPoints = rNumUPoints * rNumVPoints;
		int i = 0;
		std::list<double>::const_iterator endUIterator = rOcctUValues.end();
		if (rIsUClosed)
		{
			endUIterator--;
		}
		std::list<double>::const_iterator endVIterator = rOcctVValues.end();
		if (rIsVClosed)
		{
			endVIterator--;
		}

		for (std::list<double>::const_iterator uIterator = rOcctUValues.begin();
			uIterator != endUIterator;
			uIterator++)
		{
			std::list<gp_Pnt> rowSamplePoints;
			const double& rkU = *uIterator;
			for (std::list<double>::const_iterator vIterator = rOcctVValues.begin();
				vIterator != endVIterator;
				vIterator++)
			{
				const double& rkV = *vIterator;
				gp_Pnt occtPoint = pOcctWallSurface->Value(rkU, rkV);
				rowSamplePoints.push_back(occtPoint);
			}
			rSamplesPoints.push_back(rowSamplePoints);
		}
	}

	TopologicCore::Face::Ptr TrimByWireImpl(const TopologicCore::Face::Ptr& kpFace, const TopoDS_Wire& rkOcctWire, const bool kReverseWire)
	{
		Handle(Geom_Surface) pOcctSurface = kpFace->Surface();

		ShapeFix_Wire wireFix;
		wireFix.Load(rkOcctWire);
		wireFix.Perform();

		TopoDS_Wire trimmingWire;
		if (kReverseWire)
		{
			trimmingWire = TopoDS::Wire(wireFix.Wire().Reversed());
		}
		else
		{
			trimmingWire = TopoDS::Wire(wireFix.Wire());
		}
		BRepBuilderAPI_MakeFace occtTrimMakeFace(pOcctSurface, trimmingWire);
		if (occtTrimMakeFace.Error() != BRepBuilderAPI_FaceDone)
		{
			TopologicCore::Face::Throw(occtTrimMakeFace);
		}

		TopoDS_Face coreResultingFace = occtTrimMakeFace;

		ShapeFix_Shape occtFixShape(coreResultingFace);
		occtFixShape.Perform();
		
		// Fix edges
		{
			TopTools_MapOfShape occtEdges;
			for (TopExp_Explorer occtExplorer(coreResultingFace, TopAbs_EDGE); occtExplorer.More(); occtExplorer.Next())
			{
				const TopoDS_Shape& occtCurrent = occtExplorer.Current();
				TopoDS_Edge occtEdge = TopoDS::Edge(occtCurrent);
				ShapeFix_Edge occtFixEdge;
				bool result1 = occtFixEdge.FixAddCurve3d(occtEdge);
				bool result2 = occtFixEdge.FixVertexTolerance(occtEdge);
			}
		}

		// Fix wires
		{
			TopTools_MapOfShape occtWires;
			for (TopExp_Explorer occtExplorer(coreResultingFace, TopAbs_WIRE); occtExplorer.More(); occtExplorer.Next())
			{
				const TopoDS_Shape& occtCurrent = occtExplorer.Current();
				const TopoDS_Wire& occtWire = TopoDS::Wire(occtCurrent);
				ShapeFix_Wire occtFixWire(occtWire, coreResultingFace, 0.0001);
				bool result1 = occtFixWire.Perform();
			}
		}

		ShapeFix_Face faceFix(coreResultingFace);
		int result = faceFix.Perform();

		Handle(ShapeBuild_ReShape) occtContext = new ShapeBuild_ReShape();
		occtContext->Apply(faceFix.Face());

		TopoDS_Face occtFinalFace = TopoDS::Face(ShapeFix::RemoveSmallEdges(coreResultingFace, 0.0001, occtContext));

#ifdef _DEBUG
		BRepCheck_Analyzer occtAnalyzer(occtFinalFace);
		bool isValid = occtAnalyzer.IsValid();

		BRepCheck_Face occtFaceCheck(TopoDS::Face(occtFinalFace));
		bool isUnorientable = occtFaceCheck.IsUnorientable();
		BRepCheck_Status orientationStatus = occtFaceCheck.OrientationOfWires();
		BRepCheck_Status intersectionStatus = occtFaceCheck.IntersectWires();
		BRepCheck_Status classificationStatus = occtFaceCheck.ClassifyWires();
#endif

		return std::make_shared<TopologicCore::Face>(TopoDS::Face(occtFinalFace));
	}

	TopologicCore::Face::Ptr FaceUtility::TrimByWire(const TopologicCore::Face::Ptr& kpFace, const TopologicCore::Wire::Ptr& kpWire, const bool kReverseWire)
	{
		TopologicCore::Face::Ptr pCopyInputFace = std::dynamic_pointer_cast<TopologicCore::Face>(kpFace->ShallowCopy());
		TopologicCore::Wire::Ptr pCopyInputWire = std::dynamic_pointer_cast<TopologicCore::Wire>(kpWire->ShallowCopy());
		TopologicCore::Face::Ptr pFace = TrimByWireImpl(pCopyInputFace, pCopyInputWire->GetOcctWire(), kReverseWire);
		TopologicCore::Face::Ptr pCopyOutputFace = std::dynamic_pointer_cast<TopologicCore::Face>(pFace->DeepCopy());
		TopologicCore::GlobalCluster::GetInstance().AddTopology(pCopyOutputFace->GetOcctFace());
		return pCopyOutputFace;
	}

	void FaceUtility::Triangulate(const TopologicCore::Face::Ptr & kpFace, const double kDeflection, std::list<TopologicCore::Face::Ptr>& rTriangles)
	{
		TopoDS_Face occtFace = kpFace->GetOcctFace();
		BRepMesh_IncrementalMesh occtIncrementalMesh(occtFace, kDeflection);
		TopLoc_Location occtLocation;
		Handle(Poly_Triangulation) pOcctTriangulation = BRep_Tool::Triangulation(occtFace, occtLocation);
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

	void FaceUtility::NormalizeUV(const TopologicCore::Face::Ptr& kpFace, const double kNonNormalizedU, const double kNonNormalizedV, double& rNormalizedU, double& rNormalizedV)
	{
		double occtUMin = 0.0, occtUMax = 0.0, occtVMin = 0.0, occtVMax = 0.0;
		//ShapeFix_Face occtShapeFix(kpFace->GetOcctFace());
		//occtShapeFix.Perform();
		ShapeAnalysis::GetFaceUVBounds(kpFace->GetOcctFace(), occtUMin, occtUMax, occtVMin, occtVMax);
		double occtDU = occtUMax - occtUMin;
		double occtDV = occtVMax - occtVMin;
		if (occtDU <= 0.0 || occtDV <= 0.0)
		{
			throw std::runtime_error("Negative range");
		}

		rNormalizedU = (kNonNormalizedU - occtUMin) / occtDU;
		rNormalizedV = (kNonNormalizedV - occtVMin) / occtDV;
	}

	void FaceUtility::NonNormalizeUV(const TopologicCore::Face::Ptr& kpFace, const double kNormalizedU, const double kNormalizedV, double& rNonNormalizedU, double& rNonNormalizedV)
	{
		double occtUMin = 0.0, occtUMax = 0.0, occtVMin = 0.0, occtVMax = 0.0;
		//ShapeFix_Face occtShapeFix(kpFace->GetOcctFace());
		//occtShapeFix.Perform();
		//ShapeAnalysis::GetFaceUVBounds(occtShapeFix.Face(), occtUMin, occtUMax, occtVMin, occtVMax);
		ShapeAnalysis::GetFaceUVBounds(kpFace->GetOcctFace(), occtUMin, occtUMax, occtVMin, occtVMax);
		double occtDU = occtUMax - occtUMin;
		double occtDV = occtVMax - occtVMin;

		rNonNormalizedU = occtUMin + kNormalizedU * occtDU;
		rNonNormalizedV = occtVMin + kNormalizedV * occtDV;
	}
	
	void FaceUtility::AdjacentShells(const TopologicCore::Face::Ptr & kpFace, const TopologicCore::Topology::Ptr & kpParentTopology, std::list<TopologicCore::Shell::Ptr>& rCoreAdjacentShells)
	{
		std::list<TopologicCore::Topology::Ptr> coreAdjacentTopologies;
		kpFace->UpwardNavigation(kpParentTopology->GetOcctShape(), TopologicCore::Shell::Type(), coreAdjacentTopologies);
		for (const TopologicCore::Topology::Ptr& kpAdjacentTopology : coreAdjacentTopologies)
		{
			rCoreAdjacentShells.push_back(
				TopologicCore::TopologicalQuery::Downcast<TopologicCore::Shell>(kpAdjacentTopology)
			);
		}
	}
	
	void FaceUtility::AdjacentCells(const TopologicCore::Face::Ptr & kpFace, const TopologicCore::Topology::Ptr & kpParentTopology, std::list<TopologicCore::Cell::Ptr>& rCoreAdjacentCells)
	{
        AdjacentCells(kpFace.get(), kpParentTopology, rCoreAdjacentCells);
	}

    void FaceUtility::AdjacentCells(TopologicCore::Face const * const pkpFace, const TopologicCore::Topology::Ptr & kpParentTopology, std::list<TopologicCore::Cell::Ptr>& rCoreAdjacentCells)
    {
        std::list<TopologicCore::Topology::Ptr> coreAdjacentTopologies;
        pkpFace->UpwardNavigation(kpParentTopology->GetOcctShape(), TopologicCore::Cell::Type(), coreAdjacentTopologies);
        for (const TopologicCore::Topology::Ptr& kpAdjacentTopology : coreAdjacentTopologies)
        {
            rCoreAdjacentCells.push_back(
                TopologicCore::TopologicalQuery::Downcast<TopologicCore::Cell>(kpAdjacentTopology)
            );
        }
    }

    void FaceUtility::AdjacentFaces(TopologicCore::Face const * const pkpFace, const TopologicCore::Topology::Ptr & kpParentTopology, std::list<TopologicCore::Face::Ptr>& rCoreAdjacentFaces)
    {
        // Iterate through the edges and find the incident faces which are not this face.
        TopTools_IndexedDataMapOfShapeListOfShape occtEdgeFaceMap;
        TopExp::MapShapesAndUniqueAncestors(kpParentTopology->GetOcctShape(), TopAbs_EDGE, TopAbs_FACE, occtEdgeFaceMap);

        // Find the constituent faces
        TopTools_MapOfShape occtEdges;
        for (TopExp_Explorer occtExplorer(pkpFace->GetOcctShape(), TopAbs_EDGE); occtExplorer.More(); occtExplorer.Next())
        {
            const TopoDS_Shape& occtCurrent = occtExplorer.Current();
            if (!occtEdges.Contains(occtCurrent))
            {
                occtEdges.Add(occtCurrent);
            }
        }

        const TopoDS_Face& rkOcctFace = pkpFace->GetOcctFace();
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
            rCoreAdjacentFaces.push_back(std::make_shared<TopologicCore::Face>(TopoDS::Face(occtAdjacentFaceIterator.Value())));
        }
    }

	// https://www.opencascade.com/content/how-find-if-point-belongs-face
	// https://www.opencascade.com/doc/occt-7.2.0/refman/html/class_b_rep_class___face_classifier.html
	bool FaceUtility::IsInside(const TopologicCore::Face::Ptr kpFace, const TopologicCore::Vertex::Ptr & kpVertex, const double kTolerance)
	{
		double u = 0.0, v = 0.0;
		ParametersAtVertex(kpFace, kpVertex, u, v);

		gp_Dir normal = NormalAtParameters(kpFace, u, v);

		gp_Pnt occtInputPoint = kpVertex->Point()->Pnt();

		gp_Pnt pointA = occtInputPoint.Translated(gp_Vec(kTolerance * normal));
		gp_Pnt pointB = occtInputPoint.Translated(gp_Vec(kTolerance * (-normal)));

		TopologicCore::Vertex::Ptr vertexA = TopologicCore::Vertex::ByPoint(new Geom_CartesianPoint(pointA));
		TopologicCore::Vertex::Ptr vertexB = TopologicCore::Vertex::ByPoint(new Geom_CartesianPoint(pointB));

		TopologicCore::Edge::Ptr edge = TopologicCore::Edge::ByStartVertexEndVertex(vertexA, vertexB, false);

		TopologicCore::Topology::Ptr sliceResult = edge->Slice(kpFace);
		
		// Why?
		if (sliceResult == nullptr)
		{
			return false;
		}
		std::list<TopologicCore::Vertex::Ptr> vertices;
		sliceResult->Vertices(vertices);

		if (vertices.size() <= 2)
		{
			return false;
		}

		std::list<TopologicCore::Vertex::Ptr> faceVertices;
		kpFace->Vertices(faceVertices);
		std::list<TopologicCore::Edge::Ptr> faceEdges;
		kpFace->Edges(faceEdges);
			
		// For each point not close to vertexA or vertexB, if the distance to 
		// existing edges or vertices is under tolerance, reject it.
		std::list<TopologicCore::Vertex::Ptr> rejectedVertices;
		for (const TopologicCore::Vertex::Ptr& kpVertex : vertices)
		{
			if (VertexUtility::Distance(kpVertex, vertexA) < kTolerance &&
				VertexUtility::Distance(kpVertex, vertexB) < kTolerance)
			{
				continue;
			}

			for (const TopologicCore::Vertex::Ptr& kpFaceVertex : faceVertices)
			{
				if (VertexUtility::Distance(kpVertex, kpFaceVertex) < kTolerance)
				{
					rejectedVertices.push_back(kpVertex);
					break;
				}
			}

			for (const TopologicCore::Edge::Ptr& kpFaceEdge : faceEdges)
			{
				if (VertexUtility::Distance(kpVertex, kpFaceEdge) < kTolerance)
				{
					rejectedVertices.push_back(kpVertex);
					break;
				}
			}
		}

		if (vertices.size() - rejectedVertices.size() <= 2)
		{
			return false;
		}

		return true;
	}

	TopologicCore::Vertex::Ptr FaceUtility::InternalVertex(const TopologicCore::Face::Ptr kpFace, const double kTolerance)
	{
		// Check the centroid first
		TopologicCore::Vertex::Ptr centerOfMass = kpFace->CenterOfMass();
		if (IsInside(kpFace, centerOfMass, kTolerance))
		{
			return centerOfMass;
		}

		// METHOD 1: subdivision on the UV space
		// 1: 0.5	(divide by 2^1 = 2)
		// 1: 0.25, 0.5, 0.75	(divide by 2^2 = 4)
		// 2: 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875 (etc...)
		int level = 1;
		int maxLevel = 10;
		std::list<double> parameters;
		do {
			++level;

			double numOfSubdivisions = pow(2, level);
			double delta = 1 / numOfSubdivisions;

			for (int i = 1; i < numOfSubdivisions; ++i)
			{
				double u = i * delta;
				for (int j = 1; j < numOfSubdivisions; ++j)
				{
					double v = j * delta;
					try {
						TopologicCore::Vertex::Ptr point = VertexAtParameters(kpFace, u, v);

						if (IsInside(kpFace, point, kTolerance))
						{
							return point;
						}
					}
					catch (const Standard_ConstructionError& occtError)
					{
						throw std::runtime_error(occtError.GetMessageString());
					}
				}
			}
		} while (level <= maxLevel);

		return nullptr;
	}

	TopologicCore::Vertex::Ptr FaceUtility::ProjectToSurface(const TopologicCore::Face::Ptr kpFace, const TopologicCore::Vertex::Ptr& kpVertex)
	{
		GeomAPI_ProjectPointOnSurf occtProjectToSurface(kpVertex->Point()->Pnt(), kpFace->Surface());
		if (!occtProjectToSurface.IsDone())
		{
			throw std::runtime_error("Fails to project a Vertex to a Face.");
		}
		gp_Pnt occtProjectedPoint = occtProjectToSurface.NearestPoint();
		return TopologicCore::Vertex::ByPoint(new Geom_CartesianPoint(occtProjectedPoint));
	}

}
