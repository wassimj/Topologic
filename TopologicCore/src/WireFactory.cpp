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

#include <WireFactory.h>
#include <Wire.h>

#include <TopoDS.hxx>

namespace TopologicCore
{
	std::shared_ptr<Topology> WireFactory::Create(const TopoDS_Shape& rkOcctShape)
	{
		return std::make_shared<Wire>(TopoDS::Wire(rkOcctShape));
	}
}