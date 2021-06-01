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

#include <Utilities/Vector.h>

namespace TopologicUtilities
{
	Vector::Ptr Vector::ByCoordinates(const double kX, const double kY, const double kZ)
	{
		return std::make_shared<Vector>(new Geom_VectorWithMagnitude(kX, kY, kZ));
	}

	Vector::Ptr Vector::ByReverseVector(const Vector::Ptr& kpAnotherVector)
	{
		Handle(Geom_Vector) pOcctReversedVector = kpAnotherVector->m_pOcctVector->Reversed();
		return std::make_shared<Vector>(Handle(Geom_VectorWithMagnitude)::DownCast(pOcctReversedVector));
	}

	Vector::Ptr Vector::ByScaledVector(const Vector::Ptr& kpAnotherVector, const double kScalingFactor)
	{
		return std::make_shared<Vector>(kpAnotherVector->m_pOcctVector->Multiplied(kScalingFactor));
	}

	Vector::Ptr Vector::ByNormalizedVector(const Vector::Ptr& kpAnotherVector)
	{
		Handle(Geom_VectorWithMagnitude) pOcctNormalizedVector = kpAnotherVector->m_pOcctVector->Normalized();
		return std::make_shared<Vector>(pOcctNormalizedVector);
	}

	double Vector::Magnitude() const
	{
		return m_pOcctVector->Magnitude();
	}

	double Vector::X() const
	{
		return m_pOcctVector->X();
	}

	double Vector::Y() const
	{
		return m_pOcctVector->Y();
	}

	double Vector::Z() const
	{
		return m_pOcctVector->Z();
	}

	Vector::~Vector()
	{
	}

	Vector::Vector(const Handle(Geom_VectorWithMagnitude) kpOcctVector)
		: m_pOcctVector(kpOcctVector)
	{
	}
}