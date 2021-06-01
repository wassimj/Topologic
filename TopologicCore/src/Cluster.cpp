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

#include "Cluster.h"
#include "CellComplex.h"
#include "Cell.h"
#include "Vertex.h"
#include "Edge.h"
#include "Wire.h"
#include "Face.h"
#include "Shell.h"
#include "ClusterFactory.h"
#include "GlobalCluster.h"
#include "AttributeManager.h"

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <Geom_CartesianPoint.hxx>
#include <TopoDS.hxx>
#include <TopoDS_FrozenShape.hxx>
#include <TopoDS_UnCompatibleShapes.hxx>

#include <assert.h>

namespace TopologicCore
{
	Cluster::Ptr Cluster::ByTopologies(const std::list<Topology::Ptr>& rkTopologies)
	{
		if (rkTopologies.empty())
		{
			return nullptr;
		}

		TopoDS_Compound occtCompound;
		TopoDS_Builder occtBuilder;
		occtBuilder.MakeCompound(occtCompound);
		Cluster::Ptr pCluster = std::make_shared<Cluster>(occtCompound);
		for(const Topology::Ptr& kpTopology : rkTopologies)
		{
			pCluster->AddTopology(kpTopology.get());
		}

		// Deep copy
		Cluster::Ptr pCopyCluster = std::dynamic_pointer_cast<Cluster>(pCluster->DeepCopy());

		// Transfer the attributes
		/*for (const Topology::Ptr& kpTopology : rkTopologies)
		{
			AttributeManager::GetInstance().DeepCopyAttributes(kpTopology->GetOcctShape(), pCopyCluster->GetOcctCompound());
		}*/
		pCopyCluster->DeepCopyAttributesFrom(rkTopologies);

		// Add to the Global Cluster
		GlobalCluster::GetInstance().AddTopology(pCopyCluster->GetOcctCompound());
		return pCopyCluster;
	}

	TopoDS_Compound Cluster::ByOcctTopologies(const TopTools_MapOfShape & rkOcctShapes)
	{
		TopoDS_Compound occtCompound;
		TopoDS_Builder occtBuilder;
		occtBuilder.MakeCompound(occtCompound);
		for (TopTools_MapIteratorOfMapOfShape occtShapeIterator(rkOcctShapes);
			occtShapeIterator.More();
			occtShapeIterator.Next())
		{
			try {
				occtBuilder.Add(occtCompound, occtShapeIterator.Value());
			}
			catch (TopoDS_UnCompatibleShapes &)
			{
				throw std::runtime_error("Error making a OCCT compound.");
			}
			catch (TopoDS_FrozenShape &)
			{
				throw std::runtime_error("Error making a OCCT compound.");
			}
		}
		return occtCompound;
	}

	// Use raw pointer because 1) called from the individual object, 2) does not need a smart pointer
	bool Cluster::AddTopology(Topology const * const kpkTopology)
	{
		// Do the addition
		bool returnValue = true;
		try {
			m_occtBuilder.Add(GetOcctShape(), kpkTopology->GetOcctShape());
		}
		catch (TopoDS_UnCompatibleShapes &)
		{
			returnValue = false;
		}
		catch (TopoDS_FrozenShape &)
		{
			returnValue = false;
		}

		return returnValue;
	}

	bool Cluster::RemoveTopology(Topology const * const kpkTopology)
	{
		try {
			m_occtBuilder.Remove(GetOcctShape(), kpkTopology->GetOcctShape());

			return true;
		}
		catch (TopoDS_UnCompatibleShapes &)
		{
			return false;
		}
		catch (TopoDS_FrozenShape &)
		{
			return false;
		}
	}

	TopoDS_Shape& Cluster::GetOcctShape()
	{
		return GetOcctCompound();
	}

	const TopoDS_Shape& Cluster::GetOcctShape() const
	{
		return GetOcctCompound();
	}

	TopoDS_Compound& Cluster::GetOcctCompound()
	{
		assert(!m_occtCompound.IsNull() && "Cluster::m_occtCompound is null.");
		if (m_occtCompound.IsNull())
		{
			throw std::runtime_error("A null Cluster is encountered.");
		}

		return m_occtCompound;
	}

	const TopoDS_Compound& Cluster::GetOcctCompound() const
	{
		assert(!m_occtCompound.IsNull() && "Cluster::m_occtCompound is null.");
		if (m_occtCompound.IsNull())
		{
			throw std::runtime_error("A null Cluster is encountered.");
		}

		return m_occtCompound;
	}

	void Cluster::SetOcctShape(const TopoDS_Shape & rkOcctShape)
	{
		SetOcctShape(TopoDS::Compound(rkOcctShape));
	}

	void Cluster::SetOcctCompound(const TopoDS_Compound & rkOcctCompound)
	{
		m_occtCompound = rkOcctCompound;
	}

	void Cluster::Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const
	{
		throw std::runtime_error("No implementation for Cluster entity");
	}

	Cluster::Cluster(const TopoDS_Compound& rkOcctCompound, const std::string& rkGuid)
		: Topology(3, rkOcctCompound, rkGuid.compare("") == 0 ? GetClassGUID() : rkGuid)
		, m_occtCompound(rkOcctCompound)
	{
		// This constructor does not initialise the compound with MakeCompound.
		RegisterFactory(GetClassGUID(), std::make_shared<ClusterFactory>());
	}

	Cluster::~Cluster()
	{

	}

	void Cluster::Shells(std::list<Shell::Ptr>& rShells) const
	{
		DownwardNavigation(rShells);
	}

	void Cluster::Edges(std::list<Edge::Ptr>& rEdges) const
	{
		DownwardNavigation(rEdges);
	}

	void Cluster::Faces(std::list<Face::Ptr>& rFaces) const
	{
		DownwardNavigation(rFaces);
	}

	void Cluster::Vertices(std::list<Vertex::Ptr>& rVertices) const
	{
		DownwardNavigation(rVertices);
	}

	void Cluster::Wires(std::list<Wire::Ptr>& rWires) const
	{
		DownwardNavigation(rWires);
	}

	void Cluster::Cells(std::list<Cell::Ptr>& rCells) const
	{
		DownwardNavigation(rCells);
	}

	void Cluster::CellComplexes(std::list<CellComplex::Ptr>& rCellComplexes) const
	{
		DownwardNavigation(rCellComplexes);
	}

	bool Cluster::IsInside(Topology const * const kpkTopology) const
	{
		const TopoDS_Shape& rkOcctAddedShape = kpkTopology->GetOcctShape();
		TopTools_MapOfShape occtShapes;
		for (TopExp_Explorer occtExplorer(GetOcctShape(), rkOcctAddedShape.ShapeType()); occtExplorer.More(); occtExplorer.Next())
		{
			const TopoDS_Shape& rkOcctCurrent = occtExplorer.Current();
			if (rkOcctAddedShape.IsSame(rkOcctCurrent))
			{
				return true;
			}
		}

		return false;
	}

	Vertex::Ptr Cluster::CenterOfMass() const
	{
		TopoDS_Vertex occtCenterOfMass = CenterOfMass(GetOcctCompound());
		return std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtCenterOfMass));
	}

	TopoDS_Vertex Cluster::CenterOfMass(const TopoDS_Compound & rkOcctCompound)
	{
		// Compute the average of the centers of mass.
		TopTools_ListOfShape occtSubtopologies;
		SubTopologies(rkOcctCompound, occtSubtopologies);
		if (occtSubtopologies.IsEmpty())
		{
			throw std::runtime_error("The input Cluster is empty.");
		}

		double size = (double)occtSubtopologies.Size();
		gp_Pnt occtCentroidSum;
		for (TopTools_ListIteratorOfListOfShape occtIterator(occtSubtopologies);
			occtIterator.More();
			occtIterator.Next())
		{
			Topology::Ptr subtopology = Topology::ByOcctShape(occtIterator.Value(), "");
			Vertex::Ptr subtopologyCenterOfMass = subtopology->CenterOfMass();
			gp_Pnt occtSubtopologyCenterOfMass = subtopologyCenterOfMass->Point()->Pnt();
			occtCentroidSum.SetX(occtCentroidSum.X() + occtSubtopologyCenterOfMass.X());
			occtCentroidSum.SetY(occtCentroidSum.Y() + occtSubtopologyCenterOfMass.Y());
			occtCentroidSum.SetZ(occtCentroidSum.Z() + occtSubtopologyCenterOfMass.Z());
		}

		gp_Pnt occtCentroid(
			occtCentroidSum.X() / size,
			occtCentroidSum.Y() / size,
			occtCentroidSum.Z() / size
		);

		return BRepBuilderAPI_MakeVertex(occtCentroid);
	}

	bool Cluster::IsManifold() const
	{
		throw std::runtime_error("Not implemented yet");
	}

	std::string Cluster::GetTypeAsString() const
	{
		return std::string("Cluster");
	}
}