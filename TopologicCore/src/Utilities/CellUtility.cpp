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

#include "Utilities/CellUtility.h"
#include "Utilities/FaceUtility.h"
#include "Utilities/VertexUtility.h"

#include <Cell.h>
#include <Face.h>
#include <GlobalCluster.h>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepGProp.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_CartesianPoint.hxx>
#include <GProp_GProps.hxx>
#include <Message_ProgressIndicator.hxx>
#include <ShapeFix_Solid.hxx>
#include <TopoDS.hxx>

namespace TopologicUtilities
{
	TopologicCore::Cell::Ptr CellUtility::ByLoft(const std::list<TopologicCore::Wire::Ptr>& rkWires)
	{
		BRepOffsetAPI_ThruSections occtLoft(true);
		for (const TopologicCore::Wire::Ptr& kpWire : rkWires)
		{
			occtLoft.AddWire(kpWire->GetOcctWire());
		};
		try {
			occtLoft.Build();
		}
		catch (...)
		{
			throw std::runtime_error("Loft error");
		}
		TopologicCore::Cell::Ptr pCell = std::make_shared<TopologicCore::Cell>(TopoDS::Solid(occtLoft.Shape()));
		TopologicCore::GlobalCluster::GetInstance().AddTopology(pCell->GetOcctShape());
		return pCell;
	}

	TopologicCore::Cell::Ptr CellUtility::ByCuboid(
		const double kXCentroid, const double kYCentroid, const double kZCentroid,
		const double kXDimension, const double kYDimension, const double kZDimension,
		const double kXNormal, const double kYNormal, const double kZNormal, // axis Z
		const double kXAxisX, const double kYAxisX, const double kZAxisX,
		const double kXAxisY, const double kYAxisY, const double kZAxisY)
	{
		gp_Pnt occtCentroid(kXCentroid, kYCentroid, kZCentroid);
		gp_Pnt occtTransformedLowCorner = occtCentroid.Translated(gp_Vec(-kXNormal, -kYNormal, -kZNormal).Normalized().Scaled(kZDimension / 2.0));
		occtTransformedLowCorner.Translate(gp_Vec(-kXAxisX, -kYAxisX, -kZAxisX).Normalized().Scaled(kXDimension/2.0));
		occtTransformedLowCorner.Translate(gp_Vec(-kXAxisY, -kYAxisY, -kZAxisY).Normalized().Scaled(kYDimension / 2.0));

		gp_Ax2 occtAxes(
			occtTransformedLowCorner,
			gp_Dir(gp_Vec(kXNormal, kYNormal, kZNormal).Normalized()),
			gp_Dir(gp_Vec(kXAxisX, kYAxisX, kZAxisX).Normalized()));
		BRepPrimAPI_MakeBox occtMakeBox(occtAxes, kXDimension, kYDimension, kZDimension);
		occtMakeBox.Build();

		TopologicCore::Cell::Ptr pCell = std::make_shared<TopologicCore::Cell>(occtMakeBox.Solid());
		TopologicCore::GlobalCluster::GetInstance().AddTopology(pCell->GetOcctSolid());
		return pCell;
	}

	TopologicCore::Cell::Ptr CellUtility::BySphere(const double kCenterX, const double kCenterY, const double kCenterZ, const double kRadius)
	{
		BRepPrimAPI_MakeSphere occtMakeSphere(gp_Pnt(kCenterX, kCenterY, kCenterZ), kRadius);

		TopologicCore::Cell::Ptr pCell = std::make_shared<TopologicCore::Cell>(occtMakeSphere.Solid());
		TopologicCore::GlobalCluster::GetInstance().AddTopology(pCell->GetOcctSolid());
		return pCell;
	}

	TopologicCore::Cell::Ptr CellUtility::ByTwoCorners(const std::shared_ptr<TopologicCore::Vertex>& kpMinVertex, const std::shared_ptr<TopologicCore::Vertex>& kpMaxVertex)
	{
		BRepPrimAPI_MakeBox occtMakeBox(
			kpMinVertex->Point()->Pnt(), 
			kpMaxVertex->Point()->Pnt());

		TopologicCore::Cell::Ptr pCell = std::make_shared<TopologicCore::Cell>(occtMakeBox.Solid());
		TopologicCore::GlobalCluster::GetInstance().AddTopology(pCell->GetOcctSolid());
		return pCell;
		return TopologicCore::Cell::Ptr();
	}

	TopologicCore::Cell::Ptr CellUtility::ByCylinder(const double kReferencePointX, const double kReferencePointY, const double kReferencePointZ,
		const double kNormalX, const double kNormalY, const double kNormalZ,
		const double kRadius, const double kHeight)
	{
		BRepPrimAPI_MakeCylinder occtMakeCylinder(
			gp_Ax2(
				gp_Pnt(kReferencePointX, kReferencePointY, kReferencePointZ),
				gp_Dir(kNormalX, kNormalY, kNormalZ)),
			kRadius, kHeight);
		TopologicCore::Cell::Ptr pCell = std::make_shared<TopologicCore::Cell>(occtMakeCylinder.Solid());
		TopologicCore::GlobalCluster::GetInstance().AddTopology(pCell->GetOcctSolid());
		return pCell;
	}

	std::shared_ptr<TopologicCore::Cell> CellUtility::ByCone(
		const double kReferencePointX, const double kReferencePointY, const double kReferencePointZ,
		const double kNormalX, const double kNormalY, const double kNormalZ,
		const double kXDirectionX, const double kXDirectionY, const double kDirectionZ,
		const double kRadius1, const double kRadius2, const double kHeight)
	{
		BRepPrimAPI_MakeCone occtMakeCone(
			gp_Ax2(
				gp_Pnt(kReferencePointX, kReferencePointY, kReferencePointZ),
				gp_Dir(kNormalX, kNormalY, kNormalZ),
				gp_Dir(kXDirectionX, kXDirectionY, kDirectionZ)),
			kRadius1, kRadius2, kHeight);
		TopologicCore::Cell::Ptr pCell = std::make_shared<TopologicCore::Cell>(occtMakeCone.Solid());
		TopologicCore::GlobalCluster::GetInstance().AddTopology(pCell->GetOcctSolid());
		return pCell;
	}

	TopologicCore::Vertex::Ptr CellUtility::InternalVertex(const TopologicCore::Cell::Ptr kpCell, const double kTolerance)
	{
		// Check the centroid first
		TopologicCore::Vertex::Ptr centerOfMass = kpCell->CenterOfMass();
		if (Contains(kpCell, centerOfMass, kTolerance) == INSIDE)
		{
			return centerOfMass;
		}

		// This methods accepts as input a Cell and outputs a Vertex guaranteed to be inside the Cell. 
		// This method relies on #3 to first choose a Vertex on a random Face of the Cell, 
		// then it shoots a Ray in the opposite direction of its normal and finds the closest intersection. 
		// The midpoint is returned as a Vertex guaranteed to be inside the Cell.
		
		// Pick random Face
		std::list<TopologicCore::Face::Ptr> faces;
		kpCell->Faces(faces);
		
		for (const TopologicCore::Face::Ptr& kpFace : faces)
		{
			TopologicCore::Vertex::Ptr vertexInFace = FaceUtility::InternalVertex(kpFace, kTolerance);

			// Get the bounding box diagonal length
			double minX = 0.0, maxX = 0.0, minY = 0.0, maxY = 0.0, minZ = 0.0, maxZ = 0.0;
			GetMinMax(kpCell, minX, maxX, minY, maxY, minZ, maxZ);
			TopologicCore::Vertex::Ptr corner1 = TopologicCore::Vertex::ByCoordinates(minX, minY, minZ);
			TopologicCore::Vertex::Ptr corner2 = TopologicCore::Vertex::ByCoordinates(maxX, maxY, maxZ);
			double diagonalLength = VertexUtility::Distance(corner1, corner2);

			// Get the normal at vertexInFace
			double u = 0.0, v = 0.0;
			FaceUtility::ParametersAtVertex(kpFace, vertexInFace, u, v);
			gp_Dir occtNormal = FaceUtility::NormalAtParameters(kpFace, u, v);
			gp_Dir occtReversedNormal = occtNormal.Reversed();

			gp_Pnt occtRayEnd = vertexInFace->Point()->Pnt().Translated(diagonalLength * occtReversedNormal);
			TopologicCore::Vertex::Ptr rayEnd = TopologicCore::Vertex::ByPoint(new Geom_CartesianPoint(occtRayEnd));

			// Shoot the ray = edge vs cell intersection
			TopologicCore::Edge::Ptr ray = TopologicCore::Edge::ByStartVertexEndVertex(vertexInFace, rayEnd);
			TopologicCore::Topology::Ptr rayShot = kpCell->Intersect(ray);

			// Get the vertices of the ray
			std::list<TopologicCore::Vertex::Ptr> rayVertices;
			rayShot->Vertices(rayVertices);

			// Get the closest intersection (but not the original vertex in face)
			double minDistance = std::numeric_limits<double>::max();
			TopologicCore::Vertex::Ptr closestVertex = nullptr;
			for (const TopologicCore::Vertex::Ptr& kpRayVertex : rayVertices)
			{
				double distance = VertexUtility::Distance(kpRayVertex, vertexInFace);
				if (distance < kTolerance) // the same vertex
				{
					continue;
				}

				if (distance < minDistance)
				{
					minDistance = distance;
					closestVertex = kpRayVertex;
				}
			}

			if (closestVertex == nullptr)
			{
				throw std::runtime_error("Ray casting fails to identify the closest vertex from a random point.");
			}

			// Create a line between the closest vertex and vertex in Face, then get the midpoint (center of mass)
			TopologicCore::Edge::Ptr shortestEdge = TopologicCore::Edge::ByStartVertexEndVertex(vertexInFace, closestVertex);
			TopologicCore::Vertex::Ptr shortestEdgeCenterOfMass = shortestEdge->CenterOfMass();

			return shortestEdgeCenterOfMass;
		}

		return nullptr;
	}

    TopologicCore::Vertex::Ptr CellUtility::InternalVertex(const TopoDS_Solid & rkOcctSolid, const double kTolerance)
    {
        return InternalVertex(std::make_shared<TopologicCore::Cell>(rkOcctSolid), kTolerance);
    }

    double CellUtility::Volume(const TopologicCore::Cell::Ptr & kpCell)
	{
		GProp_GProps occtShapeProperties;
		BRepGProp::VolumeProperties(kpCell->GetOcctSolid(), occtShapeProperties);
		return occtShapeProperties.Mass();
	}

	CellContainmentState CellUtility::Contains(const TopologicCore::Cell::Ptr & kpCell, const TopologicCore::Vertex::Ptr & kpVertex, const double kTolerance)
	{
		BRepClass3d_SolidClassifier occtSolidClassifier(kpCell->GetOcctSolid(), kpVertex->Point()->Pnt(), Precision::Confusion());
		TopAbs_State occtState = occtSolidClassifier.State();
		
		if (occtState == TopAbs_IN)
		{
			return INSIDE;
		}
		else if (occtState == TopAbs_ON)
		{
			return ON_BOUNDARY;
		}
		else if (occtState == TopAbs_OUT)
		{
			return OUTSIDE;
		}
		
		return UNKNOWN;
	}

	void CellUtility::GetMinMax(const TopologicCore::Cell::Ptr & kpCell, double & rMinX, double & rMaxX, double & rMinY, double & rMaxY, double & rMinZ, double & rMaxZ)
	{
		Bnd_Box occtBoundingBox;

		BRepBndLib::Add(kpCell->GetOcctShape(), occtBoundingBox);
		occtBoundingBox.Get(rMinX, rMinY, rMinZ, rMaxX, rMaxY, rMaxZ);
	}

}