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

#include <TopoDS_CompSolid.hxx>

namespace TopologicCore
{
	class Cell;
	class Vertex;
	class Edge;
	class Wire;
	class Face;
	class Shell;

	class CellComplexGUID
	{
	public:
		static std::string Get()
		{
			return std::string("4ec9904b-dc01-42df-9647-2e58c2e08e78");
		}
	};

	/// <summary>
	/// A CellComplex is a contiguous collection of Cells where adjacent Cells are connected by shared Faces. It is non-manifold.
	/// </summary>
	class CellComplex : public Topology
	{
	public:
		typedef std::shared_ptr<CellComplex> Ptr;

	public:
		CellComplex(const TopoDS_CompSolid& rkOcctCompSolid, const std::string& rkGuid = "");

		virtual ~CellComplex();

		/// <summary>
		/// Returns the Cells constituent to the CellComplex.
		/// </summary>
		/// <param name="rCells">A list of Cells constituent to the CellComplex</param>
		TOPOLOGIC_API void Cells(std::list<std::shared_ptr<Cell>>& rCells) const;

		/// <summary>
		/// Returns the Faces constituent to the CellComplex.
		/// </summary>
		/// <param name="rCells">A list of Faces constituent to the CellComplex</param>
		TOPOLOGIC_API void Faces(std::list<std::shared_ptr<Face>>& rFaces) const;

		/// <summary>
		/// Returns the Shells constituent to the CellComplex.
		/// </summary>
		/// <returns name="rShells">A list of Shells constituent to the CellComplex</returns>
		TOPOLOGIC_API void Shells(std::list<std::shared_ptr<Shell>>& rShells) const;

		/// <summary>
		/// Returns the Edges constituent to the CellComplex.
		/// </summary>
		/// <param name="rEdges">A list of Edges constituent to the CellComplex</param>
		TOPOLOGIC_API void Edges(std::list<std::shared_ptr<Edge>>& rEdges) const;

		/// <summary>
		/// Returns the Vertices constituent to the CellComplex.
		/// </summary>
		/// <param name="rVertices">A list of Vertices constituent to the CellComplex</param>
		TOPOLOGIC_API void Vertices(std::list<std::shared_ptr<Vertex>>& rVertices) const;

		/// <summary>
		/// Returns the Wires constituent to the CellComplex.
		/// </summary>
		/// <param name="rWires">A list of Wires constituent to the CellComplex</param>
		TOPOLOGIC_API void Wires(std::list<std::shared_ptr<Wire>>& rWires) const;

		/// <summary>
		/// Creates a CellComplex by a set of Cells.
		/// </summary>
		/// <param name="rkCells">A set of Cells</param>
		/// <returns name="CellComplex">The created CellComplex</returns>
		TOPOLOGIC_API static std::shared_ptr<CellComplex> ByCells(const std::list<std::shared_ptr<Cell>>& rkCells);

		/// <summary>
		/// Creates an OCCT CompSolid by a set of OCCT Solids.
		/// </summary>
		/// <param name="rkOcctSolids">A set of OCCT Solids</param>
		/// <returns name="CellComplex">The created OCCT CompSolid</returns>
		static TopoDS_CompSolid ByOcctSolids(const TopTools_ListOfShape& rkOcctSolids);

		/// <summary>
		/// Creates a CellComplex from the space enclosed by a set of Faces. Parts of the Faces which do not enclose any space will be discarded.
		/// </summary>
		/// <param name="rkFaces">A set of Faces</param>
		/// <returns name="CellComplex">The created CellComplex</returns> 
		TOPOLOGIC_API static std::shared_ptr<CellComplex> ByFaces(const std::list<std::shared_ptr<Face>>& rkFaces, const double kTolerance);

		/// <summary>
		/// Returns the external boundary (Cell) of the CellComplex.
		/// </summary>
		/// <returns name="Cell">The external boundary (Cell) of the CellComplex</returns>
		TOPOLOGIC_API std::shared_ptr<Cell> ExternalBoundary() const;

		/// <summary>
		/// Returns the internal boundaries (Faces) of the CellComplex.
		/// </summary>
		/// <param name="rkInternalFaces">The internal boundaries (Faces) of the CellComplex</param>
		TOPOLOGIC_API void InternalBoundaries(std::list<std::shared_ptr<Face>>& rInternalFaces) const;

		/// <summary>
		/// Returns True if this CellComplex is a manifold, otherwise a False.
		/// </summary>
		/// <returns name="bool">True if this CellComplex is a manifold, otherwise a False</returns>
		virtual bool IsManifold() const;

		/// <summary>
		/// Returns the non-manifold Faces of this CellComplex.
		/// </summary>
		/// <param name="rNonManifoldFaces">The non-manifold Faces of this CellComplex</param>
		TOPOLOGIC_API void NonManifoldFaces(std::list<std::shared_ptr<Face>>& rNonManifoldFaces) const;

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
		/// Returns the underlying OCCT CompSolid.
		/// </summary>
		/// <returns name="TopoDS_CompSolid">The underlying OCCT CompSolid</returns>
		virtual TopoDS_CompSolid& GetOcctCompSolid();

		/// <summary>
		/// Returns the underlying OCCT CompSolid.
		/// </summary>
		/// <returns name="TopoDS_CompSolid">The underlying OCCT CompSolid</returns>
		virtual const TopoDS_CompSolid& GetOcctCompSolid() const;

		/// <summary>
		/// Sets the underlying OCCT shape.
		/// </summary>
		/// <param name="rkOcctShape">A new OCCT shape</param>
		virtual void SetOcctShape(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// Sets the underlying OCCT CompSolid.
		/// </summary>
		/// <param name="rkOcctCompSolid">A new OCCT CompSolid</param>
		void SetOcctCompSolid(const TopoDS_CompSolid& rkOcctCompSolid);

		/// <summary>
		/// Returns the center of mass of this CellComplex.
		/// </summary>
		/// <returns name="Vertex">The center of mass of this CellComplex</returns>
		virtual std::shared_ptr<Vertex> CenterOfMass() const;

		/// <summary>
		/// Returns the center of mass of an OCCT CompSolid.
		/// </summary>
		/// <param name="rkOcctCompSolid">An OCCT CompSolid</param>
		/// <returns name="TopoDS_Vertex">The OCCT center of mass of this CompSolid</returns>
		static TopoDS_Vertex CenterOfMass(const TopoDS_CompSolid& rkOcctCompSolid);

		/// <summary>
		/// Creates a geometry from this CellComplex.
		/// </summary>
		/// <param name="rOcctGeometries">The created geometry</param>
		virtual void Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const;

		/// <summary>
		/// Returns the type associated to CellComplex.
		/// </summary>
		/// <returns name="TopologyType">The type associated to CellComplex</returns>
		virtual TopologyType GetType() const { return TOPOLOGY_CELLCOMPLEX; }

		/// <summary>
		/// Returns the type of the CellComplex as a String.
		/// </summary>
		/// <returns name="String">The type of the CellComplex as a String</returns>
		virtual std::string GetTypeAsString() const;

		/// <summary>
		/// Returns the class GUID.
		/// </summary>
		/// <returns name="String">The class GUID</returns>
		virtual std::string GetClassGUID() const {
			return CellComplexGUID::Get();
		}

		/// <summary>
		/// Returns the type of the CellComplex.
		/// </summary>
		/// <returns name="int">The type of the CellComplex</returns>
		TOPOLOGIC_API static int Type() { return TopologicCore::TOPOLOGY_CELLCOMPLEX; }

		/// <summary>
		/// Checks if the underlying Topology is a container type (Wire, Shell, CellComplex, Cluster).
		/// </summary>
		/// <returns name="bool">True if the underlying Topology is a container type (Wire, Shell, CellComplex, Cluster), otherwise False</returns>
		virtual bool IsContainerType() { return true; }

		/// <summary>
		/// Fixes the input OCCT CompSolid.
		/// </summary>
		/// <param name="rkOcctInputSolid">An input OCCT CompSolid</param>
		/// <returns name="TopoDS_Solid">The fixed OCCT CompSolid</returns>
		static TopoDS_CompSolid OcctShapeFix(const TopoDS_CompSolid& rkOcctInputCompSolid);

	protected:
		/// <summary>
		/// The underlying OCCT cell complex.
		/// </summary>
		TopoDS_CompSolid m_occtCompSolid;
	};
}
