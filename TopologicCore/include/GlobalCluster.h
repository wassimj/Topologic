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

#include <TopoDS_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <iostream>
#include <memory>

#include <list>

namespace TopologicCore
{
    class Cluster;
	class Topology;

	class GlobalCluster
	{
	public:
		typedef std::shared_ptr<GlobalCluster> Ptr;

	public:
		TOPOLOGIC_API static GlobalCluster& GetInstance()
		{
			static GlobalCluster instance;
			return instance;
		}

		TOPOLOGIC_API GlobalCluster();
		TOPOLOGIC_API ~GlobalCluster();

		TOPOLOGIC_API void AddTopology(const std::shared_ptr<Topology>& rkTopology);

		TOPOLOGIC_API void AddTopology(const TopoDS_Shape& rkOcctShape);

        std::shared_ptr<Cluster> GetCluster();

		void RemoveTopology(const std::shared_ptr<Topology>& rkTopology);

		void RemoveTopology(const TopoDS_Shape& rkOcctShape);

		void Clear();

		const TopoDS_Compound& GetOcctCompound() const;

		TopoDS_Compound& GetOcctCompound();

		TOPOLOGIC_API void SubTopologies(std::list<std::shared_ptr<Topology>>& rSubTopologies) const;

	protected:
		TopoDS_Compound m_occtCompound;
		TopoDS_Builder m_occtBuilder;
	};
}
