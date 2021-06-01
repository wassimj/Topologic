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

#include <Utilities/ShellUtility.h>
#include <Utilities/EdgeUtility.h>
#include <Utilities/FaceUtility.h>

#include <Aperture.h>
#include <GlobalCluster.h>

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>

#include <array>

namespace TopologicUtilities
{
	bool SutherlandHodgmanIsInside(const Handle(Geom2d_CartesianPoint)& kpPoint, const Handle(Geom2d_CartesianPoint)& kpLinePoint1, const Handle(Geom2d_CartesianPoint)& kpLinePoint2)
	{
		return (kpLinePoint2->X() - kpLinePoint1->X())*(kpPoint->Y() - kpLinePoint1->Y()) >
			(kpLinePoint2->Y() - kpLinePoint1->Y())*(kpPoint->X() - kpLinePoint1->X());
	}

	Handle(Geom2d_CartesianPoint) ComputeIntersection(const Handle(Geom2d_CartesianPoint)& S, const Handle(Geom2d_CartesianPoint)& E, const Handle(Geom2d_CartesianPoint)& kpLinePoint1, const Handle(Geom2d_CartesianPoint)& kpLinePoint2)
	{
		Handle(Geom2d_CartesianPoint) dc = new Geom2d_CartesianPoint(kpLinePoint1->X() - kpLinePoint2->X(), kpLinePoint1->Y() - kpLinePoint2->Y());
		Handle(Geom2d_CartesianPoint) dp = new Geom2d_CartesianPoint(S->X() - E->X(), S->Y() - E->Y());
		double n1 = kpLinePoint1->X() * kpLinePoint2->Y() - kpLinePoint1->Y() * kpLinePoint2->X();
		double n2 = S->X() * E->Y() - S->Y() * E->X();
		double n3 = 1.0 / (dc->X() * dp->Y() - dc->Y() * dp->X());
		return new Geom2d_CartesianPoint((n1*dp->X() - n2 * dc->X()) * n3, (n1*dp->Y() - n2 * dc->Y()) * n3);
	}

	TopologicCore::Shell::Ptr ShellUtility::ByLoft(const std::list<TopologicCore::Wire::Ptr>& rkWires)
	{
		BRepOffsetAPI_ThruSections occtLoft;
		for (const TopologicCore::Wire::Ptr& kpWire : rkWires)
		{
			occtLoft.AddWire(kpWire->GetOcctWire());
		};
		try {
			occtLoft.Build();
		}
		catch (...)
		{
			throw std::runtime_error("Loft error");
		}
		TopologicCore::Shell::Ptr pShell = std::make_shared<TopologicCore::Shell>(TopoDS::Shell(occtLoft.Shape()));
		TopologicCore::Shell::Ptr pCopyShell = std::dynamic_pointer_cast<TopologicCore::Shell>(pShell->DeepCopy());
		TopologicCore::GlobalCluster::GetInstance().AddTopology(pCopyShell);
		return pCopyShell;
	}

}