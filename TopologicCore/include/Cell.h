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

#include <TopoDS_Solid.hxx>

namespace TopologicCore
{
	class Vertex;
	class Edge;
	class Wire;
	class Face;
	class Shell;
	class CellComplex;

	class CellGUID
	{
	public:
		static std::string Get()
		{
			return std::string("8bda6c76-fa5c-4288-9830-80d32d283251");
		}
	};

	/// <summary>
	/// A Cell is a three-dimensional region defined by a collection of closed Shells. It may be manifold or non-manifold.
	/// </summary>
	class Cell : public Topology
	{
	public:
		typedef std::shared_ptr<Cell> Ptr;

	public:
		TOPOLOGIC_API Cell(const TopoDS_Solid& rkOcctSolid, const std::string& rkGuid = "");
		virtual ~Cell();

		/// <summary>
		/// Returns the Cells adjacent to the Cell.
		/// </summary>
		/// <param name="rAdjacentCells">>A list of Cells adjacent to the Cell</param>
		TOPOLOGIC_API void AdjacentCells(std::list<std::shared_ptr<Cell>>& rAdjacentCells) const;

		/// <summary>
		/// Returns the CellComplexes which contain the Cell.
		/// </summary>
		/// <param name="rCellComplexes">A list of CellComplexes containing the Cell</param>
		TOPOLOGIC_API void CellComplexes(std::list<std::shared_ptr<CellComplex>>& rCellComplexes) const;

		/// <summary>
		/// Returns the Shells constituent to the Cell.
		/// </summary>
		/// <param name="rShells">>A list of Shells constituent to the Cell</returns>
		TOPOLOGIC_API void Shells(std::list<std::shared_ptr<Shell>>& rShells) const;

		/// <summary>
		/// Returns the Edges constituent to the Cell.
		/// </summary>
		/// <param name="rEdges">A list of Edges constituent to the Cell</param>
		TOPOLOGIC_API void Edges(std::list<std::shared_ptr<Edge>>& rEdges) const;

		/// <summary>
		/// Returns the Faces constituent to the Cell.
		/// </summary>
		/// <param name="rFaces">A list of Faces constituent to the Cell</returns>
		TOPOLOGIC_API void Faces(std::list<std::shared_ptr<Face>>& rFaces) const;

		/// <summary>
		/// Returns the Vertices constituent to the Cell.
		/// </summary>
		/// <param name="rVertices">A list of Vertices constituent to the Cell</param>
		TOPOLOGIC_API void Vertices(std::list<std::shared_ptr<Vertex>>& rVertices) const;

		/// <summary>
		/// Returns the Wires constituent to the Cell.
		/// </summary>
		/// <param name="rWires">A list of Wires constituent to the Cell</param>
		TOPOLOGIC_API void Wires(std::list<std::shared_ptr<Wire>>& rWires) const;

		/// <summary>
		/// Returns the center of mass of this Cell.
		/// </summary>
		/// <returns name="Vertex">The center of mass of this Cell</returns>
		virtual std::shared_ptr<Vertex> CenterOfMass() const;

		/// <summary>
		/// Returns the center of mass of an OCCT Solid.
		/// </summary>
		/// <param name="rkOcctSolid">An OCCT Solid</param>
		/// <returns name="TopoDS_Vertex">The OCCT center of mass of this Solid</returns>
		static TopoDS_Vertex CenterOfMass(const TopoDS_Solid & rkOcctSolid);

		/// <summary>
		/// Creates a Cell by a set of Faces.
		/// </summary>
		/// <param name="rkFaces">A set of Faces</param>
		/// <param name="kTolerance">A positive tolerance value</param>
		/// <returns name="Cell">The created Cell</returns>
		TOPOLOGIC_API static std::shared_ptr<Cell> ByFaces(const std::list<std::shared_ptr<Face>>& rkFaces, const double kTolerance = 0.001);

		/// <summary>
		/// Creates a Cell from a Shell. The Shell must be closed, otherwise an exception is thrown.
		/// </summary>
		/// <param name="rkShell">A Shell</param>
		/// <returns name="Cell">The created Cell</returns>
		TOPOLOGIC_API static std::shared_ptr<Cell> ByShell(const std::shared_ptr<Shell>& rkShell);

		/// <summary>
		/// Returns the shared Edges between two Cells. 
		/// </summary>
		/// <param name="kpAnotherCell">Another Cell</param>
		/// <param name="rEdges">A list of shared Edges between two Cells</param>
		TOPOLOGIC_API void SharedEdges(const std::shared_ptr<Cell>& kpAnotherCell, std::list<std::shared_ptr<Edge>>& rEdges) const;

		/// <summary>
		/// Returns the shared Faces between two Cells. 
		/// </summary>>
		/// <param name="kpAnotherCell">Another Cell</param>
		/// <param name="rFaces">A list of shared Faces between two Cells</param>
		TOPOLOGIC_API void SharedFaces(const std::shared_ptr<Cell>& kpAnotherCell, std::list<std::shared_ptr<Face>>& rFaces) const;

		/// <summary>
		/// Returns the shared Vertices between two Cells. 
		/// </summary>
		/// <param name="kpAnotherCell">Another Cell</param>
		/// <param name="rVertices">A list of shared Vertices between two Cells</param>
		TOPOLOGIC_API void SharedVertices(const std::shared_ptr<Cell>& kpkAnotherCell, std::list<std::shared_ptr<Vertex>>& rVertices) const;

		/// <summary>
		/// Returns the external boundary (Shell) of the Cell.
		/// </summary>
		/// <returns name="Shell">The external boundary (Shell) of the Cell</returns>
		TOPOLOGIC_API std::shared_ptr<Shell> ExternalBoundary() const;

		/// <summary>
		/// Returns the internal boundaries (Shells) of the Cell.
		/// </summary>
		/// <param name="rShells">A list of the internal boundaries (Shells) of the Cell</param>
		TOPOLOGIC_API void InternalBoundaries(std::list<std::shared_ptr<Shell>>& rShells) const;

		/// <summary>
		/// Returns True if this Cell is a manifold, otherwise a False.
		/// </summary>
		/// <returns name="bool">True if this Cell is a manifold, otherwise a False.</returns>
		virtual bool IsManifold() const;

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
		/// Returns the underlying OCCT Solid.
		/// </summary>
		/// <returns name="TopoDS_Solid">The underlying OCCT Solid</returns>
		virtual TopoDS_Solid& GetOcctSolid();

		/// <summary>
		/// Returns the underlying OCCT solid.
		/// </summary>
		/// <returns name="TopoDS_Solid">The underlying OCCT Solid</returns>
		virtual const TopoDS_Solid& GetOcctSolid() const;

		/// <summary>
		/// Sets the underlying OCCT shape.
		/// </summary>
		/// <param name="rkOcctShape">A new OCCT shape</param>
		virtual void SetOcctShape(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// Sets the underlying OCCT Solid.
		/// </summary>
		/// <param name="rkOcctSolid">A new OCCT Solid</param>
		void SetOcctSolid(const TopoDS_Solid& rkOcctSolid);

		/// <summary>
		/// Creates a geometry from this Cell.
		/// </summary>
		/// <param name="rOcctGeometries">The created geometry</param>
		virtual void Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const;

		/// <summary>
		/// Returns the type associated to Cell.
		/// </summary>
		/// <returns name="TopologyType">The type associated to Cell</returns>
		virtual TopologyType GetType() const { return TOPOLOGY_CELL; }

		/// <summary>
		/// Returns the type of the Cell as a String.
		/// </summary>
		/// <returns name="String">The type of the Cell as a String</returns>
		virtual std::string GetTypeAsString() const;

		/// <summary>
		/// Returns the class GUID.
		/// </summary>
		/// <returns name="String">The class GUID</returns>
		virtual std::string GetClassGUID() const {
			return CellGUID::Get();
		}

		/// <summary>
		/// Returns the type of the Cell
		/// </summary>
		/// <returns name="int">The type of the Cell</returns>
		TOPOLOGIC_API static int Type() { return TopologicCore::TOPOLOGY_CELL; }
		
		/// <summary>
		/// Checks if the underlying Topology is a container type (Wire, Shell, CellComplex, Cluster).
		/// </summary>
		/// <returns name="bool">True if the underlying Topology is a container type (Wire, Shell, CellComplex, Cluster), otherwise False</returns>
		virtual bool IsContainerType() { return false; }

		/// <summary>
		/// Fixes the input OCCT Solid.
		/// </summary>
		/// <param name="rkOcctInputSolid">An input OCCT Solid</param>
		/// <returns name="TopoDS_Solid">The fixed OCCT Solid</returns>
		static TopoDS_Solid OcctShapeFix(const TopoDS_Solid& rkOcctInputSolid);

	protected:
		/// <summary>
		/// The underlying OCCT cell.
		/// </summary>
		TopoDS_Solid m_occtSolid;
	};
}
