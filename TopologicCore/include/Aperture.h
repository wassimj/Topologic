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
#include "Topology.h"

#include <list>
#include <memory>

namespace TopologicCore
{
	class Context;
	class Wire;

	class ApertureGUID
	{
	public:
		static std::string Get()
		{
			return std::string("740d9d31-ca8c-47ef-825f-68c607af80aa");
		}
	};

	/// <summary>
	/// <para>
	/// An Aperture is any designed opening in a building including but not limited to any door, gate, window, skylight, or hatch.
	/// </para>
	/// </summary>
	class Aperture : public Topology
	{
	public:
		typedef std::shared_ptr<Aperture> Ptr;

	public:
		Aperture(const Topology::Ptr& kpTopology, const std::shared_ptr<Context>& kpContext, const std::string& rkGuid = "");
		~Aperture();

		/// <summary>
		/// Creates an Aperture by a topology and a context.
		/// </summary>
		/// <param name="kpTopology">A Topology</param>
		/// <param name="kpContext">A Context</param>
		/// <returns name="Aperture">An Aperture</returns>
		TOPOLOGIC_API static std::shared_ptr<Aperture> ByTopologyContext(const Topology::Ptr& kpTopology, const std::shared_ptr<Context>& kpContext);

		/// <summary>
		/// Creates an Aperture by a topology and a context topology.
		/// </summary>
		/// <param name="kpTopology">A Topology</param>
		/// <param name="kpContextTopology">Another Topology which represents the Context</param>
		/// <returns name="Aperture">An Aperture</returns>
		TOPOLOGIC_API static std::shared_ptr<Aperture> ByTopologyContext(const Topology::Ptr& kpTopology, const Topology::Ptr& kpContextTopology);
		
		/// <summary>
		/// Returns the underlying Topology.
		/// </summary>
		/// <returns name="Topology">The underlying Topology</returns>
		TOPOLOGIC_API std::shared_ptr<Topology> Topology() const;

		/// <summary>
		/// Returns the main Context.
		/// </summary>
		/// <returns name="Context">The main Context</returns>
		TOPOLOGIC_API std::shared_ptr<Context>& GetMainContext() { return m_pMainContext; }

		/// <summary>
		/// Returns the main Context.
		/// </summary>
		/// <returns name="Context">The main Context</returns>
		TOPOLOGIC_API const std::shared_ptr<Context>& GetMainContext() const { return m_pMainContext; }

		/// <summary>
		/// Creates a geometry from the underlying Topology.
		/// </summary>
		/// <param name="rOcctGeometries">The created geometries</param>
		virtual void Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const;

		/// <summary>
		/// Returns the center of mass of the underlying Topology.
		/// </summary>
		/// <returns="Vertex">The center of mass</returns>
		virtual std::shared_ptr<Vertex> CenterOfMass() const;

		/// <summary>
		/// Checks if the underlying Topology is manifold.
		/// </summary>
		/// <returns name="bool">True if the underlying Topology is manifold, otherwise False</returns>
		virtual bool IsManifold() const;

		/// <summary>
		/// Returns the type of the Aperture as a String.
		/// </summary>
		/// <returns name="String">The type of the Aperture as a String</returns>
		virtual std::string GetTypeAsString() const;

		/// <summary>
		/// Returns the underlying OCCT shape.
		/// </summary>
		/// <returns name="TopoDS_Shape">The underlying OCCT shape</returns>
		virtual TopoDS_Shape& GetOcctShape();

		/// <summary>
		/// Sets the underlying OCCT shape.
		/// </summary>
		/// <param name="rkOcctShape">A new underlying OCCT shape</param>
		virtual void SetOcctShape(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// Returns the underlying OCCT shape.
		/// </summary>
		/// <returns name="TopoDS_Shape">The underlying OCCT shape</returns>
		virtual const TopoDS_Shape& GetOcctShape() const;

		/// <summary>
		/// Returns the type of the Aperture.
		/// </summary>
		/// <returns name="TopologyType">The type of the underlying Topology</returns>
		virtual TopologyType GetType() const;

		/// <summary>
		/// Returns the class GUID.
		/// </summary>
		/// <returns name="String">The class GUID</returns>
		virtual std::string GetClassGUID() const {
			return ApertureGUID::Get();
		}

		/// <summary>
		/// Checks if the underlying Topology is a container type (Wire, Shell, CellComplex, Cluster).
		/// </summary>
		/// <returns name="bool">True if the underlying Topology is a container type (Wire, Shell, CellComplex, Cluster), otherwise False</returns>
		virtual bool IsContainerType();


	protected:
		/// <summary>
		/// The primary Context passed in the constructor
		/// </summary>
		std::shared_ptr<Context> m_pMainContext; 

		/// <summary>
		/// Fixes the input OCCT shape.
		/// </summary>
		/// <param name="rkOcctInputShape">An input OCCT shape</param>
		/// <returns name="TopoDS_Shape">The fixed OCCT shape</returns>
		virtual TopoDS_Shape OcctShapeFix(const TopoDS_Shape& rkOcctInputShape);

		/// <summary>
		/// The underlying Topology
		/// </summary>
		Topology::Ptr m_pTopology;
	};
}