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

#include <list>

namespace TopologicCore
{
	class ListAttributeGUID {
	public:
		static std::string Get()
		{
			return std::string("311a9530-6346-456c-aaaf-01856192ec6b");
		}
	};

	class ListAttribute : public Attribute
	{
	public:
		typedef std::shared_ptr<ListAttribute> Ptr;

		TOPOLOGIC_API ListAttribute(const std::list<Attribute::Ptr>& rkAttributes);
		virtual void* Value();
		//std::list<Attribute::Ptr>& ListValue() { return m_value; }
		const std::list<Attribute::Ptr>& ListValue() const { return m_value; }

	protected:
		std::list<Attribute::Ptr> m_value;
	};
}