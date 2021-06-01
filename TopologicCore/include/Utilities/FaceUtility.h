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

#include <Cell.h>
#include <Face.h>
#include <Shell.h>
#include <Vertex.h>
#include <Wire.h>

#include <memory>

namespace TopologicUtilities
{
	class FaceUtility
	{
	public:
		typedef std::shared_ptr<FaceUtility> Ptr;

	public:

		/// <summary>
		/// Topologic UV values are normalized (from 0 and 1), but OCCT's values are not normalized.
		/// </summary>
		/// <param name="kpFace"></param>
		/// <param name="kU"></param>
		/// <param name="kV"></param>
		/// <returns></returns>
		static TOPOLOGIC_API gp_Dir NormalAtParameters(const TopologicCore::Face::Ptr& kpFace, const double kU, const double kV);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpFace"></param>
		/// <returns></returns>
		static TOPOLOGIC_API double Area(const TopologicCore::Face::Ptr& kpFace);

		static double Area(const TopoDS_Face& rkOcctFace);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="coreVertices"></param>
		/// <returns></returns>
		static TOPOLOGIC_API TopologicCore::Face::Ptr ByVertices(const std::list<std::list<TopologicCore::Vertex::Ptr>>& rkVertices);

		/// <summary>
		/// Note: Topologic UV values are normalized (from 0 and 1), but OCCT's values are not normalized.
		/// </summary>
		/// <param name="kpFace"></param>
		/// <param name="kpVertex"></param>
		/// <param name="rU"></param>
		/// <param name="rV"></param>
		/// <returns></returns>
		static TOPOLOGIC_API void ParametersAtVertex(const TopologicCore::Face::Ptr& kpFace, const TopologicCore::Vertex::Ptr& kpVertex, double& rU, double &rV);

		/// <summary>
		/// Note: Topologic UV values are normalized (from 0 and 1), but OCCT's values are not normalized.
		/// </summary>
		/// <param name="kpFace"></param>
		/// <param name="kU"></param>
		/// <param name="kV"></param>
		/// <returns></returns>
		static TOPOLOGIC_API TopologicCore::Vertex::Ptr VertexAtParameters(const TopologicCore::Face::Ptr& kpFace, const double kU, const double kV);

		static void UVSamplePoints(
			const TopologicCore::Face::Ptr& kpFace,
			const std::list<double>& rkUValues,
			const std::list<double>& rkVValues,
			std::list<std::list<gp_Pnt>>& rSamplePoints,
			std::list<double>& rOcctUValues,
			std::list<double>& rOcctVValues,
			int& rNumUPoints,
			int& rNumVPoints,
			int& rNumUPanels,
			int& rNumVPanels,
			bool& rIsUClosed,
			bool& rIsVClosed);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpFace"></param>
		/// <param name="kpWire"></param>
		/// <param name="kReverseWire"></param>
		/// <returns></returns>
		static TOPOLOGIC_API TopologicCore::Face::Ptr TrimByWire(const TopologicCore::Face::Ptr& kpFace, const TopologicCore::Wire::Ptr& kpWire, const bool kReverseWire);

		static TOPOLOGIC_API void Triangulate(const TopologicCore::Face::Ptr& kpFace, const double kDeflection, std::list<TopologicCore::Face::Ptr>& rTriangles);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpFace"></param>
		/// <param name="kNonNormalizedU"></param>
		/// <param name="kNonNormalizedV"></param>
		/// <param name="rNormalizedU"></param>
		/// <param name="rNormalizedV"></param>
		static void NormalizeUV(const TopologicCore::Face::Ptr& kpFace, const double kNonNormalizedU, const double kNonNormalizedV, double& rNormalizedU, double& rNormalizedV);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpFace"></param>
		/// <param name="kNormalizedU"></param>
		/// <param name="kNormalizedV"></param>
		/// <param name="rNonNormalizedU"></param>
		/// <param name="rNonNormalizedV"></param>
		static void NonNormalizeUV(const TopologicCore::Face::Ptr& kpFace, const double kNormalizedU, const double kNormalizedV, double& rNonNormalizedU, double& rNonNormalizedV);

		static TOPOLOGIC_API void AdjacentShells(
			const TopologicCore::Face::Ptr & kpFace,
			const TopologicCore::Topology::Ptr & kpParentTopology,
			std::list<TopologicCore::Shell::Ptr>& rCoreAdjacentShells);

		static TOPOLOGIC_API void AdjacentCells(
			const TopologicCore::Face::Ptr & kpFace,
			const TopologicCore::Topology::Ptr & kpParentTopology,
			std::list<TopologicCore::Cell::Ptr>& rCoreAdjacentCells);

        static void AdjacentCells(
            TopologicCore::Face const * const pkpFace,
            const TopologicCore::Topology::Ptr & kpParentTopology,
            std::list<TopologicCore::Cell::Ptr>& rCoreAdjacentCells);

        static void AdjacentFaces(
            TopologicCore::Face const * const pkpFace,
            const TopologicCore::Topology::Ptr & kpParentTopology,
            std::list<TopologicCore::Face::Ptr>& rCoreAdjacentFaces);


		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpFace"></param>
		/// <param name="kpVertex"></param>
		/// <param name="kTolerance"></param>
		/// <returns></returns>
		static TOPOLOGIC_API bool IsInside(const TopologicCore::Face::Ptr kpFace, const std::shared_ptr<TopologicCore::Vertex>& kpVertex, const double kTolerance);

		static std::shared_ptr<TopologicCore::Vertex> ProjectToSurface(const TopologicCore::Face::Ptr kpFace, const std::shared_ptr<TopologicCore::Vertex>& kpVertex);

		static TOPOLOGIC_API std::shared_ptr<TopologicCore::Vertex> InternalVertex(
			const TopologicCore::Face::Ptr kpFace, const double kTolerance = 0.0001);
	};
}