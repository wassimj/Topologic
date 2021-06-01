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

#include <Vertex.h>
#include <Edge.h>

#include <memory>

namespace TopologicUtilities
{
	class VertexUtility
	{
	public:
		typedef std::shared_ptr<VertexUtility> Ptr;

	public:
		static TOPOLOGIC_API void AdjacentEdges(
			const TopologicCore::Vertex::Ptr & kpVertex, 
			const TopologicCore::Topology::Ptr & kpParentTopology,
			std::list<TopologicCore::Edge::Ptr>& rCoreAdjacentEdges);

		static TOPOLOGIC_API void AdjacentEdges(
			const TopologicCore::Vertex::Ptr & kpVertex,
			TopologicCore::Topology const * const kpkParentTopology,
			std::list<TopologicCore::Edge::Ptr>& rCoreAdjacentEdges);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpVertex"></param>
		/// <param name="kpTopology"></param>
		/// <returns></returns>
		static TOPOLOGIC_API double Distance(
			const TopologicCore::Vertex::Ptr& kpVertex, 
			const TopologicCore::Topology::Ptr& kpTopology);

	};
}