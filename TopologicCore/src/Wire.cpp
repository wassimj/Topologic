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

#include "Wire.h"
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"
#include "WireFactory.h"
#include "GlobalCluster.h"
#include "AttributeManager.h"

#include <Utilities/VertexUtility.h>

#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Wire.hxx>
#include <BRepGProp.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom_CartesianPoint.hxx>
#include <GProp_GProps.hxx>
#include <ShapeFix_Wire.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <assert.h>
#include <algorithm>

namespace TopologicCore
{
	void Wire::Edges(std::list<Edge::Ptr>& rEdges) const
	{
		if (!IsManifold())
		{
            // Gives in any order
			DownwardNavigation(rEdges);
		}
		else
		{
            // This only works for manifold wire with a flow
			std::list<Vertex::Ptr> vertices;
			DownwardNavigation<Vertex>(vertices);
			if (vertices.empty())
			{
				return;
			}
			bool isClosed = IsClosed();
			Vertex::Ptr startingVertex = nullptr;
			if (isClosed)
			{
				// any vertex
				startingVertex = *vertices.begin();
			}else
			{
				for (const Vertex::Ptr& kpVertex : vertices)
				{
					std::list<Edge::Ptr> adjacentEdges;
					//kpVertex->Edges(adjacentEdges);
					TopologicUtilities::VertexUtility::AdjacentEdges(kpVertex, this, adjacentEdges);

					if (adjacentEdges.size() == 1)
					{
						Vertex::Ptr edgeStartVertex = (*adjacentEdges.begin())->StartVertex();

						if (edgeStartVertex->IsSame(kpVertex))
						{
							// This vertex needs to be the start vertex of this edge
							startingVertex = kpVertex;
							break;
						}
					}
				}

				if (startingVertex == nullptr)
				{
					throw std::runtime_error("This Wire is closed, but is identified as an open Wire.");
				}
			}

			// Get an adjacent edge
			Vertex::Ptr currentVertex = startingVertex;
			Edge::Ptr previousEdge = nullptr;
			do {
				std::list<Edge::Ptr> adjacentEdges;
				TopologicUtilities::VertexUtility::AdjacentEdges(currentVertex, this, adjacentEdges);

				Edge::Ptr currentEdge = nullptr;
				for (const Edge::Ptr& kpAdjacentEdge : adjacentEdges)
				{
					if (previousEdge == nullptr)
					{
						if (kpAdjacentEdge->StartVertex()->IsSame(currentVertex))
						{
							currentEdge = kpAdjacentEdge;
						}
					}else if(!previousEdge->IsSame(kpAdjacentEdge))
					{
						currentEdge = kpAdjacentEdge;
					}
				}

				// Still null? break. This happens in the open wire when the last Vertex is visited.
				if (currentEdge == nullptr)
				{
					break;
				}
				
				rEdges.push_back(currentEdge);
				previousEdge = currentEdge;

				// Get the other vertex
				std::list<Vertex::Ptr> vertices;
				currentEdge->Vertices(vertices);

				for (const Vertex::Ptr& kpVertex : vertices)
				{
					if (kpVertex->IsSame(currentVertex))
					{
						continue;
					}

					currentVertex = kpVertex;
					break;
				}
				
				// If the starting vertex is revisited, break. This happens in the open wire.
				if (currentVertex->IsSame(startingVertex))
				{
					break;
				}
			} while (true);
		}
	}

	void Wire::Faces(std::list<Face::Ptr>& rFaces) const
	{
		UpwardNavigation(rFaces);
	}

	bool Wire::IsClosed() const
	{
		BRepCheck_Wire occtCheckWire(TopoDS::Wire(GetOcctShape()));
		BRepCheck_Status status = occtCheckWire.Closed();
		bool isClosed = status == BRepCheck_NoError;
		return isClosed;
	}

	void Wire::Vertices(std::list<Vertex::Ptr>& rVertices) const
	{
		TopTools_MapOfShape occtVertices;
		std::list<Edge::Ptr> edges;
		Edges(edges);

		for (const Edge::Ptr kpEdge : edges)
		{
			std::list<Vertex::Ptr> vertices;
			kpEdge->Vertices(vertices);

			// Special case when handling the second edge
			if (rVertices.size() == 2)
			{
				auto firstVertexIterator = rVertices.begin();

				for (const Vertex::Ptr kpVertex : vertices)
				{
					if (kpVertex->IsSame(*firstVertexIterator))
					{
						Vertex::Ptr firstVertex = *firstVertexIterator;
						rVertices.erase(firstVertexIterator);
						rVertices.push_back(firstVertex);
						break;
					}
				}
			}

			for (const Vertex::Ptr kpVertex : vertices)
			{
				if (!occtVertices.Contains(kpVertex->GetOcctShape()))
				{
					occtVertices.Add(kpVertex->GetOcctShape());
					rVertices.push_back(kpVertex);
				}
			}
		}
	}

	// This method may involve making copies of the edges if they originally do not share vertices.
	Wire::Ptr Wire::ByEdges(const std::list<Edge::Ptr>& rkEdges)
	{
		if (rkEdges.empty())
		{
			return nullptr;
		}

		TopTools_ListOfShape occtEdges;
		for(const Edge::Ptr& kpEdge : rkEdges)
		{
			occtEdges.Append(kpEdge->GetOcctShape());
		}

		TopoDS_Wire occtWire = ByOcctEdges(occtEdges);
		Wire::Ptr pWire = std::make_shared<Wire>(occtWire);
		Wire::Ptr pCopyWire = std::dynamic_pointer_cast<Wire>(pWire->DeepCopy());
		for (const Edge::Ptr& kpEdge : rkEdges)
		{
			AttributeManager::GetInstance().DeepCopyAttributes(kpEdge->GetOcctEdge(), pCopyWire->GetOcctWire());
		}

		GlobalCluster::GetInstance().AddTopology(pCopyWire->GetOcctWire());
		return pCopyWire;
	}

	TopoDS_Wire Wire::ByOcctEdges(const TopTools_ListOfShape & rkOcctEdges)
	{
		BRepBuilderAPI_MakeWire occtMakeWire;
		occtMakeWire.Add(rkOcctEdges);

		try {
			TransferMakeShapeContents(occtMakeWire, rkOcctEdges);
			TopoDS_Wire occtWire = TopoDS::Wire(Topology::FixShape(occtMakeWire));
			return occtMakeWire;
		}
		catch (StdFail_NotDone&)
		{
			Throw(occtMakeWire);
			return TopoDS_Wire();
		}
	}

	bool Wire::IsManifold() const
	{
		int numberOfBranches = NumberOfBranches();
		if (numberOfBranches == 0)
		{
			return true;
		}

		return false;
	}

	int Wire::NumberOfBranches() const
	{
		std::list<Vertex::Ptr> vertices;
		DownwardNavigation<Vertex>(vertices);

		int numOfBranches = 0;
		for (const Vertex::Ptr& kpVertex: vertices)
		{
			std::list<Edge::Ptr> edges;
			kpVertex->UpwardNavigation<Edge>(GetOcctWire(), edges);
			if (edges.size() > 2)
			{
				numOfBranches++;
			}
		}

		return numOfBranches;
	}

	void Wire::Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const
	{
		// Returns a list of curves
		std::list<Edge::Ptr> edges;
		Edges(edges);

		for (const Edge::Ptr& kpEdge : edges)
		{
			rOcctGeometries.push_back(kpEdge->Curve());
		}
	}

	TopoDS_Shape& Wire::GetOcctShape()
	{
		return GetOcctWire();
	}

	const TopoDS_Shape& Wire::GetOcctShape() const
	{
		return GetOcctWire();
	}

	void Wire::SetOcctShape(const TopoDS_Shape & rkOcctShape)
	{
		try {
			SetOcctWire(TopoDS::Wire(rkOcctShape));
		}
		catch (Standard_Failure e)
		{
			throw std::runtime_error(e.GetMessageString());
		}
	}

	void Wire::SetOcctWire(const TopoDS_Wire & rkOcctWire)
	{
		m_occtWire = rkOcctWire;
	}

	TopoDS_Wire& Wire::GetOcctWire()
	{
		assert(!m_occtWire.IsNull() && "Wire::m_occtWire is null.");
		if (m_occtWire.IsNull())
		{
			throw std::runtime_error("A null Wire is encountered.");
		}

		return m_occtWire;
	}

	const TopoDS_Wire& Wire::GetOcctWire() const
	{
		assert(!m_occtWire.IsNull()  && "Wire::m_occtWire is null.");
		if (m_occtWire.IsNull())
		{
			throw std::runtime_error("A null Wire is encountered.");
		}

		return m_occtWire;
	}

	void Wire::Throw(const BRepBuilderAPI_MakeWire & rkOcctMakeWire)
	{
		// The error messages are based on those in the OCCT documentation.
		// https://www.opencascade.com/doc/occt-7.2.0/refman/html/_b_rep_builder_a_p_i___wire_error_8hxx.html

		switch (rkOcctMakeWire.Error())
		{
		case BRepBuilderAPI_EmptyWire:
			throw std::runtime_error("No initialization of the algorithm. Only an empty constructor was used.");

		case BRepBuilderAPI_DisconnectedWire:
			throw std::runtime_error("The last edge which you attempted to add was not connected to the wire.");

		case BRepBuilderAPI_NonManifoldWire:
			throw std::runtime_error("The wire has some singularity.");

		//default: // i.e. BRepBuilderAPI_WireDone, do nothing
		}
	}

	std::shared_ptr<Vertex> Wire::CenterOfMass() const
	{
		TopoDS_Vertex occtCenterOfMass = CenterOfMass(GetOcctWire());
		return std::dynamic_pointer_cast<Vertex>(Topology::ByOcctShape(occtCenterOfMass));
	}

	TopoDS_Vertex Wire::CenterOfMass(const TopoDS_Wire & rkOcctWire)
	{
		GProp_GProps occtShapeProperties;
		BRepGProp::LinearProperties(rkOcctWire, occtShapeProperties);
		TopoDS_Vertex occtCenterOfMass = BRepBuilderAPI_MakeVertex(occtShapeProperties.CentreOfMass());
		TopoDS_Vertex occtFixedCenterOfMass = TopoDS::Vertex(Topology::FixShape(occtCenterOfMass));
		return occtFixedCenterOfMass;
	}

	std::string Wire::GetTypeAsString() const
	{
		return std::string("Wire");
	}

	Wire::Wire(const TopoDS_Wire& rkOcctWire, const std::string& rkGuid)
		: Topology(1, rkOcctWire, rkGuid.compare("") == 0 ? GetClassGUID() : rkGuid)
		, m_occtWire(rkOcctWire)
	{
		RegisterFactory(GetClassGUID(), std::make_shared<WireFactory>());
	}

	Wire::~Wire()
	{

	}
}
