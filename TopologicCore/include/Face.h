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

#include <Standard_Handle.hxx>
#include <TopoDS_Face.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>

class BRepBuilderAPI_MakeFace;
class Geom_Surface;

namespace TopologicCore
{
	class Vertex;
	class Edge;
	class Wire;
	class Shell;
	class Cell;

	class FaceGUID
	{
	public:
		static std::string Get()
		{
			return std::string("3b0a6afe-af86-4d96-a30d-d235e9c98475");
		}
	};

	/// <summary>
	/// 
	/// </summary>
	class Face : public Topology
	{
	public:
		typedef std::shared_ptr<Face> Ptr;

	public:
		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctFace"></param>
		TOPOLOGIC_API Face(const TopoDS_Face& rkOcctFace, const std::string& rkGuid = "");

		virtual ~Face();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rFaces"></param>
		TOPOLOGIC_API void AdjacentFaces(std::list<std::shared_ptr<Face>>& rFaces) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rCells"></param>
		/// <returns></returns>
		TOPOLOGIC_API void Cells(std::list<std::shared_ptr<Cell>>& rCells) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rEdges"></param>
		TOPOLOGIC_API void Edges(std::list<std::shared_ptr<Edge>>& rEdges) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rShells"></param>
		/// <returns></returns>
		TOPOLOGIC_API void Shells(std::list<std::shared_ptr<Shell>>& rShells) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rVertices"></param>
		TOPOLOGIC_API void Vertices(std::list<std::shared_ptr<Vertex>>& rVertices) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rWires"></param>
		TOPOLOGIC_API void Wires(std::list<std::shared_ptr<Wire>>& rWires) const;

		/// <summary>
		/// 
		/// </summary>
		virtual std::shared_ptr<Vertex> CenterOfMass() const;

		static TopoDS_Vertex CenterOfMass(const TopoDS_Face& rkOcctFace);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpExternalBoundary"></param>
		/// <returns></returns>
		TOPOLOGIC_API static std::shared_ptr<Face> ByExternalBoundary(const std::shared_ptr<Wire>& kpExternalBoundary);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="pkExternalBoundary"></param>
		/// <param name="rkInternalBoundaries"></param>
		/// <returns></returns>
		TOPOLOGIC_API static std::shared_ptr<Face> ByExternalInternalBoundaries(
			const std::shared_ptr<Wire>& pkExternalBoundary, 
			const std::list<std::shared_ptr<Wire>>& rkInternalBoundaries);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkEdges"></param>
		/// <returns></returns>
		static TOPOLOGIC_API std::shared_ptr<Face> ByEdges(const std::list<std::shared_ptr<Edge>>& rkEdges);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="pOcctSurface"></param>
		/// <returns></returns>
		static TOPOLOGIC_API std::shared_ptr<Face> BySurface(Handle(Geom_Surface) pOcctSurface);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctPoles"></param>
		/// <param name="rkOcctWeights"></param>
		/// <param name="rkOcctUKnots"></param>
		/// <param name="rkOcctVKnots"></param>
		/// <param name="rkOcctMultiplicities"></param>
		/// <param name="rkOcctVMultiplicities"></param>
		/// <param name="kUDegree"></param>
		/// <param name="kVDegree"></param>
		/// <param name="kIsUPeriodic"></param>
		/// <param name="kIsVPeriodic"></param>
		/// <param name="kIsRational"></param>
		/// <param name="rkOuterWire"></param>
		/// <param name="rkInnerWires"></param>
		/// <returns></returns>
		static TOPOLOGIC_API std::shared_ptr<Face> BySurface(
			const TColgp_Array2OfPnt& rkOcctPoles,
			const TColStd_Array2OfReal& rkOcctWeights,
			const TColStd_Array1OfReal& rkOcctUKnots,
			const TColStd_Array1OfReal& rkOcctVKnots,
			const TColStd_Array1OfInteger& rkOcctUMultiplicities,
			const TColStd_Array1OfInteger& rkOcctVMultiplicities,
			const int kUDegree,
			const int kVDegree,
			const bool kIsUPeriodic,
			const bool kIsVPeriodic,
			const bool kIsRational,
			const std::shared_ptr<Wire>& kpOuterWire,
			const std::list<std::shared_ptr<Wire>>& rkInnerWires);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpAnotherFace"></param>
		/// <param name="rEdges"></param>
		void TOPOLOGIC_API SharedEdges(const std::shared_ptr<Face>& kpAnotherFace, std::list<std::shared_ptr<Edge>>& rEdges) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpAnotherFace"></param>
		/// <param name="rVertices"></param>
		void TOPOLOGIC_API SharedVertices(const std::shared_ptr<Face>& kpAnotherFace, std::list<std::shared_ptr<Vertex>>& rVertices) const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API std::shared_ptr<Wire> ExternalBoundary() const;

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        static TopoDS_Wire ExternalBoundary(const TopoDS_Face& rkOcctFace);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rInternalBoundaries"></param>
		/// <returns></returns>
		TOPOLOGIC_API void InternalBoundaries(std::list<std::shared_ptr<Wire>>& rInternalBoundaries) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpWire"></param>
		/// <returns></returns>
		TOPOLOGIC_API void AddInternalBoundary(const std::shared_ptr<Wire>& kpWire);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkWires"></param>
		/// <returns></returns>
		TOPOLOGIC_API void AddInternalBoundaries(const std::list<std::shared_ptr<Wire>>& rkWires);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual bool IsManifold() const;

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        bool IsManifoldToTopology(const Topology::Ptr& kpTopology = nullptr) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rOcctGeometries"></param>
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
		/// <returns></returns>
		virtual TopoDS_Face& GetOcctFace();

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual const TopoDS_Face& GetOcctFace() const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		virtual void SetOcctShape(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctFace"></param>
		void SetOcctFace(const TopoDS_Face& rkOcctFace);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API Handle(Geom_Surface) Surface() const;

		virtual TopologyType GetType() const { return TOPOLOGY_FACE; }

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual std::string GetTypeAsString() const;

		virtual std::string GetClassGUID() const {
			return FaceGUID::Get();
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctMakeFace"></param>
		static TOPOLOGIC_API void Throw(const BRepBuilderAPI_MakeFace& rkOcctMakeFace);

		static TOPOLOGIC_API int Type() { return TopologicCore::TOPOLOGY_FACE; }

		virtual bool IsContainerType() { return false; }

		void Triangulate(const double kLinearDeflection, const double kAngularDeflection, std::list<TopologicCore::Face::Ptr>& rTriangles) const;


		/// <summary>
		/// Fixes the input OCCT Face.
		/// </summary>
		/// <param name="rkOcctInputFace">An input OCCT Face</param>
		/// <returns name="TopoDS_Face">The fixed OCCT Face</returns>
		static TopoDS_Face OcctShapeFix(const TopoDS_Face& rkOcctInputFace);

	protected:
		/// <summary>
		/// The underlying OCCT face.
		/// </summary>
		TopoDS_Face m_occtFace;
	};
}
