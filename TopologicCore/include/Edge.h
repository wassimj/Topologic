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

#pragma once

#include "Topology.h"
#include "Utilities.h"

#include <list>

#include <Standard_Handle.hxx>
#include <Geom_Curve.hxx>
#include <TopoDS_Edge.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <BRepBuilderAPI_EdgeError.hxx>

namespace TopologicCore
{
	class Vertex;
	class Wire;
	class Face;

	class EdgeGUID
	{
	public:
		static std::string Get()
		{
			return std::string("1fc6e6e1-9a09-4c0a-985d-758138c49e35");
		}
	};

	class Edge : public Topology
	{
	public:
		typedef std::shared_ptr<Edge> Ptr;

	public:
		TOPOLOGIC_API Edge(const TopoDS_Edge& rkOcctEdge, const std::string& rkGuid = "");

		virtual ~Edge();

		/// <summary>
		/// Returns the Edges adjacent to the Edge.
		/// </summary>
		/// <param name="rAdjacentEdges">>A list of Edges adjacent to the Edge</param>
		TOPOLOGIC_API void AdjacentEdges(std::list<std::shared_ptr<Edge>>& rAdjacentEdges) const;

		/// <summary>
		/// Returns the start Vertex of the Edge.
		/// </summary>
		/// <returns name="Vertex">The start Vertex of the Edge</returns>
		TOPOLOGIC_API std::shared_ptr<Vertex> StartVertex() const;

		/// <summary>
		/// Returns the start OCCT Vertex of the OCCT Edge.
		/// </summary>
		/// <param name="rkOcctEdge">An OCCT Edge</param>
		/// <returns name="TopoDS_Vertex">The start OCCT Vertex of the OCCT Edge</returns>
		static TopoDS_Vertex StartVertex(const TopoDS_Edge& rkOcctEdge);
		
		/// <summary>
		/// Returns the end OCCT Vertex of the OCCT Edge.
		/// </summary>
		/// <param name="rkOcctEdge">An OCCT Edge</param>
		/// <returns>The end OCCT Vertex of the OCCT Edge</returns>
		static TopoDS_Vertex EndVertex(const TopoDS_Edge& rkOcctEdge);

		/// <summary>
		/// Returns the end Vertex of the Edge.
		/// </summary>
		/// <returns name="Vertex">The end Vertex of the Edge</returns>
		TOPOLOGIC_API std::shared_ptr<Vertex> EndVertex() const;

		/// <summary>
		/// Returns the Vertices at the ends of the Edge.
		/// </summary>
		/// <param name="rVertices">A list of Vertices at the ends of the Edge</param>
		TOPOLOGIC_API void Vertices(std::list<std::shared_ptr<Vertex>>& rVertices) const;

		/// <summary>
		/// Returns the Wires incident to the Edge.
		/// </summary>
		/// <param name="rWires">A list of Wires incident to the Edge</param>
		TOPOLOGIC_API void Wires(std::list<std::shared_ptr<Wire>>& rWires) const;

		/// <summary>
		/// Returns the Faces incident to the Edge.
		/// </summary>
		/// <param name="rFaces">A list of Faces incident to the Edge</param>
		TOPOLOGIC_API void Faces(std::list<std::shared_ptr<Face>>& rFaces) const;

		/// <summary>
		/// Creates an Edge by NURBS curve parameters
		/// </summary>
		/// <param name="rkOcctPoles">The OCCT poles</param>
		/// <param name="rkOcctWeights">The weights</param>
		/// <param name="rkOcctKnots">The knots</param>
		/// <param name="rkOcctMultiplicities">The knots' multiplicities</param>
		/// <param name="kDegree">The degree</param>
		/// <param name="kIsPeriodic">The curve's periodic status</param>
		/// <param name="kIsRational">The curve's rational status</param>
		/// <returns name="Edge">An Edge</returns>
		TOPOLOGIC_API static Edge::Ptr ByCurve(
			const TColgp_Array1OfPnt& rkOcctPoles, 
			const TColStd_Array1OfReal& rkOcctWeights, 
			const TColStd_Array1OfReal& rkOcctKnots, 
			const TColStd_Array1OfInteger& rkOcctMultiplicities, 
			const int kDegree, 
			const bool kIsPeriodic = false,
			const bool kIsRational = true);

		/// <summary>
		/// Creates an Edge by an OCCT Curve and the minimum and maximum parameters.
		/// </summary>
		/// <param name="pOcctCurve">The underlying Curve</param>
		/// <param name="rkFirstParameter">The first parameter, ranging between 0 and 1.</param>
		/// <param name="rkLastParameter">The second parameter, ranging between 0 and 1. Must be larger than rkFirstParameter, otherwise they will be swapped.</param>
		/// <returns name="Edge">An Edge</returns>
		static TOPOLOGIC_API Edge::Ptr ByCurve(Handle(Geom_Curve) pOcctCurve, const double rkFirstParameter = 0.0, const double rkLastParameter = 1.0);

		/// <summary>
		/// Creates a straight Edge by the startVertex and endVertex.
		/// </summary>
		/// <param name="kpStartVertex">The start Vertex</param>
		/// <param name="kpEndVertex">The end Vertex</param>
		/// <param name="kCopyAttributes">If True, copy the dictionaries</param>
		/// <returns name="Edge">The created Edge</returns>
		TOPOLOGIC_API static Edge::Ptr ByStartVertexEndVertex(const std::shared_ptr<Vertex>& kpStartVertex, const std::shared_ptr<Vertex>& kpEndVertex, const bool kCopyAttributes = true);

		/// <summary>
		/// Returns the shared Vertices between two Edges.
		/// </summary>
		/// <param name="kpAnotherEdge">Another Edge</param>
		/// <param name="rSharedVertices">A list of shared Vertices between the two Edges</param>
		TOPOLOGIC_API void SharedVertices(const Edge::Ptr& kpAnotherEdge, std::list<std::shared_ptr<Vertex>>& rSharedVertices) const;

		/// <summary>
		/// Returns True if this Edge is a manifold, otherwise a False.
		/// </summary>
		/// <returns name="bool">True if this Edge is a manifold, otherwise a False</returns>
		virtual bool IsManifold() const;

        virtual bool IsManifold(const TopologicCore::Topology::Ptr& rkParentTopology) const;

		/// <summary>
		/// Creates a geometry from Edge.
		/// </summary>
		/// <param name="rOcctGeometries">The created geometry</param>
		virtual void Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const;

		/// <summary>
		/// Returns the underlying OCCT shape.
		/// </summary>
		/// <returns name="TopoDS_Shape">The underlying OCCT shape</returns>
		virtual TopoDS_Shape& GetOcctShape();

		/// <summary>
		/// Returns the underlying OCCT shape.
		/// </summary>
		/// <returns name="TopoDS_Shape">The underlying OCCT shape</returns>
		virtual const TopoDS_Shape& GetOcctShape() const;

		/// <summary>
		/// Returns the underlying OCCT Edge.
		/// </summary>
		/// <returns name="TopoDS_Edge">The underlying OCCT Edge</returns>
		virtual TopoDS_Edge& GetOcctEdge();

		/// <summary>
		/// Returns the underlying OCCT Edge.
		/// </summary>
		/// <returns name="TopoDS_Edge">The underlying OCCT Edge</returns>
		virtual const TopoDS_Edge& GetOcctEdge() const;

		/// <summary>
		/// Sets the underlying OCCT shape.
		/// </summary>
		/// <param name="rkOcctShape">A new OCCT shape</param>
		virtual void SetOcctShape(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// Sets the underlying OCCT Edge.
		/// </summary>
		/// <param name="rkOcctEdge">A new OCCT Edge</param>
		void SetOcctEdge(const TopoDS_Edge& rkOcctEdge);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API Handle(Geom_Curve) Curve() const;

		/// <summary>
		/// Returns the underlying OCCT Curve
		/// </summary>
		/// <param name="rFirstParameter">The first parameter</param>
		/// <param name="rSecondParameter">The second parameter</param>
		/// <returns name="Geom_Curve">The underlying OCCT Curve</returns>
		TOPOLOGIC_API Handle(Geom_Curve) Curve(double& rFirstParameter, double& rSecondParameter) const;

		/// <summary>
		/// Returns the center of mass of this Edge.
		/// </summary>
		/// <returns name="Vertex">The center of mass of this Edge</returns>
		virtual std::shared_ptr<Vertex> CenterOfMass() const;

		/// <summary>
		/// Returns the center of mass of this an OCCT Edge.
		/// </summary>
		/// <returns name="Vertex">The center of mass of this an OCCT Edge</returns>
		static TopoDS_Vertex CenterOfMass(const TopoDS_Edge& rkOcctEdge);

		/// <summary>
		/// Returns the type associated to Edge.
		/// </summary>
		/// <returns name="TopologyType">The type associated to Edge</returns>
		virtual TopologyType GetType() const { return TOPOLOGY_EDGE; }

		/// <summary>
		/// Returns the type of the Edge as a String.
		/// </summary>
		/// <returns name="String">The type of the Edge as a String</returns>
		virtual std::string GetTypeAsString() const;

		/// <summary>
		/// Returns the class GUID.
		/// </summary>
		/// <returns name="String">The class GUID</returns>
		virtual std::string GetClassGUID() const {
			return EdgeGUID::Get();
		}

		/// <summary>
		/// Throw an exception associated to an OCCT error.
		/// </summary>
		/// <param name="occtEdgeError">An OCCT error</param>
		TOPOLOGIC_API static void Throw(const BRepBuilderAPI_EdgeError occtEdgeError);

		/// <summary>
		/// Normalize the parameters. (OCCT uses non-normalized parameters, while Topologic uses normalized parameters)
		/// </summary>
		/// <param name="kOcctFirstParameter">The first OCCT parameter</param>
		/// <param name="kOcctLastParameter">The last OCCT parameter</param>
		/// <param name="kNonNormalizedParameter">A non-normalized parameter</param>
		/// <returns name="double">A normalized parameter</returns>
		TOPOLOGIC_API static double NormalizeParameter(const double kOcctFirstParameter, const double kOcctLastParameter, const double kNonNormalizedParameter);

		/// <summary>
		/// Non-normalize the parameters. (OCCT uses non-normalized parameters, while Topologic uses normalized parameters)
		/// </summary>
		/// <param name="kOcctFirstParameter">The first OCCT parameter</param>
		/// <param name="kOcctLastParameter">The last OCCT parameter</param>
		/// <param name="kNormalizedParameter">A normalized parameter</param>
		/// <returns name="double">A non-normalized parameter</returns>
		static TOPOLOGIC_API double NonNormalizeParameter(const double kOcctFirstParameter, const double kOcctLastParameter, const double kNormalizedParameter);

		/// <summary>
		/// Returns the type of the Edge.
		/// </summary>
		/// <returns name="int">The type of the Edge</returns>
		static TOPOLOGIC_API int Type() { return TopologicCore::TOPOLOGY_EDGE; }

		/// <summary>
		/// Checks if the underlying Topology is a container type (Wire, Shell, CellComplex, Cluster).
		/// </summary>
		/// <returns name="bool">True if the underlying Topology is a container type (Wire, Shell, CellComplex, Cluster), otherwise False</returns>
		virtual bool IsContainerType() { return false; }

		/// <summary>
		/// Fixes the input OCCT Edge.
		/// </summary>
		/// <param name="rkOcctInputEdge">An input OCCT Edge</param>
		/// <returns name="TopoDS_Edge">The fixed OCCT Edge</returns>
		static TopoDS_Edge OcctShapeFix(const TopoDS_Edge& rkOcctInputEdge);

	protected:
		/// <summary>
		/// The underlying OCCT edge.
		/// </summary>
		TopoDS_Edge m_occtEdge;
	};
}
