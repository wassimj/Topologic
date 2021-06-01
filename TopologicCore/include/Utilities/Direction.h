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
#include "Vector.h"

#include <Geom_Direction.hxx>

#include <memory>

namespace TopologicUtilities
{
	class Direction
	{
	public:
		typedef std::shared_ptr<Direction> Ptr;

	public:
		Direction(const Handle(Geom_Direction) kpOcctVector);
		~Direction();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kX"></param>
		/// <param name="kY"></param>
		/// <param name="kZ"></param>
		/// <returns></returns>
		TOPOLOGIC_API static Direction::Ptr ByCoordinates(const double kX, const double kY, const double kZ);

		TOPOLOGIC_API static Direction::Ptr ByVector(const Vector::Ptr& kpVector);

	protected:

		Handle(Geom_Direction) m_pOcctDirection;
	};
}