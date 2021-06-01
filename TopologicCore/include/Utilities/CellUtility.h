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
#include <Vertex.h>
#include <Wire.h>

#include <memory>

namespace TopologicUtilities
{
	enum CellContainmentState
	{
		INSIDE,
		ON_BOUNDARY,
		OUTSIDE,
		UNKNOWN
	};

	class CellUtility
	{
	public:
		typedef std::shared_ptr<CellUtility> Ptr;

	public:
		/*/// <summary>
		/// 
		/// </summary>
		/// <param name="rkVertices"></param>
		/// <param name="rkFaceIndices"></param>
		/// <returns></returns>
		static TOPOLOGIC_API TopologicCore::Cell::Ptr ByVerticesFaceIndices(const std::vector<TopologicCore::Vertex::Ptr>& rkVertices, const std::list<std::list<int>>& rkFaceIndices);*/

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkWires"></param>
		/// <returns></returns>
		static TOPOLOGIC_API TopologicCore::Cell::Ptr ByLoft(const std::list<TopologicCore::Wire::Ptr>& rkWires);

		//
		static TOPOLOGIC_API TopologicCore::Cell::Ptr ByCuboid(
			const double kXCentroid, const double kYCentroid, const double kZCentroid,
			const double kXDimension, const double kYDimension, const double kZDimension,
			const double kXNormal, const double kYNormal, const double kZNormal,
			const double kXAxisX, const double kYAxisX, const double kZAxisX,
			const double kXAxisY, const double kYAxisY, const double kZAxisY);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kCenterX"></param>
		/// <param name="kCenterY"></param>
		/// <param name="kCenterZ"></param>
		/// <param name="kRadius"></param>
		/// <returns></returns>
		static TOPOLOGIC_API TopologicCore::Cell::Ptr BySphere(const double kCenterX, const double kCenterY, const double kCenterZ, const double kRadius);

		static TOPOLOGIC_API TopologicCore::Cell::Ptr ByTwoCorners(const std::shared_ptr<TopologicCore::Vertex>& kpMinVertex, const std::shared_ptr<TopologicCore::Vertex>& kpMaxVertex);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kReferencePointX"></param>
		/// <param name="kReferencePointY"></param>
		/// <param name="kReferencePointZ"></param>
		/// <param name="kNormalX"></param>
		/// <param name="kNormalY"></param>
		/// <param name="kNormalZ"></param>
		/// <param name="kRadius"></param>
		/// <param name="kHeight"></param>
		/// <returns></returns>
		static TOPOLOGIC_API TopologicCore::Cell::Ptr ByCylinder(
			const double kReferencePointX, const double kReferencePointY, const double kReferencePointZ,
			const double kNormalX, const double kNormalY, const double kNormalZ,
			const double kRadius, const double kHeight);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kReferencePointX"></param>
		/// <param name="kReferencePointY"></param>
		/// <param name="kReferencePointZ"></param>
		/// <param name="kNormalX"></param>
		/// <param name="kNormalY"></param>
		/// <param name="kNormalZ"></param>
		/// <param name="kXDirectionX"></param>
		/// <param name="kXDirectionY"></param>
		/// <param name="kDirectionZ"></param>
		/// <param name="kRadius1"></param>
		/// <param name="kRadius2"></param>
		/// <param name="kHeight"></param>
		/// <returns></returns>
		static TOPOLOGIC_API TopologicCore::Cell::Ptr ByCone(
			const double kReferencePointX, const double kReferencePointY, const double kReferencePointZ,
			const double kNormalX, const double kNormalY, const double kNormalZ,
			const double kXDirectionX, const double kXDirectionY, const double kDirectionZ,
			const double kRadius1, const double kRadius2, const double kHeight);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpCell"></param>
		/// <param name="kTolerance"></param>
		/// <returns></returns>
		static TOPOLOGIC_API std::shared_ptr<TopologicCore::Vertex> InternalVertex(
			const TopologicCore::Cell::Ptr kpCell, 
			const double kTolerance = 0.0001);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="rkOcctSolid"></param>
        /// <param name="kTolerance"></param>
        /// <returns></returns>
        static std::shared_ptr<TopologicCore::Vertex> InternalVertex(
            const TopoDS_Solid& rkOcctSolid,
            const double kTolerance = 0.0001);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpCell"></param>
		/// <returns></returns>
		static TOPOLOGIC_API double Volume(const TopologicCore::Cell::Ptr& kpCell);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpCell"></param>
		/// <param name="kpVertex"></param>
		/// <param name="kTolerance"></param>
		/// <returns></returns>
		static TOPOLOGIC_API CellContainmentState Contains(const TopologicCore::Cell::Ptr & kpCell, const TopologicCore::Vertex::Ptr& kpVertex, const double kTolerance = 0.0001);

		static TOPOLOGIC_API void GetMinMax(const TopologicCore::Cell::Ptr & kpCell, double &rMinX, double &rMaxX, double &rMinY, double &rMaxY, double &rMinZ, double &rMaxZ);
	};
}