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
#include "Surface.h"

#include <Geom_BSplineSurface.hxx>

#include <vector>
#include <list>

namespace TopologicCore
{
	class Vertex;
	class Face;

	class NurbsSurface : Surface
	{
	public:
		typedef std::shared_ptr<NurbsSurface> Ptr;

	public:
		TOPOLOGIC_API NurbsSurface(Handle(Geom_BSplineSurface) pOcctBSplineSurface, const std::shared_ptr<Face> kpFace);
		virtual ~NurbsSurface();

		TOPOLOGIC_API int UDegree() const;

		TOPOLOGIC_API int VDegree() const;

		TOPOLOGIC_API bool IsURational() const;

		TOPOLOGIC_API bool IsVRational() const;

		TOPOLOGIC_API int NumOfUControlVertices() const;

		TOPOLOGIC_API int NumOfVControlVertices() const;

		TOPOLOGIC_API std::shared_ptr<Vertex> ControlVertex(const int u, const int v) const;

		TOPOLOGIC_API void UKnots(std::list<double>& rUKnots) const;

		TOPOLOGIC_API void VKnots(std::list<double>& rVKnots) const;

	protected:
		Handle(Geom_BSplineSurface) m_pOcctBSplineSurface;
		std::shared_ptr<Face> m_pFace;	// This is needed to get the wires information
	};
}