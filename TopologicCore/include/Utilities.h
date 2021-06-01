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

#include <TopoDS_Shape.hxx>

#ifdef _WIN32
#ifdef TOPOLOGICCORE_EXPORTS
#define TOPOLOGIC_API __declspec(dllexport)
#else
#define TOPOLOGIC_API __declspec(dllimport)
#endif
#else
#define TOPOLOGIC_API
#endif

namespace TopologicCore {
	enum TopologyType
	{
		TOPOLOGY_VERTEX = 1,
		TOPOLOGY_EDGE = 2,
		TOPOLOGY_WIRE = 4,
		TOPOLOGY_FACE = 8,
		TOPOLOGY_SHELL = 16,
		TOPOLOGY_CELL = 32,
		TOPOLOGY_CELLCOMPLEX = 64,
		TOPOLOGY_CLUSTER = 128,
		TOPOLOGY_APERTURE = 256,
		TOPOLOGY_ALL = 511
	};

	struct OcctShapeComparator {
		bool operator()(const TopoDS_Shape& rkOcctShape1, const TopoDS_Shape& rkOcctShape2) const {
			int value1 = (int)ptrdiff_t(rkOcctShape1.TShape().operator->());
			int value2 = (int)ptrdiff_t(rkOcctShape2.TShape().operator->());
			return value1 < value2;
		}
	};

	TopAbs_ShapeEnum OcctTypeFromTopologicType(const TopologyType kTopologyType);
}
