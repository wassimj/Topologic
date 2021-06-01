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

#include "Attribute.h"
#include "Utilities.h"

#include <string>

namespace TopologicCore
{
	class DoubleAttributeGUID {
	public:
		static std::string Get()
		{
			return std::string("881714ed-c3da-4aee-9c0b-36841dcb09bb");
		}
	};

	class DoubleAttribute : public Attribute
	{
	public:
		typedef std::shared_ptr<DoubleAttribute> Ptr;

		TOPOLOGIC_API DoubleAttribute(const double kValue);
		virtual void* Value();
		double DoubleValue() { return m_value; }

	protected:
		double m_value;
	};
}