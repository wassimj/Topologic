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

#include <Utilities/Direction.h>

namespace TopologicUtilities
{
	Direction::Ptr Direction::ByCoordinates(const double kX, const double kY, const double kZ)
	{
		return std::make_shared<Direction>(new Geom_Direction(kX, kY, kZ));
	}

	Direction::Ptr Direction::ByVector(const Vector::Ptr& kpVector)
	{
		Vector::Ptr normalizedVector = Vector::ByNormalizedVector(kpVector);
		return std::make_shared<Direction>(new Geom_Direction(normalizedVector->X(), normalizedVector->Y(), normalizedVector->Z()));
	}
	Direction::~Direction()
	{
	}
	Direction::Direction(const Handle(Geom_Direction) kpOcctDirection)
		: m_pOcctDirection(kpOcctDirection)
	{
	}
}