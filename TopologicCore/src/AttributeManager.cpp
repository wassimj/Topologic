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

#include "AttributeManager.h"
#include "Attribute.h"
#include "ListAttribute.h"
#include "Topology.h"
#include "Utilities/CellUtility.h"

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

namespace TopologicCore
{
	AttributeManager & AttributeManager::GetInstance()
	{
		static AttributeManager instance;
		return instance;
	}

	void AttributeManager::Add(const std::shared_ptr<TopologicCore::Topology>& kpTopology, const std::string & kAttributeName, const std::shared_ptr<Attribute>& kpAttribute)
	{
		Add(kpTopology->GetOcctShape(), kAttributeName, kpAttribute);
	}

	void AttributeManager::Add(const TopoDS_Shape& rkOcctShape, const std::string& kAttributeName, const std::shared_ptr<Attribute>& kpAttribute)
	{
		if (m_occtShapeToAttributesMap.find(rkOcctShape) == m_occtShapeToAttributesMap.end())
		{
			std::map<std::string, Attribute::Ptr> attributeMap;
			m_occtShapeToAttributesMap.insert(std::pair<TopoDS_Shape, AttributeMap>(rkOcctShape, attributeMap));
		}

		m_occtShapeToAttributesMap[rkOcctShape][kAttributeName] = kpAttribute;
	}

	void AttributeManager::Remove(const TopologicCore::Topology::Ptr& kpTopology, const std::string& kAttributeName)
	{
		Remove(kpTopology->GetOcctShape(), kAttributeName);
	}

	void AttributeManager::Remove(const TopoDS_Shape& rkOcctShape, const std::string& kAttributeName)
	{
		if (m_occtShapeToAttributesMap.find(rkOcctShape) != m_occtShapeToAttributesMap.end())
		{
			m_occtShapeToAttributesMap[rkOcctShape].erase(kAttributeName);
		}
	}

	Attribute::Ptr AttributeManager::Find(const TopoDS_Shape& rkOcctShape, const std::string& rkAttributeName)
	{
		if (m_occtShapeToAttributesMap.find(rkOcctShape) != m_occtShapeToAttributesMap.end())
		{
			std::map<std::string, Attribute::Ptr>& rkAttributeMap = m_occtShapeToAttributesMap[rkOcctShape];
			if (rkAttributeMap.find(rkAttributeName) != rkAttributeMap.end())
			{
				return rkAttributeMap[rkAttributeName];
			}
			return nullptr;
		}

		return nullptr;
	}

	bool AttributeManager::FindAll(const TopoDS_Shape & rkOcctShape, std::map<std::string, std::shared_ptr<Attribute>>& rAttributes)
	{
		if (m_occtShapeToAttributesMap.find(rkOcctShape) != m_occtShapeToAttributesMap.end())
		{
			rAttributes = m_occtShapeToAttributesMap[rkOcctShape];
			return true;
		}

		return false;
	}

	void AttributeManager::ClearOne(const TopoDS_Shape & rkOcctShape)
	{
		if (m_occtShapeToAttributesMap.find(rkOcctShape) != m_occtShapeToAttributesMap.end())
		{
			m_occtShapeToAttributesMap[rkOcctShape].clear();
			m_occtShapeToAttributesMap.erase(rkOcctShape);
		}
	}

	void AttributeManager::ClearAll()
	{
		m_occtShapeToAttributesMap.clear();
	}

	void AttributeManager::CopyAttributes(const TopoDS_Shape& rkOcctOriginShape, const TopoDS_Shape& rkOcctDestinationShape, const bool addDuplicateEntries)
	{
		std::map<std::string, Attribute::Ptr> originAttributes;
		bool doesOriginHaveDictionary = FindAll(rkOcctOriginShape, originAttributes);
		if (!doesOriginHaveDictionary)
		{
			return;
		}
		
		std::map<std::string, Attribute::Ptr> destinationAttributes;
		bool doesDestinationHaveDictionary = FindAll(rkOcctDestinationShape, destinationAttributes);
		if (doesDestinationHaveDictionary)
		{
			for(auto originAttribute : originAttributes)
			{
				// This mode will add values of the same keys into a list
				if (addDuplicateEntries)
				{
					// Does the key already exist in the destination's Dictionary?
					auto destinationAttributeIterator = destinationAttributes.find(originAttribute.first);

					// If yes (there is already an attribe), create a list
					if (destinationAttributeIterator != destinationAttributes.end())
					{
						Attribute::Ptr oldDestinationAttribute = destinationAttributeIterator->second;
							
						// If a list, get the old list
						ListAttribute::Ptr oldListAttribute = std::dynamic_pointer_cast<ListAttribute>(oldDestinationAttribute);
						std::list<Attribute::Ptr> attributes;
						if (oldListAttribute != nullptr)
						{
							attributes = oldListAttribute->ListValue();
						}
						else // get the old single value
						{
							attributes.push_back(oldDestinationAttribute);
						}

						attributes.push_back(originAttribute.second);
						destinationAttributes[originAttribute.first] = std::make_shared<ListAttribute>(attributes);
					}

					// If not, assign the value from the origin
					else
					{
						destinationAttributes[originAttribute.first] = originAttribute.second;
					}
				}

				// This mode will overwrite an old value with the same key
				else
				{
					destinationAttributes[originAttribute.first] = originAttribute.second;
				}
			}
			m_occtShapeToAttributesMap[rkOcctDestinationShape] = destinationAttributes;
		}else
		{
			m_occtShapeToAttributesMap[rkOcctDestinationShape] = std::map<std::string, Attribute::Ptr>(originAttributes);
		}
	}

	void AttributeManager::DeepCopyAttributes(const TopoDS_Shape& rkOcctShape1, const TopoDS_Shape& rkOcctShape2)
	{
		// For parent topology
        std::map<std::string, Attribute::Ptr> attributes;
        bool isFound1 = FindAll(rkOcctShape1, attributes);
        if (isFound1)
        {
            TopoDS_Shape occtSelectedSubtopology = Topology::SelectSubtopology(
                rkOcctShape2,
                rkOcctShape1.ShapeType() == TopAbs_SOLID ?
                TopologicUtilities::CellUtility::InternalVertex(TopoDS::Solid(rkOcctShape1), 0.0001)->GetOcctVertex() :
                Topology::CenterOfMass(rkOcctShape1),
                Topology::GetTopologyType(rkOcctShape1.ShapeType()));
            if (!occtSelectedSubtopology.IsNull())
            {
                CopyAttributes(rkOcctShape1, occtSelectedSubtopology);
            }
        }

		// Get all subtopologies
		for (int occtShapeTypeInt = (int)rkOcctShape1.ShapeType() + 1; occtShapeTypeInt < (int)TopAbs_SHAPE; ++occtShapeTypeInt)
		{
			TopAbs_ShapeEnum occtShapeType = (TopAbs_ShapeEnum)occtShapeTypeInt;
			for (TopExp_Explorer occtExplorer(rkOcctShape1, occtShapeType); occtExplorer.More(); occtExplorer.Next())
			{
				TopoDS_Shape occtSubshape1 = occtExplorer.Current();
				std::map<std::string, Attribute::Ptr> attributes;
				bool isFound2 = FindAll(occtSubshape1, attributes);
				if (!isFound2)
				{
					continue;
				}

				// WARNING: very costly. Only do this if necessary.
                TopoDS_Shape occtSelectedSubtopology = Topology::SelectSubtopology(
                    rkOcctShape2,
                    rkOcctShape1.ShapeType() == TopAbs_SOLID ?
                        TopologicUtilities::CellUtility::InternalVertex(TopoDS::Solid(occtSubshape1), 0.0001)->GetOcctVertex() :
                        Topology::CenterOfMass(occtSubshape1),
                    Topology::GetTopologyType(occtSubshape1.ShapeType()));
				if (!occtSelectedSubtopology.IsNull())
				{
					CopyAttributes(occtSubshape1, occtSelectedSubtopology);
				}
			}
		}
	}

	void AttributeManager::GetAttributesInSubshapes(const TopoDS_Shape & rkOcctShape, ShapeToAttributesMap & rShapesToAttributesMap)
	{
		// For parent topology
		AttributeMap parentAttributeMap;
		bool isFound = FindAll(rkOcctShape, parentAttributeMap);
		if (!parentAttributeMap.empty())
		{
			rShapesToAttributesMap.insert(std::pair<TopoDS_Shape, AttributeMap>(rkOcctShape, parentAttributeMap));
		}

		// Get all subtopologies
		for (int occtShapeTypeInt = (int)rkOcctShape.ShapeType() + 1; occtShapeTypeInt < (int)TopAbs_SHAPE; ++occtShapeTypeInt)
		{
			TopAbs_ShapeEnum occtShapeType = (TopAbs_ShapeEnum)occtShapeTypeInt;
			for (TopExp_Explorer occtExplorer(rkOcctShape, occtShapeType); occtExplorer.More(); occtExplorer.Next())
			{
				TopoDS_Shape occtSubshape = occtExplorer.Current();
				AttributeMap childAttributeMap;
				bool isFound = FindAll(occtSubshape, childAttributeMap);
				if (!childAttributeMap.empty())
				{
					rShapesToAttributesMap.insert(std::pair<TopoDS_Shape, AttributeMap>(occtSubshape, childAttributeMap));
				}
			}
		}
	}
}