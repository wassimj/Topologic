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

#include <Edge.h>
#include <Vertex.h>
#include <Wire.h>
#include <Face.h>

#include <memory>

namespace TopologicUtilities
{
	class EdgeUtility
	{
	public:
		typedef std::shared_ptr<EdgeUtility> Ptr;

	public:

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkVertices"></param>
		/// <returns></returns>
		static TOPOLOGIC_API TopologicCore::Edge::Ptr ByVertices(const std::list<TopologicCore::Vertex::Ptr>& rkVertices);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpCenterPoint"></param>
		/// <param name="kRadius"></param>
		/// <param name="kXAxisX"></param>
		/// <param name="kXAxisY"></param>
		/// <param name="kXAxisZ"></param>
		/// <param name="kNormalX"></param>
		/// <param name="kNormalY"></param>
		/// <param name="kNormalZ"></param>
		/// <returns></returns>
		static TOPOLOGIC_API TopologicCore::Edge::Ptr ByCircle(
			const TopologicCore::Vertex::Ptr& kpCenterPoint, const double kRadius,
			const double kXAxisX, const double kXAxisY, const double kXAxisZ,
			const double kNormalX, const double kNormalY, const double kNormalZ);
		
		static TOPOLOGIC_API TopologicCore::Edge::Ptr ByEllipse(
			const std::shared_ptr<TopologicCore::Vertex>& kpCenterPoint, const double kMajorRadius, const double kMinorRadius,
			const double kXAxisX, const double kXAxisY, const double kXAxisZ,
			const double kNormalX, const double kNormalY, const double kNormalZ);

		static TOPOLOGIC_API TopologicCore::Edge::Ptr ByNurbsCurve(
			const std::list<TopologicCore::Vertex::Ptr>& rkControlPoints,
			const std::list<double>& rkKnots,
			const std::list<double>& rkWeights,
			const int kDegree,
			const bool kIsPeriodic,
			const bool kIsRational);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpEdge"></param>
		/// <param name="kpVertex"></param>
		/// <returns></returns>
		static TOPOLOGIC_API double ParameterAtPoint(const TopologicCore::Edge::Ptr& kpEdge, const TopologicCore::Vertex::Ptr& kpVertex);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpEdge"></param>
		/// <param name="kParameter"></param>
		/// <returns></returns>
		static TOPOLOGIC_API TopologicCore::Vertex::Ptr PointAtParameter(const TopologicCore::Edge::Ptr& kpEdge, const double kParameter);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpEdge"></param>
		/// <returns></returns>
		static TOPOLOGIC_API double Length(const TopologicCore::Edge::Ptr& kpEdge);

		static TOPOLOGIC_API void AdjacentWires(
			const TopologicCore::Edge::Ptr & kpEdge,
			const TopologicCore::Topology::Ptr & kpParentTopology,
			std::list<TopologicCore::Wire::Ptr>& rCoreAdjacentWires);

        static void AdjacentWires(
            TopologicCore::Edge const * const kpkEdge,
            const TopologicCore::Topology::Ptr & kpParentTopology,
            std::list<TopologicCore::Wire::Ptr>& rCoreAdjacentWires);

        static TOPOLOGIC_API void AdjacentFaces(
            const TopologicCore::Edge::Ptr & kpEdge,
            const TopologicCore::Topology::Ptr & kpParentTopology,
            std::list<TopologicCore::Face::Ptr>& rCoreAdjacentFaces);

		static double AngleBetween(
			const TopologicCore::Edge::Ptr & kpEdge1,
			const TopologicCore::Edge::Ptr & kpEdge2
		);
	};
}