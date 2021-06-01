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

#include "About.h"

#define MAJOR_VERSION 1
#define MINOR_VERSION 4
#define BUGFIX_VERSION 0
#define INTERNAL_BUILD_VERSION 1

namespace TopologicCore
{
	std::string About::Version()
	{
		std::string version = std::to_string(MAJOR_VERSION) + "." + std::to_string(MINOR_VERSION) + "." + std::to_string(BUGFIX_VERSION);
		if (INTERNAL_BUILD_VERSION > 0)
		{
			version = version + "." + std::to_string(INTERNAL_BUILD_VERSION);
		}
		return version;
	}
}