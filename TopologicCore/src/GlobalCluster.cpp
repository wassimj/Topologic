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

#include "GlobalCluster.h"
#include "Cluster.h"
#include "Topology.h"

namespace TopologicCore
{
	void GlobalCluster::AddTopology(const std::shared_ptr<Topology>& rkTopology)
	{
        if (rkTopology != nullptr)
        {
            AddTopology(rkTopology->GetOcctShape());
        }
	}

	void GlobalCluster::AddTopology(const TopoDS_Shape & rkOcctShape)
	{
		m_occtBuilder.Add(m_occtCompound, rkOcctShape);
	}

    Cluster::Ptr GlobalCluster::GetCluster()
    {
        return std::make_shared<TopologicCore::Cluster>(m_occtCompound);
    }

	void GlobalCluster::RemoveTopology(const std::shared_ptr<Topology>& rkTopology)
	{
		RemoveTopology(rkTopology->GetOcctShape());
	}

	void GlobalCluster::RemoveTopology(const TopoDS_Shape & rkOcctShape)
	{
		m_occtBuilder.Remove(m_occtCompound, rkOcctShape);
	}

	void GlobalCluster::Clear()
	{
		m_occtBuilder.MakeCompound(m_occtCompound);
	}

	const TopoDS_Compound& GlobalCluster::GetOcctCompound() const
	{
		return m_occtCompound;
	}
	
	TopoDS_Compound& GlobalCluster::GetOcctCompound()
	{
		return m_occtCompound;
	}

	void GlobalCluster::SubTopologies(std::list<Topology::Ptr>& rSubTopologies) const
	{
		TopTools_ListOfShape occtListMembers;
		Topology::SubTopologies(GetOcctCompound(), occtListMembers);
		for (TopTools_ListIteratorOfListOfShape occtIterator(occtListMembers);
			occtIterator.More();
			occtIterator.Next())
		{
			Topology::Ptr pMemberTopology = Topology::ByOcctShape(occtIterator.Value(), "");
			rSubTopologies.push_back(pMemberTopology);
		}
	}

	GlobalCluster::GlobalCluster()
	{
		m_occtBuilder.MakeCompound(m_occtCompound);
	}

	GlobalCluster::~GlobalCluster()
	{
	}
}