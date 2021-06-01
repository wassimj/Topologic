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

#include <Standard_Handle.hxx>
#include <Geom_Geometry.hxx>
#include <TopoDS_Vertex.hxx>

class Geom_Point;
class gp_Pnt;

namespace TopologicCore
{
	class Edge;
	
	class VertexGUID
	{
	public:
		static std::string Get()
		{
			return std::string("c4a9b420-edaf-4f8f-96eb-c87fbcc92f2b");
		}
	};

	/// <summary>
	/// The representation of a topological vertex. This class wraps OCCT's TopoDS_Vertex.
	/// </summary>
	class Vertex : public Topology
	{
	public:
		typedef std::shared_ptr<Vertex> Ptr;

	public:
		/// <summary>
		/// Creates a vertex by an OCCT vertex.
		/// </summary>
		/// <param name="rkOcctVertex">An OCCT vertex</param>
		TOPOLOGIC_API Vertex(const TopoDS_Vertex& rkOcctVertex, const std::string& rkGuid = "");

		virtual ~Vertex();

		/// <summary>
		/// Creates a vertex by a point.
		/// </summary>
		/// <param name="pOcctPoint">An OCCT point</param>
		/// <returns>The created Topologic vertex.</returns>
		static TOPOLOGIC_API std::shared_ptr<Vertex> ByPoint(Handle(Geom_Point) pOcctPoint);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kX"></param>
		/// <param name="kY"></param>
		/// <param name="kZ"></param>
		/// <returns></returns>
		static TOPOLOGIC_API std::shared_ptr<Vertex> ByCoordinates(const double kX, const double kY, const double kZ);

		/// <summary>
		/// Returns the list of edges of which this vertex is a constituent member.
		/// </summary>
		/// <param name="rEdges">The edges containing this vertex as a constituent member</param>
		TOPOLOGIC_API void Edges(std::list<std::shared_ptr<Edge>>& rEdges);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API double X() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API double Y() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API double Z() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API std::tuple<double, double, double> Coordinates() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual bool IsManifold() const;

		/// <summary>
		/// Return the corresponding point of this vertex. The output list only contains one vertex.
		/// </summary>
		/// <exception cref="std::runtime_error">Null OCCT vertex</exception>
		/// <param name="rOcctGeometries">The output parameter, containing only one point corresponding to this vertex.</param>
		virtual void Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual TopoDS_Shape& GetOcctShape();

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual const TopoDS_Shape& GetOcctShape() const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		virtual void SetOcctShape(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual TopoDS_Vertex& GetOcctVertex();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctVertex"></param>
		void SetOcctVertex(const TopoDS_Vertex& rkOcctVertex);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual const TopoDS_Vertex& GetOcctVertex() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API Handle(Geom_Point) Point() const;

		virtual std::shared_ptr<Vertex> CenterOfMass() const;

		static TopoDS_Vertex CenterOfMass(const TopoDS_Vertex& rkOcctVertex);

		virtual TopologyType GetType() const { return TOPOLOGY_VERTEX; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual std::string GetTypeAsString() const;

		virtual std::string GetClassGUID() const {
			return VertexGUID::Get();
		}

		static TOPOLOGIC_API int Type() { return TopologicCore::TOPOLOGY_VERTEX; }

		virtual bool IsContainerType() { return false; }

	protected:
		/// <summary>
		/// The underlying OCCT vertex.
		/// </summary>
		TopoDS_Vertex m_occtVertex;
	};
}