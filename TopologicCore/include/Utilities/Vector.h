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

#include "Utilities.h"

#include <Geom_VectorWithMagnitude.hxx>

#include <memory>

namespace TopologicUtilities
{
	class Vector
	{
	public:
		typedef std::shared_ptr<Vector> Ptr;

	public:
		Vector(const Handle(Geom_VectorWithMagnitude) kpOcctVector);
		~Vector();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kX"></param>
		/// <param name="kY"></param>
		/// <param name="kZ"></param>
		/// <returns></returns>
		TOPOLOGIC_API static Vector::Ptr ByCoordinates(const double kX, const double kY, const double kZ);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpAnotherVector"></param>
		/// <returns></returns>
		TOPOLOGIC_API static Vector::Ptr ByReverseVector(const Vector::Ptr& kpAnotherVector);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpAnotherVector"></param>
		/// <param name="kScalingFactor"></param>
		/// <returns></returns>
		TOPOLOGIC_API static Vector::Ptr ByScaledVector(const Vector::Ptr& kpAnotherVector, const double kScalingFactor);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpAnotherVector"></param>
		/// <returns></returns>
		TOPOLOGIC_API static Vector::Ptr ByNormalizedVector(const Vector::Ptr& kpAnotherVector);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API double Magnitude() const;

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

	protected:
		Handle(Geom_VectorWithMagnitude) m_pOcctVector;
	};
}