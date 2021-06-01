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

#include "NurbsCurve.h"
#include "Vertex.h"

#include <BRep_Tool.hxx>

namespace TopologicCore
{
	NurbsCurve::NurbsCurve(Handle(Geom_BSplineCurve) pOcctBSplineCurve, const TopoDS_Edge& rkOcctEdge)
		: m_pOcctBSplineCurve(pOcctBSplineCurve)
		, m_occtEdge(rkOcctEdge)
	{
	}

	NurbsCurve::~NurbsCurve()
	{
	}

	bool NurbsCurve::IsPeriodic() const
	{
		return m_pOcctBSplineCurve->IsPeriodic();
	}

	bool NurbsCurve::IsRational() const
	{
		return m_pOcctBSplineCurve->IsRational();
	}

	int NurbsCurve::Degree() const
	{
		return m_pOcctBSplineCurve->Degree();
	}

	double NurbsCurve::FirstParameter() const
	{
		double u0 = 0.0, u1 = 0.0;
		Handle(Geom_Curve) pOcctCurve = BRep_Tool::Curve(m_occtEdge, u0, u1);
		return u0;
	}

	double NurbsCurve::LastParameter() const
	{
		double u0 = 0.0, u1 = 0.0;
		Handle(Geom_Curve) pOcctCurve = BRep_Tool::Curve(m_occtEdge, u0, u1);
		return u1;
	}

	void NurbsCurve::ControlVertices(std::list<std::shared_ptr<Vertex>>& rControlVertices) const
	{
		TColgp_Array1OfPnt occtControlPoints = m_pOcctBSplineCurve->Poles();
		for (int i = occtControlPoints.Lower(); i <= occtControlPoints.Upper(); i++)
		{
			const gp_Pnt& rkControlPoint = occtControlPoints.Value(i);
			Vertex::Ptr pControlVertex = Vertex::ByCoordinates(rkControlPoint.X(), rkControlPoint.Y(), rkControlPoint.Z());
			rControlVertices.push_back(pControlVertex);
		}
	}

	void NurbsCurve::Knots(std::list<double>& rKnots) const
	{
		const TColStd_Array1OfReal& krOcctKnots = m_pOcctBSplineCurve->Knots();
		for (int i = krOcctKnots.Lower(); i <= krOcctKnots.Upper(); i++)
		{
			int multiplicity = m_pOcctBSplineCurve->Multiplicity(i);
			for (int j = 0; j < multiplicity; j++)
			{
				double occtKnot = krOcctKnots.Value(i);
				rKnots.push_back(occtKnot);
			}
		}
	}
}