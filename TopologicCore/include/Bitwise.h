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

#include <list>

namespace TopologicUtilities
{
	class Bitwise
	{
	public:
		static TOPOLOGIC_API int AND(const std::list<int>& rArguments);

		static TOPOLOGIC_API int OR(const std::list<int>& rArguments);

		static TOPOLOGIC_API int XOR(const std::list<int>& rArguments);

		static TOPOLOGIC_API int NOT(const int kArgument1);

		static TOPOLOGIC_API unsigned int NOT(const unsigned int kArgument1);
	};
}