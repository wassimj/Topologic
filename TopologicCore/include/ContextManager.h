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

#include <TopoDS_Shape.hxx>

#include <list>
#include <map>
#include <memory>

namespace TopologicCore
{
	class Context;

	class ContextManager
	{
	public:
		typedef std::shared_ptr<ContextManager> Ptr;

	public:
		static ContextManager& GetInstance()
		{
			static ContextManager instance;
			return instance;
		}

		/// <summary>
		/// Add a Context to a destination Topology
		/// </summary>
		/// <param name="rkOcctShape">An OCCT shape</param>
		/// <param name="kpContext">A Context</param>
		void Add(const TopoDS_Shape& rkOcctShape, const std::shared_ptr<Context>& kpContext);

		/// <summary>
		/// Remove a Context from a Topology
		/// </summary>
		/// <param name="rkOcctShape">An OCCT shape</param>
		/// <param name="rkOcctContextShape">An OCCT context shape</param>
		void Remove(const TopoDS_Shape& rkOcctShape, const TopoDS_Shape& rkOcctContextShape);

		/// <summary>
		/// Remove Contexts from a Topology
		/// </summary>
		/// <param name="rkOcctShape">An OCCT shape</param>
		/// <param name="rContexts">Contexts</param>
		bool Find(const TopoDS_Shape& rkOcctShape, std::list<std::shared_ptr<Context>>& rContexts);

		/// <summary>
		/// Clear the contexts of an OCCT shape.
		/// </summary>
		/// <param name="rkOcctShape"></param>
		void ClearOne(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// Clear all contexts.
		/// </summary>
		void ClearAll();

	protected:
		/// <summary>
		/// The map which pairs an OCCT shape with a list of Contexts
		/// </summary>
		std::map<TopoDS_Shape, std::list<std::shared_ptr<Context>>, OcctShapeComparator> m_occtShapeToContextsMap;
	};
}