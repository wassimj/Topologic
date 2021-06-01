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

#include <TopoDS_Compound.hxx>
#include <TopoDS_Builder.hxx>

namespace TopologicCore
{
	class CellComplex;
	class Cell;
	class Vertex;
	class Edge;
	class Wire;
	class Face;
	class Shell;

	class ClusterGUID
	{
	public:
		static std::string Get()
		{
			return std::string("7c498db6-f3e7-4722-be58-9720a4a9c2cc");
		}
	};

	class Cluster : public Topology
	{
	public:
		typedef std::shared_ptr<Cluster> Ptr;

	public:
		TOPOLOGIC_API Cluster(const TopoDS_Compound& rkOcctCompound, const std::string& rkGuid = "");

		TOPOLOGIC_API virtual ~Cluster();

		/// <summary>
		/// Creates a Cluster by a set of Topologies.
		/// </summary>
		/// <param name="rkTopologies">A set of Topologies</param>
		/// <returns name="Cluster">The created Cluster</returns>
		TOPOLOGIC_API static std::shared_ptr<Cluster> ByTopologies(const std::list<std::shared_ptr<Topology>>& rkTopologies);

		static TopoDS_Compound ByOcctTopologies(const TopTools_MapOfShape& rkOcctShapes);

		/// <summary>
		/// Adds Topology to a Cluster and returns the resulting Topology.
		/// </summary>
		/// <param name="kpkTopology">A Topology</param>
		/// <returns>The created Topology</returns>
		TOPOLOGIC_API bool AddTopology(Topology const * const kpkTopology);

		/// <summary>
		/// Removes Topology from a Cluster and returns the resulting Topology.
		/// </summary>
		/// <param name="kpkTopology">A Topology</param>
		/// <returns>The created Topology</returns>
		TOPOLOGIC_API bool RemoveTopology(Topology const * const kpkTopology);

		/// <summary>
		/// Returns the underlying OCCT shape.
		/// </summary>
		/// <returns name="TopoDS_Shape">The underlying OCCT shape</returns>
		TOPOLOGIC_API virtual TopoDS_Shape& GetOcctShape();

		/// <summary>
		/// Returns the underlying OCCT shape.
		/// </summary>
		/// <returns name="TopoDS_Shape">The underlying OCCT shape</returns>
		TOPOLOGIC_API virtual const TopoDS_Shape& GetOcctShape() const;

		/// <summary>
		/// Returns the underlying OCCT Cluster.
		/// </summary>
		/// <returns name="TopoDS_Compound">The underlying OCCT Cluster</returns>
		TOPOLOGIC_API virtual TopoDS_Compound& GetOcctCompound();

		/// <summary>
		/// Returns the underlying OCCT Cluster.
		/// </summary>
		/// <returns name="TopoDS_Compound">The underlying OCCT Cluster</returns>
		TOPOLOGIC_API virtual const TopoDS_Compound& GetOcctCompound() const;

		/// <summary>
		/// Sets the underlying OCCT shape.
		/// </summary>
		/// <param name="rkOcctShape">A new OCCT shape</param>
		TOPOLOGIC_API virtual void SetOcctShape(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// Sets the underlying OCCT Compound.
		/// </summary>
		/// <param name="rkOcctCompound">A new OCCT Compound</param>
		void SetOcctCompound(const TopoDS_Compound& rkOcctCompound);

		/// <summary>
		/// Creates a geometry from this Cluster.
		/// </summary>
		/// <param name="rOcctGeometries">The created geometry</param>
		TOPOLOGIC_API virtual void Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const;

		/// <summary>
		/// Returns the Shells constituent to the Cluster.
		/// </summary>
		/// <returns name="rShells">A list of Shells constituent to the Cluster</returns>
		TOPOLOGIC_API void Shells(std::list<std::shared_ptr<Shell>>& rShells) const;

		/// <summary>
		/// Returns the Edges constituent to the Cluster.
		/// </summary>
		/// <returns name="rShells">A list of Edges constituent to the Cluster</returns>
		TOPOLOGIC_API void Edges(std::list<std::shared_ptr<Edge>>& rEdges) const;

		/// <summary>
		/// Returns the Faces constituent to the Cluster.
		/// </summary>
		/// <returns name="rShells">A list of Faces constituent to the Cluster</returns>
		TOPOLOGIC_API void Faces(std::list<std::shared_ptr<Face>>& rFaces) const;

		/// <summary>
		/// Returns the Vertices constituent to the Cluster.
		/// </summary>
		/// <returns name="rShells">A list of Vertices constituent to the Cluster</returns>
		TOPOLOGIC_API void Vertices(std::list<std::shared_ptr<Vertex>>& rVertices) const;

		/// <summary>
		/// Returns the Wires constituent to the Cluster.
		/// </summary>
		/// <returns name="rShells">A list of Wires constituent to the Cluster</returns>
		TOPOLOGIC_API void Wires(std::list<std::shared_ptr<Wire>>& rWires) const;

		/// <summary>
		/// Returns the Cells constituent to the Cluster.
		/// </summary>
		/// <returns name="rShells">A list of Cells constituent to the Cluster</returns>
		TOPOLOGIC_API void Cells(std::list<std::shared_ptr<Cell>>& rCells) const;

		/// <summary>
		/// Returns the CellComplexes constituent to the Cluster.
		/// </summary>
		/// <returns name="rShells">A list of CellComplexes constituent to the Cluster</returns>
		TOPOLOGIC_API void CellComplexes(std::list<std::shared_ptr<CellComplex>>& rCellComplexes) const;

		/// <summary>
		/// Returns the center of mass of this Cluster.
		/// </summary>
		/// <returns name="Vertex">The center of mass of this Cluster</returns>
		TOPOLOGIC_API virtual std::shared_ptr<Vertex> CenterOfMass() const;

		/// <summary>
		/// Returns the center of mass of this Cluster.
		/// </summary>
		/// <returns name="Vertex">The center of mass of this Cluster</returns>
		static TopoDS_Vertex CenterOfMass(const TopoDS_Compound& rkOcctCompound);

		/// <summary>
		/// Returns True if this Cluster is a manifold, otherwise a False.
		/// </summary>
		/// <returns name="bool">True if this Cluster is a manifold, otherwise a False</returns>
		TOPOLOGIC_API virtual bool IsManifold() const;

		/// <summary>
		/// Returns the type associated to Cluster.
		/// </summary>
		/// <returns name="TopologyType">The type associated to Cluster</returns>
		virtual TopologyType GetType() const { return TOPOLOGY_CLUSTER; }

		/// <summary>
		/// Returns the type of the Cluster as a String.
		/// </summary>
		/// <returns name="String">The type of the Cluster as a String</returns>
		virtual std::string GetTypeAsString() const;

		/// <summary>
		/// Returns the class GUID.
		/// </summary>
		/// <returns name="String">The class GUID</returns>
		virtual std::string GetClassGUID() const {
			return ClusterGUID::Get();
		}

		/// <summary>
		/// Returns the type of the Cluster
		/// </summary>
		/// <returns name="int">The type of the Cluster</returns>
		static TOPOLOGIC_API int Type() { return TopologicCore::TOPOLOGY_CLUSTER; }

		/// <summary>
		/// Checks if the underlying Topology is a container type (Wire, Shell, CellComplex, Cluster).
		/// </summary>
		/// <returns name="bool">True if the underlying Topology is a container type (Wire, Shell, CellComplex, Cluster), otherwise False</returns>
		virtual bool IsContainerType() { return true; }

	protected:
		/// <summary>
		/// Returns True if the Topology is inside this Cluster, otherwise False.
		/// </summary>
		/// <param name="kpkTopology">A Topology</param>
		/// <returns name="bool">True if the Topology is inside this Cluster, otherwise False</returns>
		bool IsInside(Topology const * const kpkTopology) const;

		/// <summary>
		/// The underlying OCCT compound.
		/// </summary>
		TopoDS_Compound m_occtCompound;

		/// <summary>
		/// The OCCT builder to create the Cluster.
		/// </summary>
		TopoDS_Builder m_occtBuilder;
	};
}
