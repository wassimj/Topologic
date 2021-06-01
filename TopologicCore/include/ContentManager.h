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
	class Topology;

	/// <summary>
	/// ContentManager does not deal with ContextManager to prevent cyclic dependency.
	/// </summary>
	class ContentManager
	{
	public:
		typedef std::shared_ptr<ContentManager> Ptr;

	public:
		static ContentManager& GetInstance()
		{
			static ContentManager instance;
			return instance;
		}

		/// <summary>
		/// Add a content Topology to a destination OCCT shape
		/// </summary>
		/// <param name="rkOcctShape">A destination OCCT shape</param>
		/// <param name="kpContentTopology">A content Topology</param>
		void Add(const TopoDS_Shape& rkOcctShape, const std::shared_ptr<Topology>& kpContentTopology);

		/// <summary>
		/// Remove a content Topology from a source OCCT shape
		/// </summary>
		/// <param name="rkOcctShape">A source OCCT shape</param>
		/// <param name="rkOcctContentTopology">A content Topology</param>
		void Remove(const TopoDS_Shape& rkOcctShape, const TopoDS_Shape& rkOcctContentTopology);

		/// <summary>
		/// Returns the contents of a OCCT shape.
		/// </summary>
		/// <param name="rkOcctShape">An OCCT shape</param>
		/// <param name="rContents">The contents</param>
		/// <returns name="bool">Returns True if the Topology has contents, otherwise False</returns>
		bool Find(const TopoDS_Shape& rkOcctShape, std::list<std::shared_ptr<Topology>>& rContents);

		/// <summary>
		/// Returns True if the OCCT shape contains the content Topology, otherwise False
		/// </summary>
		/// <param name="rkOcctShape">An OCCT shape</param>
		/// <param name="rkOcctContentTopology">A content Topology</param>
		/// <returns name="bool">True if the Topology contains the content Topology, otherwise False</returns>
		bool HasContent(const TopoDS_Shape& rkOcctShape, const TopoDS_Shape& rkOcctContentTopology);

		/// <summary>
		/// Clear the contents of an OCCT shape.
		/// </summary>
		/// <param name="rkOcctShape">An OCCT shape</param>
		void ClearOne(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// Clear all contents.
		/// </summary>
		void ClearAll();

	protected:
		/// <summary>
		/// The map which pairs an OCCT shape with a list of contents
		/// </summary>
		std::map<TopoDS_Shape, std::list<std::shared_ptr<Topology>>, OcctShapeComparator> m_occtShapeToContentsMap;
	};
}