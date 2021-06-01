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

#include "PlanarSurface.h"
#include "NurbsSurface.h"
#include "Face.h"

#include <ShapeAnalysis.hxx>
#include <GeomConvert.hxx>

namespace TopologicCore
{
	PlanarSurface::PlanarSurface(Handle(Geom_Plane) pOcctPlane, const TopoDS_Face& rkOcctFace)
		: m_pOcctPlane(pOcctPlane)
		, m_occtFace(rkOcctFace)
	{
	}

	PlanarSurface::~PlanarSurface()
	{
	}

	void PlanarSurface::Coefficients(std::list<double>& rCoefficients) const 
	{
		double a = 0.0, b = 0.0, c = 0.0, d = 0.0;
		m_pOcctPlane->Coefficients(a, b, c, d);
		rCoefficients.push_back(a);
		rCoefficients.push_back(b);
		rCoefficients.push_back(c);
		rCoefficients.push_back(d);
	}

	double PlanarSurface::XMin() const
	{
		double uMin = 0.0, uMax = 0.0, vMin = 0.0, vMax = 0.0;
		ShapeAnalysis::GetFaceUVBounds(m_occtFace, uMin, uMax, vMin, vMax);
		return uMin;
	}

	double PlanarSurface::XMax() const
	{
		double uMin = 0.0, uMax = 0.0, vMin = 0.0, vMax = 0.0;
		ShapeAnalysis::GetFaceUVBounds(m_occtFace, uMin, uMax, vMin, vMax);
		return uMax;
	}

	double PlanarSurface::YMin() const
	{
		double uMin = 0.0, uMax = 0.0, vMin = 0.0, vMax = 0.0;
		ShapeAnalysis::GetFaceUVBounds(m_occtFace, uMin, uMax, vMin, vMax);
		return vMin;
	}

	double PlanarSurface::YMax() const
	{
		double uMin = 0.0, uMax = 0.0, vMin = 0.0, vMax = 0.0;
		ShapeAnalysis::GetFaceUVBounds(m_occtFace, uMin, uMax, vMin, vMax);
		return vMax;
	}

	NurbsSurface::Ptr PlanarSurface::ToNurbsSurface() const
	{
		try {
			
			Handle(Geom_BSplineSurface) occtBSplineSurface = GeomConvert::SurfaceToBSplineSurface(m_pOcctPlane);
			return std::make_shared<NurbsSurface>(occtBSplineSurface, std::make_shared<Face>(m_occtFace, ""));
		}
		catch (Standard_DomainError e)
		{
			throw std::runtime_error(e.GetMessageString());
		}
	}
}