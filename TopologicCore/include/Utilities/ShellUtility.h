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

#include <Edge.h>
#include <Face.h>
#include <Shell.h>
#include <Vertex.h>
#include <Wire.h>

#include <memory>

namespace TopologicUtilities
{
	class ShellUtility
	{
	public:
		typedef std::shared_ptr<ShellUtility> Ptr;

	public:

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkWires"></param>
		/// <returns></returns>
		static TOPOLOGIC_API TopologicCore::Shell::Ptr ByLoft(const std::list<TopologicCore::Wire::Ptr>& rkWires);
	};
}