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

#include <list>
#include <vector>

#include <TopoDS_Shell.hxx>

namespace TopologicCore
{
	class Vertex;
	class Edge;
	class Wire;
	class Face;
	class Cell;

	class ShellGUID
	{
	public:
		static std::string Get()
		{
			return std::string("51c1e590-cec9-4e84-8f6b-e4f8c34fd3b3");
		}
	};

	/// <summary>
	/// 
	/// </summary>
	class Shell : public Topology
	{
	public:
		typedef std::shared_ptr<Shell> Ptr;

	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShell"></param>
		TOPOLOGIC_API Shell(const TopoDS_Shell& rkOcctShell, const std::string& rkGuid = "");

		TOPOLOGIC_API virtual ~Shell();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rCells"></param>
		/// <returns></returns>
		TOPOLOGIC_API void Cells(const Topology::Ptr& kpHostTopology, std::list<std::shared_ptr<Cell>>& rCells) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rEdges"></param>
		TOPOLOGIC_API void Edges(const Topology::Ptr& kpHostTopology, std::list<std::shared_ptr<Edge>>& rEdges) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rWires"></param>
		TOPOLOGIC_API void Wires(const Topology::Ptr& kpHostTopology, std::list<std::shared_ptr<Wire>>& rWires) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rFaces"></param>
		TOPOLOGIC_API void Faces(const Topology::Ptr& kpHostTopology, std::list<std::shared_ptr<Face>>& rFaces) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rVertices"></param>
		TOPOLOGIC_API void Vertices(const Topology::Ptr& kpHostTopology, std::list<std::shared_ptr<Vertex>>& rVertices) const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API bool IsClosed() const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkFaces"></param>
		/// <param name="kTolerance"></param>
		/// <returns></returns>
		static TOPOLOGIC_API std::shared_ptr<Shell> ByFaces(const std::list<std::shared_ptr<Face>>& rkFaces, const double kTolerance = 0.001, const bool kCopyAttributes = false);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API virtual bool IsManifold(const Topology::Ptr& kpHostTopology) const;

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
		/// <returns></returns>
		TOPOLOGIC_API virtual TopoDS_Shell& GetOcctShell();

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API virtual const TopoDS_Shell& GetOcctShell() const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		TOPOLOGIC_API virtual void SetOcctShape(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShell"></param>
		TOPOLOGIC_API void SetOcctShell(const TopoDS_Shell& rkOcctShell);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rOcctGeometries"></param>
		TOPOLOGIC_API virtual void Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const;

		/// <summary>
		/// 
		/// </summary>
		TOPOLOGIC_API virtual std::shared_ptr<Vertex> CenterOfMass() const;

		TOPOLOGIC_API static TopoDS_Vertex CenterOfMass(const TopoDS_Shell& rkOcctShell);


		TOPOLOGIC_API virtual TopologyType GetType() const { return TOPOLOGY_SHELL; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API virtual std::string GetTypeAsString() const;

		TOPOLOGIC_API virtual std::string GetClassGUID() const {
			return ShellGUID::Get();
		}

		static TOPOLOGIC_API int Type() { return TopologicCore::TOPOLOGY_SHELL; }

		TOPOLOGIC_API virtual bool IsContainerType() { return true; }

	protected:
		/// <summary>
		/// The underlying OCCT shell.
		/// </summary>
		TopoDS_Shell m_occtShell;
	};
}
