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

#include <ContentManager.h>
#include <Context.h>
#include <Topology.h>

#include <algorithm>

namespace TopologicCore
{
	void ContentManager::Add(const TopoDS_Shape& rkOcctShape, const std::shared_ptr<Topology>& kpContentTopology)
	{
		// If the OCCT shape does not have a content, initialise it in the map
		if (m_occtShapeToContentsMap.find(rkOcctShape) == m_occtShapeToContentsMap.end())
		{
			std::list<Topology::Ptr> contents;
			m_occtShapeToContentsMap.insert(std::pair<TopoDS_Shape, std::list<std::shared_ptr<Topology>>>(rkOcctShape, contents));
		}

		m_occtShapeToContentsMap[rkOcctShape].push_back(kpContentTopology);
	}

	void ContentManager::Remove(const TopoDS_Shape& rkOcctShape, const TopoDS_Shape& rkOcctContentTopology)
	{
		if (m_occtShapeToContentsMap.find(rkOcctShape) != m_occtShapeToContentsMap.end())
		{
			m_occtShapeToContentsMap[rkOcctShape].remove_if(
				[&](const Topology::Ptr& kpContent) {
				return kpContent->GetOcctShape().IsSame(rkOcctContentTopology);
			});
		}
	}

	bool ContentManager::Find(const TopoDS_Shape& rkOcctShape, std::list<std::shared_ptr<Topology>>& rContents)
	{
		if (m_occtShapeToContentsMap.find(rkOcctShape) != m_occtShapeToContentsMap.end())
		{
			std::list<Topology::Ptr> contents = m_occtShapeToContentsMap[rkOcctShape];
			rContents.insert(rContents.end(), contents.begin(), contents.end());
			return true;
		}

		return false;
	}

	bool ContentManager::HasContent(const TopoDS_Shape & rkOcctShape, const TopoDS_Shape& rkOcctContentTopology)
	{
		std::list<Topology::Ptr> contents;
		bool hasContents = Find(rkOcctShape, contents);
		if (!hasContents)
		{
			return false;
		}

		std::list<Topology::Ptr>::iterator contentIterator = std::find_if(contents.begin(), contents.end(), 
			[&](const Topology::Ptr& kpContent) { 
			return kpContent->GetOcctShape().IsSame(rkOcctContentTopology);
		});

		return contentIterator != contents.end();
	}

	void ContentManager::ClearOne(const TopoDS_Shape & rkOcctShape)
	{
		if (m_occtShapeToContentsMap.find(rkOcctShape) != m_occtShapeToContentsMap.end())
		{
			m_occtShapeToContentsMap[rkOcctShape].clear();
			m_occtShapeToContentsMap.erase(rkOcctShape);
		}
	}

	void ContentManager::ClearAll()
	{
		m_occtShapeToContentsMap.clear();
	}
}