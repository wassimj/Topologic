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
	class IntAttributeGUID {
	public:
		static std::string Get()
		{
			return std::string("1a8944c6-23e0-4607-9313-336f4ab3fd53");
		}
	};

	class IntAttribute :public Attribute
	{
	public:
		typedef std::shared_ptr<IntAttribute> Ptr;

		TOPOLOGIC_API IntAttribute(const long long int kValue);
		TOPOLOGIC_API virtual void* Value();
		TOPOLOGIC_API long long int IntValue() { return m_value; }

	protected:
		long long int m_value;
	};
}