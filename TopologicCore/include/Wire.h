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

#include <TopoDS_Wire.hxx>

class BRepBuilderAPI_MakeWire;

namespace TopologicCore
{
	class Vertex;
	class Edge;
	class Face;

	class WireGUID
	{
	public:
		static std::string Get()
		{
			return std::string("b99ccd99-6756-401d-ab6c-11162de541a3");
		}
	};

	class Wire : public Topology
	{
	public:
		typedef std::shared_ptr<Wire> Ptr;

	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctWire"></param>
		TOPOLOGIC_API Wire(const TopoDS_Wire& rkOcctWire, const std::string& rkGuid = "");

		TOPOLOGIC_API virtual ~Wire();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rEdges"></param>
		void TOPOLOGIC_API Edges(std::list<std::shared_ptr<Edge>>& rEdges) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rFaces"></param>
		/// <returns></returns>
		void TOPOLOGIC_API Faces(std::list<std::shared_ptr<Face>>& rFaces) const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		bool TOPOLOGIC_API IsClosed() const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rVertices"></param>
		virtual void TOPOLOGIC_API Vertices(std::list<std::shared_ptr<Vertex>>& rVertices) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkEdges"></param>
		/// <returns></returns>
		static TOPOLOGIC_API std::shared_ptr<Wire> ByEdges(const std::list<std::shared_ptr<Edge>>& rkEdges);

		static TopoDS_Wire ByOcctEdges(const TopTools_ListOfShape& rkOcctEdges);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API virtual bool IsManifold() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API int NumberOfBranches() const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rOcctGeometries"></param>
		TOPOLOGIC_API virtual void Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API virtual TopoDS_Shape& GetOcctShape();

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API virtual const TopoDS_Shape& GetOcctShape() const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		TOPOLOGIC_API virtual void SetOcctShape(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctWire"></param>
		TOPOLOGIC_API void SetOcctWire(const TopoDS_Wire& rkOcctWire);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API virtual TopoDS_Wire& GetOcctWire();

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API virtual const TopoDS_Wire& GetOcctWire() const;

		TOPOLOGIC_API virtual std::shared_ptr<Vertex> CenterOfMass() const;

		static TopoDS_Vertex CenterOfMass(const TopoDS_Wire& rkOcctWire);


		virtual TopologyType GetType() const { return TOPOLOGY_WIRE; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual std::string GetTypeAsString() const;

		virtual std::string GetClassGUID() const {
			return WireGUID::Get();
		}

		static TOPOLOGIC_API int Type() { return TopologicCore::TOPOLOGY_WIRE; }

		virtual bool IsContainerType() { return true; }

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctMakeWire"></param>
		static TOPOLOGIC_API void Throw(const BRepBuilderAPI_MakeWire& rkOcctMakeWire);

	protected:
		/// <summary>
		/// The underlying OCCT wire.
		/// </summary>
		TopoDS_Wire m_occtWire;
	};
}
