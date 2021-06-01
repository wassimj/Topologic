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
#include "Geometry.h"

#include <Geom_BSplineCurve.hxx>
#include <TopoDS_Edge.hxx>

#include <list>

namespace TopologicCore
{
	class Vertex;

	class NurbsCurve : Geometry
	{
	public:
		typedef std::shared_ptr<NurbsCurve> Ptr;

	public:
		TOPOLOGIC_API NurbsCurve(Handle(Geom_BSplineCurve) pOcctBSplineCurve, const TopoDS_Edge& rkOcctEdge);
		virtual ~NurbsCurve();

		TOPOLOGIC_API bool IsPeriodic() const;

		TOPOLOGIC_API bool IsRational() const;

		TOPOLOGIC_API int Degree() const;

		TOPOLOGIC_API void ControlVertices(std::list<std::shared_ptr<Vertex>>& rControlVertices) const;

		TOPOLOGIC_API void Knots(std::list<double>& rKnots) const;

		TOPOLOGIC_API double FirstParameter() const;

		TOPOLOGIC_API double LastParameter() const;

	protected:
		Handle(Geom_BSplineCurve) m_pOcctBSplineCurve;
		TopoDS_Edge m_occtEdge;
	};
}