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

#include <TopologyFactoryManager.h>
#include <TopologyFactory.h>
#include <ClusterFactory.h>
#include <CellComplexFactory.h>
#include <CellFactory.h>
#include <ShellFactory.h>
#include <FaceFactory.h>
#include <WireFactory.h>
#include <EdgeFactory.h>
#include <VertexFactory.h>

namespace TopologicCore
{
	void TopologyFactoryManager::Add(const std::string& rkGuid, const TopologyFactory::Ptr& rkTopologyFactory)
	{
		if (m_topologyFactoryMap.find(rkGuid) == m_topologyFactoryMap.end())
		{
			m_topologyFactoryMap.insert(std::pair<std::string, TopologyFactory::Ptr>(rkGuid, rkTopologyFactory));
		}
	}

	bool TopologyFactoryManager::Find(const std::string& rkGuid, TopologyFactory::Ptr& rTopologyFactory)
	{
		if (m_topologyFactoryMap.find(rkGuid) != m_topologyFactoryMap.end())
		{
			rTopologyFactory = m_topologyFactoryMap[rkGuid];
			return true;
		}

		return false;
	}

	TopologyFactory::Ptr TopologyFactoryManager::GetDefaultFactory(const TopAbs_ShapeEnum kOcctType)
	{
		switch (kOcctType)
		{
		case TopAbs_COMPOUND: return std::make_shared<ClusterFactory>();
		case TopAbs_COMPSOLID: return std::make_shared<CellComplexFactory>();
		case TopAbs_SOLID: return std::make_shared<CellFactory>();
		case TopAbs_SHELL: return std::make_shared<ShellFactory>();
		case TopAbs_FACE: return std::make_shared<FaceFactory>();
		case TopAbs_WIRE: return std::make_shared<WireFactory>();
		case TopAbs_EDGE: return std::make_shared<EdgeFactory>();
		case TopAbs_VERTEX: return std::make_shared<VertexFactory>();
		default:
			throw std::runtime_error("Topology::ByOcctShape: unknown topology.");
		}
	}
}