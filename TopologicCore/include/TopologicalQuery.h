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

#include <memory>

namespace TopologicCore
{
	/// <summary>
	/// The root class in the Topologic library
	/// </summary>
	class TopologicalQuery
	{
	public:
		template <typename T>
		static T* Downcast(TopologicalQuery *const kpTopologicalQuery)
		{
			T* pSubclassTopology = dynamic_cast<T*>(kpTopologicalQuery);
			if (pSubclassTopology == nullptr)
			{
				throw std::runtime_error("Failed downcasting a topological query");
			}
			return pSubclassTopology;
		}

		template <typename T>
		static T const * Downcast(TopologicalQuery const *const kpkTopologicalQuery)
		{
			T const * kpSubclassTopology = dynamic_cast<T const *>(kpkTopologicalQuery);
			if (kpSubclassTopology == nullptr)
			{
				throw std::runtime_error("Failed downcasting topology");
			}
			return kpSubclassTopology;
		}

		template <typename T>
		static std::shared_ptr<T> Downcast(std::shared_ptr<TopologicalQuery>& rTopologicalQuery)
		{
			std::shared_ptr<T> pSubclassTopology = std::dynamic_pointer_cast<T>(rTopologicalQuery);
			if (pSubclassTopology == nullptr)
			{
				throw std::runtime_error("Failed downcasting a topological query");
			}
			return pSubclassTopology;
		}

		template <typename T>
		static const std::shared_ptr<T> Downcast(const std::shared_ptr<TopologicalQuery>& rkTopologicalQuery)
		{
			const std::shared_ptr<T> kSubclassTopology = std::dynamic_pointer_cast<T>(rkTopologicalQuery);
			if (kSubclassTopology == nullptr)
			{
				throw std::runtime_error("Failed downcasting a topological query");
			}
			return kSubclassTopology;
		}

		template <typename T>
		static std::shared_ptr<T> Upcast(std::shared_ptr<TopologicalQuery>& rTopologicalQuery)
		{
			std::shared_ptr<T> pSuperClassTopology = std::dynamic_pointer_cast<T>(rTopologicalQuery);
			if (pSuperClassTopology == nullptr)
			{
				throw std::runtime_error("Failed upcasting a topological query");
			}
			return pSuperClassTopology;
		}

		template <typename T>
		static const std::shared_ptr<T> Upcast(const std::shared_ptr<TopologicalQuery>& rkTopologicalQuery)
		{
			const std::shared_ptr<T> pSuperClassTopology = std::dynamic_pointer_cast<T>(rkTopologicalQuery);
			if (pSuperClassTopology == nullptr)
			{
				throw std::runtime_error("Failed upcasting a topological query");
			}
			return pSuperClassTopology;
		}

		virtual ~TopologicalQuery() {};

	};
}