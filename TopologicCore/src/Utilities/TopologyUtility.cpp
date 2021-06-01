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

#include <Utilities/TopologyUtility.h>

#include <GlobalCluster.h>
#include <AttributeManager.h>
#include <Context.h>
#include <Shell.h>
#include <CellComplex.h>
#include <Cluster.h>

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_GTransform.hxx>
#include <Geom_Point.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <ShapeFix_Shape.hxx>

#include <algorithm>

namespace TopologicUtilities
{
	TopologicCore::Topology::Ptr TopologyUtility::Translate(const TopologicCore::Topology::Ptr& kpTopology, const double x, const double y, const double z)
	{
		gp_Trsf transformation;
		transformation.SetTranslation(gp_Vec(x, y, z));
		BRepBuilderAPI_Transform transform(kpTopology->GetOcctShape(), transformation, true);
		TopologicCore::Topology::Ptr pCoreTransformedTopology = TopologicCore::Topology::ByOcctShape(transform.Shape(), kpTopology->GetClassGUID());

		TopologicCore::AttributeManager::GetInstance().DeepCopyAttributes(kpTopology->GetOcctShape(), pCoreTransformedTopology->GetOcctShape());

		std::list<TopologicCore::Topology::Ptr> subContents;
		TopologicCore::Topology::SubContents(kpTopology->GetOcctShape(), subContents);

		for (const TopologicCore::Topology::Ptr kpSubContent : subContents)
		{
			// Transform kpSubContent
			TopologicCore::Topology::Ptr transformedSubcontent = Translate(kpSubContent,
				x, y, z);

			// Attach to the same context type
			int contextType = 0;
			std::list<TopologicCore::Context::Ptr> contexts;
			kpSubContent->Contexts(contexts);
			for (const TopologicCore::Context::Ptr& kpContext : contexts)
			{
				TopologicCore::Topology::Ptr pContextTopology = kpContext->Topology();
				TopologicCore::TopologyType contextTopologyType = pContextTopology->GetType();
				contextType = contextType | contextTopologyType;
			}

			std::list<TopologicCore::Topology::Ptr> transformedSubcontents;
			transformedSubcontents.push_back(transformedSubcontent);
			pCoreTransformedTopology->AddContents(transformedSubcontents, contextType);
		}

		TopologicCore::GlobalCluster::GetInstance().AddTopology(pCoreTransformedTopology);
		return pCoreTransformedTopology;
	}

	TopologicCore::Topology::Ptr TopologyUtility::Rotate(const TopologicCore::Topology::Ptr & kpTopology,
		const TopologicCore::Vertex::Ptr& kpOrigin,
		const double kDirectionX, const double kDirectionY, const double kDirectionZ,
		const double kDegree)
	{
		gp_Trsf transformation;
		Handle(Geom_Point) pOcctOrigin = kpOrigin->Point();
		double radian = DegreeToRadian(kDegree);
		transformation.SetRotation(gp_Ax1(gp_Pnt(pOcctOrigin->X(), pOcctOrigin->Y(), pOcctOrigin->Z()), gp_Dir(kDirectionX, kDirectionY, kDirectionZ)), radian);
		BRepBuilderAPI_Transform transform(kpTopology->GetOcctShape(), transformation, true);
		TopologicCore::Topology::Ptr pCoreTransformedTopology = TopologicCore::Topology::ByOcctShape(transform.Shape(), kpTopology->GetClassGUID());

		TopologicCore::AttributeManager::GetInstance().DeepCopyAttributes(kpTopology->GetOcctShape(), pCoreTransformedTopology->GetOcctShape());

		std::list<TopologicCore::Topology::Ptr> subContents;
		TopologicCore::Topology::SubContents(kpTopology->GetOcctShape(), subContents);

		for (const TopologicCore::Topology::Ptr kpSubContent : subContents)
		{
			// Transform kpSubContent
			TopologicCore::Topology::Ptr transformedSubcontent = Rotate(kpSubContent,
				kpOrigin, kDirectionX, kDirectionY, kDirectionZ, kDegree);

			// Attach to the same context type
			int contextType = 0;
			std::list<TopologicCore::Context::Ptr> contexts;
			kpSubContent->Contexts(contexts);
			for (const TopologicCore::Context::Ptr& kpContext : contexts)
			{
				TopologicCore::Topology::Ptr pContextTopology = kpContext->Topology();
				TopologicCore::TopologyType contextTopologyType = pContextTopology->GetType();
				contextType = contextType | contextTopologyType;
			}

			std::list<TopologicCore::Topology::Ptr> transformedSubcontents;
			transformedSubcontents.push_back(transformedSubcontent);
			pCoreTransformedTopology = pCoreTransformedTopology->AddContents(transformedSubcontents, contextType);
		}

		TopologicCore::GlobalCluster::GetInstance().AddTopology(pCoreTransformedTopology);
		return pCoreTransformedTopology;
	}

	TopologicCore::Topology::Ptr TopologyUtility::Transform(const TopologicCore::Topology::Ptr & kpTopology, const double kTranslationX, const double kTranslationY, const double kTranslationZ, const double kRotation11, const double kRotation12, const double kRotation13, const double kRotation21, const double kRotation22, const double kRotation23, const double kRotation31, const double kRotation32, const double kRotation33)
	{
		gp_GTrsf gTransformation(
			gp_Mat(kRotation11, kRotation12, kRotation13, kRotation21, kRotation22, kRotation23, kRotation31, kRotation32, kRotation33),
			gp_XYZ(kTranslationX, kTranslationY, kTranslationZ)
		);
		BRepBuilderAPI_GTransform gTransform(kpTopology->GetOcctShape(), gTransformation, true);
		TopologicCore::Topology::Ptr pCoreTransformedTopology = TopologicCore::Topology::ByOcctShape(gTransform.Shape(), kpTopology->GetClassGUID());

		TopologicCore::AttributeManager::GetInstance().DeepCopyAttributes(kpTopology->GetOcctShape(), pCoreTransformedTopology->GetOcctShape());

		std::list<TopologicCore::Topology::Ptr> subContents;
		TopologicCore::Topology::SubContents(kpTopology->GetOcctShape(), subContents);

		for (const TopologicCore::Topology::Ptr kpSubContent : subContents)
		{
			// Transform kpSubContent
			TopologicCore::Topology::Ptr transformedSubcontent = Transform(kpSubContent,
				kTranslationX, kTranslationY, kTranslationZ,
				kRotation11, kRotation12, kRotation13,
				kRotation21, kRotation22, kRotation23,
				kRotation31, kRotation32, kRotation33);

			// Attach to the same context type
			int contextType = 0;
			std::list<TopologicCore::Context::Ptr> contexts;
			kpSubContent->Contexts(contexts);
			for (const TopologicCore::Context::Ptr& kpContext : contexts)
			{
				TopologicCore::Topology::Ptr pContextTopology = kpContext->Topology();
				TopologicCore::TopologyType contextTopologyType = pContextTopology->GetType();
				contextType = contextType | contextTopologyType;
			}

			std::list<TopologicCore::Topology::Ptr> transformedSubcontents;
			transformedSubcontents.push_back(transformedSubcontent);
			pCoreTransformedTopology = pCoreTransformedTopology->AddContents(transformedSubcontents, contextType);

		}

		TopologicCore::GlobalCluster::GetInstance().AddTopology(pCoreTransformedTopology);
		return pCoreTransformedTopology;
	}

	TopologicCore::Topology::Ptr TopologyUtility::Scale(
		const TopologicCore::Topology::Ptr & kpTopology, const TopologicCore::Vertex::Ptr & kpOrigin,
		const double kXFactor, const double kYFactor, const double kZFactor)
	{
		Handle(Geom_Point) pOcctScaleOrigin = kpOrigin->Point();
		gp_Trsf transformation1;
		transformation1.SetTranslation(gp_Vec(-pOcctScaleOrigin->X(), -pOcctScaleOrigin->Y(), -pOcctScaleOrigin->Z()));
		BRepBuilderAPI_Transform transform1(kpTopology->GetOcctShape(), transformation1, true);

		gp_GTrsf occtGTransformation(
			gp_Mat(
				kXFactor, 0.0, 0.0,
				0.0, kYFactor, 0.0,
				0.0, 0.0, kZFactor),
			gp_XYZ(0.0, 0.0, 0.0)
		);

		BRepBuilderAPI_GTransform occtTransform(transform1.Shape(), occtGTransformation, true);
		gp_Trsf transformation2;
		transformation2.SetTranslation(gp_Vec(pOcctScaleOrigin->X(), pOcctScaleOrigin->Y(), pOcctScaleOrigin->Z()));
		BRepBuilderAPI_Transform transform2(occtTransform.Shape(), transformation2, true);
		TopoDS_Shape occtTransformedShape = transform2.Shape();
		TopologicCore::Topology::Ptr pCoreTransformedTopology = TopologicCore::Topology::ByOcctShape(transform2.Shape(), kpTopology->GetClassGUID());
		TopologicCore::AttributeManager::GetInstance().DeepCopyAttributes(kpTopology->GetOcctShape(), pCoreTransformedTopology->GetOcctShape());

		std::list<TopologicCore::Topology::Ptr> subContents;
		TopologicCore::Topology::SubContents(kpTopology->GetOcctShape(), subContents);

		for (const TopologicCore::Topology::Ptr kpSubContent : subContents)
		{
			// Transform kpSubContent
			TopologicCore::Topology::Ptr transformedSubcontent = Scale(kpSubContent,
				kpOrigin, kXFactor, kYFactor, kZFactor);

			// Attach to the same context type
			int contextType = 0;
			std::list<TopologicCore::Context::Ptr> contexts;
			kpSubContent->Contexts(contexts);
			for (const TopologicCore::Context::Ptr& kpContext : contexts)
			{
				TopologicCore::Topology::Ptr pContextTopology = kpContext->Topology();
				TopologicCore::TopologyType contextTopologyType = pContextTopology->GetType();
				contextType = contextType | contextTopologyType;
			}

			std::list<TopologicCore::Topology::Ptr> transformedSubcontents;
			transformedSubcontents.push_back(transformedSubcontent);
			pCoreTransformedTopology->AddContents(transformedSubcontents, contextType);
		}
		TopologicCore::GlobalCluster::GetInstance().AddTopology(pCoreTransformedTopology);
		return pCoreTransformedTopology;
	}

	double TopologyUtility::DegreeToRadian(const double kDegree)
	{
		return kDegree * M_PI / 180.0;
	}

	double TopologyUtility::RadianToDegree(const double kRadian)
	{
		return kRadian * 180.0 / M_PI;
	}

	void TopologyUtility::AdjacentTopologies(const TopologicCore::Topology::Ptr & kpCoreTopology, const TopologicCore::Topology::Ptr & kpParentTopology, const int kTypeFilter,
		std::list<TopologicCore::Topology::Ptr>& rCoreAdjacentTopologies)
	{
		// Reject: Shell, Cell, CellComplex, Cluster
		if (kpCoreTopology->GetType() == TopologicCore::Shell::Type() ||
			kpCoreTopology->GetType() == TopologicCore::Cell::Type() ||
			kpCoreTopology->GetType() == TopologicCore::CellComplex::Type() ||
			kpCoreTopology->GetType() == TopologicCore::Cluster::Type())
		{
			std::string errorMessage = "Does not accept an input Topology of type " + kpCoreTopology->GetTypeAsString();
			throw std::runtime_error(errorMessage.c_str());
		}

		kpCoreTopology->UpwardNavigation(kpCoreTopology->GetOcctShape(), kTypeFilter, rCoreAdjacentTopologies);
	}
}