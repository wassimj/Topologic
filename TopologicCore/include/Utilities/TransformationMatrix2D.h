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

#include <Geom2d_Transformation.hxx>

#include <Utilities.h>

#include <memory>

namespace TopologicUtilities
{
	class TransformationMatrix2D
	{
	public:
		typedef std::shared_ptr<TransformationMatrix2D> Ptr;

	public:
		TransformationMatrix2D(const Handle(Geom2d_Transformation) kpOcctTransformationMatrix2D);
		~TransformationMatrix2D();

	protected:
		Handle(Geom2d_Transformation) m_pOcctTransformationMatrix2D;
	};
}