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

#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>

#include <list>
#include <map>
#include <memory>

namespace TopologicCore
{
	class Topology;
	class Attribute;

	class AttributeManager
	{
	public:
		typedef std::shared_ptr<AttributeManager> Ptr;
		typedef std::map<std::string, std::shared_ptr<Attribute>> AttributeMap;
		typedef std::map<TopoDS_Shape, AttributeMap, TopologicCore::OcctShapeComparator> ShapeToAttributesMap;

	public:
		TOPOLOGIC_API static AttributeManager& GetInstance();

		TOPOLOGIC_API void Add(const std::shared_ptr<TopologicCore::Topology>& kpTopology, const std::string& kAttributeName, const std::shared_ptr<Attribute>& kpAttribute);

		TOPOLOGIC_API void Add(const TopoDS_Shape& rkOcctShape, const std::string& kAttributeName, const std::shared_ptr<Attribute>& kpAttribute);

		TOPOLOGIC_API void Remove(const std::shared_ptr<TopologicCore::Topology>& kpTopology, const std::string& kAttributeName);

		TOPOLOGIC_API void Remove(const TopoDS_Shape& rkOcctShape, const std::string& kAttributeName);

		TOPOLOGIC_API std::shared_ptr<Attribute> Find(const TopoDS_Shape& rkOcctShape, const std::string& rkAttributeName);

		TOPOLOGIC_API bool FindAll(const TopoDS_Shape & rkOcctShape, std::map<std::string, std::shared_ptr<Attribute>>& rAttributes);

		TOPOLOGIC_API void ClearOne(const TopoDS_Shape& rkOcctShape);

		TOPOLOGIC_API void ClearAll();

		TOPOLOGIC_API void CopyAttributes(const TopoDS_Shape& rkOcctOriginShape, const TopoDS_Shape& rkOcctDestinationShape, const bool addDuplicateEntries = false);

		TOPOLOGIC_API void DeepCopyAttributes(const TopoDS_Shape& rkOcctShape1, const TopoDS_Shape& rkOcctShape2);

		void GetAttributesInSubshapes(const TopoDS_Shape& rkOcctShape, ShapeToAttributesMap& rShapesToAttributesMap);

	protected:
		ShapeToAttributesMap  m_occtShapeToAttributesMap;
	};
}