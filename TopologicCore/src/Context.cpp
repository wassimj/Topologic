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

#include <Context.h>
#include <Topology.h>

namespace TopologicCore
{
	std::shared_ptr<Context> Context::ByTopologyParameters(const Topology::Ptr& kpTopology, const double kU, const double kV, const double kW)
	{
		return std::make_shared<Context>(kpTopology, kU, kV, kW);
	}

	Context::Context(const Topology::Ptr& kpTopology, const double kU, double kV, double kW)
		: m_occtShape(kpTopology->GetOcctShape())
		, m_u(kU)
		, m_v(kV)
		, m_w(kW)
	{
	}

	Context::~Context()
	{
	}
	
	std::shared_ptr<Topology> Context::Topology() const
	{
		return Topology::ByOcctShape(m_occtShape, "");
	}
}