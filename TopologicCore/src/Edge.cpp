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

#include "Edge.h"
#include "Vertex.h"
#include "Wire.h"
#include "Face.h"
#include "Shell.h"
#include "Cell.h"
#include "CellComplex.h"
#include "EdgeFactory.h"
#include "Cluster.h"
#include "AttributeManager.h"

#include <Utilities/EdgeUtility.h>

#include <BRepGProp.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Tool.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GProp_GProps.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeFix_Shape.hxx>
#include <TopoDS.hxx>

#include <assert.h>

namespace TopologicCore
{
	void Edge::AdjacentEdges(const Topology::Ptr& kpHostTopology, std::list<Edge::Ptr>& rAdjacentEdges) const
	{
		std::list<Vertex::Ptr> vertices;
		Vertices(kpHostTopology, vertices);

		// Find the constituent edges
		TopTools_MapOfShape occtAdjacentEdges;
		for (const Vertex::Ptr& kpVertex : vertices)
		{
			// Find the edges
			std::list<Edge::Ptr> edges;
			kpVertex->Edges(kpHostTopology, edges);

			for (const Edge::Ptr& kpEdge : edges)
			{
				// If not the same as this Edge, add to the list
				if (!IsSame(kpEdge))
				{
					occtAdjacentEdges.Add(kpEdge->GetOcctShape());
				}
			}
		}

		for (TopTools_MapIteratorOfMapOfShape occtAdjacentEdgeIterator(occtAdjacentEdges);
			occtAdjacentEdgeIterator.More();
			occtAdjacentEdgeIterator.Next())
		{
			rAdjacentEdges.push_back(std::make_shared<Edge>(TopoDS::Edge(occtAdjacentEdgeIterator.Value())));
		}
	}

	Vertex::Ptr Edge::StartVertex() const
	{
		return std::make_shared<Vertex>(StartVertex(GetOcctEdge()));
	}

	TopoDS_Vertex Edge::StartVertex(const TopoDS_Edge& rkOcctEdge)
	{
		ShapeAnalysis_Edge occtShapeAnalysisEdge;
		TopoDS_Vertex occtFirstVertex = occtShapeAnalysisEdge.FirstVertex(rkOcctEdge);
		return occtFirstVertex;
	}

	TopoDS_Vertex Edge::EndVertex(const TopoDS_Edge& rkOcctEdge)
	{
		ShapeAnalysis_Edge occtShapeAnalysisEdge;
		TopoDS_Vertex occtLastVertex = occtShapeAnalysisEdge.LastVertex(rkOcctEdge);
		return occtLastVertex;
	}

	std::shared_ptr<Vertex> Edge::EndVertex() const
	{
		return std::make_shared<Vertex>(EndVertex(GetOcctEdge()));
	}

	void Edge::Vertices(const Topology::Ptr& kpHostTopology, std::list<Vertex::Ptr>& rVertices) const
	{
		rVertices.push_back(StartVertex());
		rVertices.push_back(EndVertex());
	}

	void Edge::Wires(const Topology::Ptr& kpHostTopology, std::list<Wire::Ptr>& rWires) const
	{
		if (kpHostTopology)
		{
			UpwardNavigation(kpHostTopology->GetOcctShape(), rWires);
		}
		else
		{
			throw std::runtime_error("Host Topology cannot be NULL when searching for ancestors.");
		}
	}

	void Edge::Faces(const Topology::Ptr& kpHostTopology, std::list<Face::Ptr>& rFaces) const
	{
		if (kpHostTopology)
		{
			UpwardNavigation(kpHostTopology->GetOcctShape(), rFaces);
		}
		else
		{
			throw std::runtime_error("Host Topology cannot be NULL when searching for ancestors.");
		}
	}

	Edge::Ptr Edge::ByCurve(
		const TColgp_Array1OfPnt &rkOcctPoles,
		const TColStd_Array1OfReal &rkOcctWeights,
		const TColStd_Array1OfReal &rkOcctKnots,
		const TColStd_Array1OfInteger &rkOcctMultiplicities,
		const int kDegree,
		const bool kIsPeriodic,
		const bool kIsRational)
	{
		BRepBuilderAPI_MakeEdge occtMakeEdge;
		Handle(Geom_BSplineCurve) pOcctBSplineCurve = nullptr;
		try {
			pOcctBSplineCurve = new Geom_BSplineCurve(
				rkOcctPoles,
				rkOcctWeights,
				rkOcctKnots,
				rkOcctMultiplicities,
				kDegree,
				false,
				kIsRational);
		}
		catch (Standard_Failure e)
		{
			throw std::runtime_error(e.GetMessageString());
		}

		// Done separately, because SetPeriod() does additional steps to adjust the poles, weights, etc.
		if (kIsPeriodic)
		{
			try{
				pOcctBSplineCurve->SetPeriodic();
			}
			catch (Standard_Failure e)
			{
				throw std::runtime_error(e.GetMessageString());
			}
		}

		return ByCurve(pOcctBSplineCurve);
	}

	Edge::Ptr Edge::ByCurve(Handle(Geom_Curve) pOcctCurve, const double rkFirstParameter, const double rkLastParameter)
	{
		const double kOcctFirstParameter = pOcctCurve->FirstParameter();
		const double kOcctLastParameter = pOcctCurve->LastParameter();
		const double kOcctDeltaParameter = kOcctLastParameter - kOcctFirstParameter;

		// Compute the non-normalised parameters.
		const double kOcctParameter1 = kOcctFirstParameter + rkFirstParameter * kOcctDeltaParameter;
		const double kOcctParameter2 = kOcctFirstParameter + rkLastParameter * kOcctDeltaParameter;

		BRepBuilderAPI_MakeEdge occtMakeEdge(pOcctCurve, kOcctParameter1, kOcctParameter2);
		if (occtMakeEdge.Error() != BRepBuilderAPI_EdgeDone)
		{
			Throw(occtMakeEdge.Error());
		}

		TopoDS_Edge occtFixedEdge = OcctShapeFix(occtMakeEdge);
		Edge::Ptr pEdge = std::make_shared<Edge>(occtFixedEdge);
		return pEdge;
	}

	Edge::Ptr Edge::ByStartVertexEndVertex(const std::shared_ptr<Vertex>& kpStartVertex, const std::shared_ptr<Vertex>& kpEndVertex)
	{
		if (kpStartVertex == nullptr || kpEndVertex == nullptr)
		{
			return nullptr;
		}

		BRepBuilderAPI_MakeEdge occtMakeEdge(
			kpStartVertex->GetOcctVertex(),
			kpEndVertex->GetOcctVertex());
		if (occtMakeEdge.Error() != BRepBuilderAPI_EdgeDone)
		{
			Throw(occtMakeEdge.Error());
		}

		TopoDS_Edge occtFixedEdge = OcctShapeFix(occtMakeEdge);

		return std::make_shared<Edge>(occtFixedEdge);
	}

	void Edge::SharedVertices(const Edge::Ptr& kpAnotherEdge, std::list<std::shared_ptr<Vertex>>& rSharedVertices) const
	{
		const TopoDS_Shape& rkOcctShape1 = GetOcctShape();
		TopTools_MapOfShape occtVertices1;
		DownwardNavigation(rkOcctShape1, TopAbs_VERTEX, occtVertices1);

		const TopoDS_Shape& rkOcctShape2 = kpAnotherEdge->GetOcctShape();
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
					rSharedVertices.push_back(pVertex);
				}
			}
		}
	}

	bool Edge::IsManifold(const Topology::Ptr& kpHostTopology) const
	{
		// In the context of a Wire, an Edge is non-manifold if it connects more than two edges.
		if (kpHostTopology->GetType() == TOPOLOGY_WIRE)
		{
			std::list<Edge::Ptr> edges;
			AdjacentEdges(kpHostTopology, edges);
			if (edges.size() > 2)
			{
				return false;
			}
		}
		// In the context of a Face, an Edge is non-manifold if it connects more than two edges.
		if (kpHostTopology->GetType() == TOPOLOGY_FACE)
		{
			std::list<Edge::Ptr> edges;
			AdjacentEdges(kpHostTopology, edges);
			if (edges.size() > 2)
			{
				return false;
			}
		}
		// In the context of a Shell, an Edge is non-manifold if it connects more than one Face.
		if (kpHostTopology->GetType() == TOPOLOGY_SHELL)
		{
			std::list<Face::Ptr> faces;
			Faces(kpHostTopology, faces);
			if (faces.size() > 1)
			{
				return false;
			}
		}
		// In the context of a Cell, an Edge is non-manifold if it connects more than two Faces.
		if (kpHostTopology->GetType() == TOPOLOGY_CELL)
		{
			std::list<Face::Ptr> faces;
			Faces(kpHostTopology, faces);
			if (faces.size() > 2)
			{
				return false;
			}
		}
		// In the context of a CellComplex, an Edge is non-manifold if it connects more than one Cell.
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

	void Edge::Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const
	{
		rOcctGeometries.push_back(Curve());
	}

	TopoDS_Shape& Edge::GetOcctShape()
	{
		return GetOcctEdge();
	}

	const TopoDS_Shape& Edge::GetOcctShape() const
	{
		return GetOcctEdge();
	}

	TopoDS_Edge& Edge::GetOcctEdge()
	{
		assert(!m_occtEdge.IsNull() && "Edge::m_occtEdge is null.");
		if (m_occtEdge.IsNull())
		{
			throw std::runtime_error("A null Edge is encountered.");
		}

		return m_occtEdge;
	}

	const TopoDS_Edge& Edge::GetOcctEdge() const
	{
		assert(!m_occtEdge.IsNull() && "Edge::m_occtEdge is null.");
		if (m_occtEdge.IsNull())
		{
			throw std::runtime_error("A null Edge is encountered.");
		}

		return m_occtEdge;
	}

	void Edge::SetOcctShape(const TopoDS_Shape& rkOcctShape)
	{
		try {
			SetOcctEdge(TopoDS::Edge(rkOcctShape));
		}
		catch (Standard_Failure e)
		{
			throw std::runtime_error(e.GetMessageString());
		}
	}

	void Edge::SetOcctEdge(const TopoDS_Edge& rkOcctEdge)
	{
		m_occtEdge = rkOcctEdge;
	}

	Handle(Geom_Curve) Edge::Curve() const
	{
		double firstParameter = 0.0, lastParameter = 0.0;
		return Curve(firstParameter, lastParameter);
	}

	Handle(Geom_Curve) Edge::Curve(double& rFirstParameter, double& rLastParameter) const
	{
		return BRep_Tool::Curve(GetOcctEdge(), rFirstParameter, rLastParameter);
	}

	double Edge::NormalizeParameter(const double kOcctFirstParameter, const double kOcctLastParameter, const double kNonNormalizedParameter)
	{
		double occtDParameter = kOcctLastParameter - kOcctFirstParameter;
		if (occtDParameter <= 0.0)
		{
			throw std::runtime_error("Negative range");
		}

		return (kNonNormalizedParameter - kOcctFirstParameter) / occtDParameter;
	}

	double Edge::NonNormalizeParameter(const double kOcctFirstParameter, const double kOcctLastParameter, const double kNormalizedParameter)
	{
		double occtDParameter = kOcctLastParameter - kOcctFirstParameter;
		return kOcctFirstParameter + kNormalizedParameter * occtDParameter;
	}

	TopoDS_Edge Edge::OcctShapeFix(const TopoDS_Edge& rkOcctInputEdge)
	{
		ShapeFix_Shape occtEdgeFix(rkOcctInputEdge);
		occtEdgeFix.Perform();
		return TopoDS::Edge(occtEdgeFix.Shape());
	}

	std::shared_ptr<Vertex> Edge::CenterOfMass() const
	{
		TopoDS_Vertex occtCenterOfMass = CenterOfMass(GetOcctEdge());
		return std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtCenterOfMass));
	}

	TopoDS_Vertex Edge::CenterOfMass(const TopoDS_Edge& rkOcctEdge)
	{
		GProp_GProps occtShapeProperties;
		BRepGProp::LinearProperties(rkOcctEdge, occtShapeProperties);
		return BRepBuilderAPI_MakeVertex(occtShapeProperties.CentreOfMass());
	}

	std::string Edge::GetTypeAsString() const
	{
		return std::string("Edge");
	}

	void Edge::Throw(const BRepBuilderAPI_EdgeError occtEdgeError)
	{
		switch (occtEdgeError)
		{
		case BRepBuilderAPI_PointProjectionFailed:
			throw std::runtime_error("No parameters were given but the projection of the 3D points on the curve failed. This happens when the point distance to the curve is greater than the precision value.");
			
		case BRepBuilderAPI_ParameterOutOfRange:
			throw std::runtime_error("The given parameters are not in the parametric range.");

		case BRepBuilderAPI_DifferentPointsOnClosedCurve:
			throw std::runtime_error("The two vertices or points are the extremities of a closed curve but have different locations.");

		case BRepBuilderAPI_PointWithInfiniteParameter:
			throw std::runtime_error("A finite coordinate point was associated with an infinite parameter.");

		case BRepBuilderAPI_DifferentsPointAndParameter:
			throw std::runtime_error("The distance between the 3D point and the point evaluated on the curve with the parameter is greater than the precision.");

		default: //case BRepBuilderAPI_LineThroughIdenticPoints:
			throw std::runtime_error("Two identical points were given to define a line (construction of an edge without curve).");
		}
	}

	Edge::Edge(const TopoDS_Edge& rkOcctEdge, const std::string& rkGuid)
		: Topology(1, rkOcctEdge, rkGuid.compare("") == 0 ? GetClassGUID() : rkGuid)
		, m_occtEdge(rkOcctEdge)
	{
		RegisterFactory(GetClassGUID(), std::make_shared<EdgeFactory>());
	}

	Edge::~Edge()
	{

	}
}
