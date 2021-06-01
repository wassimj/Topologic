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

#include <list>
#include <map>
#include <memory>

namespace TopologicCore
{
	class Topology;

	class InstanceGUIDManager
	{
	public:
		typedef std::shared_ptr<InstanceGUIDManager> Ptr;

	public:
		static InstanceGUIDManager& GetInstance()
		{
			static InstanceGUIDManager instance;
			return instance;
		}

		void Add(const TopoDS_Shape& rkOcctShape, const std::string& rkGUID);

		void Remove(const TopoDS_Shape& rkOcctShape);

		bool Find(const TopoDS_Shape& rkOcctShape, std::string& rkGUID);

		void ClearAll();

	protected:
		std::map<TopoDS_Shape, std::string, OcctShapeComparator> m_occtShapeToGUIDMap;
	};
}