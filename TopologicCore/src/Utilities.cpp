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

#include "Utilities.h"

namespace TopologicCore {
	TopAbs_ShapeEnum OcctTypeFromTopologicType(const TopologyType kTopologyType)
	{
		switch (kTopologyType)
		{
		case TOPOLOGY_VERTEX: return TopAbs_VERTEX;
		case TOPOLOGY_EDGE: return TopAbs_EDGE;
		case TOPOLOGY_WIRE: return TopAbs_WIRE;
		case TOPOLOGY_FACE: return TopAbs_FACE;
		case TOPOLOGY_SHELL: return TopAbs_SHELL;
		case TOPOLOGY_CELL: return TopAbs_SOLID;
		case TOPOLOGY_CELLCOMPLEX: return TopAbs_COMPSOLID;
		case TOPOLOGY_CLUSTER: return TopAbs_COMPOUND;
		default: throw std::runtime_error("No OCCT counterpart is found from this topology.");
		}
	}
}