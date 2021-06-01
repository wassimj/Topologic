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
#include "GlobalCluster.h"
#include "TopologicalQuery.h"
#include "Dictionary.h"

#include <TopTools_ListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <BOPAlgo_CellsBuilder.hxx>
#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <Standard_Handle.hxx>
#include <Geom_Geometry.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Shell.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#include <limits>
#include <list>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

class TopoDS_Shape;

namespace TopologicCore
{
	class Attribute;
	class Cluster;
	class CellComplex;
	class Cell;
	class Shell;
	class Face;
	class Vertex;
	class Edge;
	class Wire;
	class Context;
	class Aperture;
	class TopologyFactory;

	/// <summary>
	/// A Topology is an abstract superclass that constructors, properties and methods used by other subclasses that extend it.
	/// </summary>
	class Topology : public TopologicalQuery, public std::enable_shared_from_this<Topology>
	{
	public:
		typedef std::shared_ptr<Topology> Ptr;

	public:
		virtual ~Topology();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		/// <param name="rkInstanceGuid"></param>
		/// <returns></returns>
		static TOPOLOGIC_API Topology::Ptr ByOcctShape(const TopoDS_Shape& rkOcctShape, const std::string& rkInstanceGuid = "");

		/// <summary>
		/// 
		/// </summary>
		/// <param name="pGeometry"></param>
		/// <returns></returns>
		static Topology::Ptr ByGeometry(Handle(Geom_Geometry) pGeometry);

		/// <summary>
		/// 
		///		</summary>
		/// <param name="kpContext"></param>
		/// <returns></returns>
		static Topology::Ptr ByContext(const std::shared_ptr<Context>& kpContext);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkVertices"></param>
		/// <param name="rkVertexIndices"></param>
		/// <param name="rTopologies"></param>
		/// <returns></returns>
		static TOPOLOGIC_API void ByVertexIndex(const std::vector<std::shared_ptr<Vertex>>& rkVertices, const std::list<std::list<int>>& rkVertexIndices, std::list<Topology::Ptr>& rTopologies);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkFaces"></param>
		/// <param name="kTolerance"></param>
		/// <returns></returns>
		static TOPOLOGIC_API std::shared_ptr<Topology> ByFaces(const std::list<std::shared_ptr<Face>>& rkFaces, const double kTolerance = 0.001);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkTopology"></param>
		/// <returns></returns>
		void AddContent(const Topology::Ptr& rkTopology);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkTopology"></param>
		/// <param name="kTypeFilter"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr  AddContent(const Topology::Ptr& rkTopology, const int kTypeFilter);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkContentTopologies"></param>
		/// <param name="kTypeFilter"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr AddContents(const std::list<Topology::Ptr>& rkContentTopologies, const int kTypeFilter);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkTopology"></param>
		/// <returns></returns>
		void RemoveContent(const Topology::Ptr& rkTopology);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkTopologies"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr RemoveContents(const std::list<Topology::Ptr>& rkTopologies);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkContext"></param>
		/// <returns></returns>
		void AddContext(const std::shared_ptr<Context>& rkContext);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkContexts"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr AddContexts(const std::list<std::shared_ptr<Context>>& rkContexts);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkContext"></param>
		/// <returns></returns>
		void RemoveContext(const std::shared_ptr<Context>& rkContext);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkContexts"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr RemoveContexts(const std::list<std::shared_ptr<Context>>& rkContexts);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpTopology"></param>
		/// <param name="kFilterType"></param>
		/// <param name="rSharedTopologies"></param>
		TOPOLOGIC_API void SharedTopologies(const Topology::Ptr& kpTopology, const int kFilterType, std::list<Topology::Ptr>& rSharedTopologies) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkSelectors"></param>
		/// <param name="rkDictionaries"></param>
		/// <param name="kTypeFilter"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr SetDictionaries(
			const std::list<std::shared_ptr<Vertex>>& rkSelectors, 
			const std::list<std::map<std::string, std::shared_ptr<Attribute>>>& rkDictionaries,
			const int kTypeFilter = TOPOLOGY_ALL);

        Topology::Ptr SetDictionaries(
            const std::list<std::shared_ptr<Vertex>>& rkSelectors,
            const std::list<std::map<std::string, std::shared_ptr<Attribute>>>& rkDictionaries,
            const std::list<int>& rkTypeFilters,
            const bool expectDuplicateTopologies = false);

		TOPOLOGIC_API Topology::Ptr SetDictionaries(
			const std::list<std::shared_ptr<Vertex>>& rkSelectors,
			const std::list<Dictionary>& rkDictionaries,
			const int kTypeFilter = TOPOLOGY_ALL);

		Topology::Ptr SetDictionaries(
			const std::list<std::shared_ptr<Vertex>>& rkSelectors,
			const std::list<Dictionary>& rkDictionaries,
			const std::list<int>& rkTypeFilters,
			const bool expectDuplicateTopologies = false);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctFaces"></param>
		/// <param name="kTolerance"></param>
		/// <returns></returns>
		static TopoDS_Shape OcctSewFaces(const TopTools_ListOfShape& rkOcctFaces, const double kTolerance = 0.001);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpOtherTopology"></param>
		/// <param name="kTransferDictionary"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr Difference(const Topology::Ptr& kpOtherTopology = nullptr, const bool kTransferDictionary = false);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpTool"></param>
		/// <param name="kTransferDictionary"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr Impose(const Topology::Ptr& kpTool = nullptr, const bool kTransferDictionary = false);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpTool"></param>
		/// <param name="kTransferDictionary"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr Imprint(const Topology::Ptr& kpTool = nullptr, const bool kTransferDictionary = false);
		
		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpOtherTopology"></param>
		/// <param name="kTransferDictionary"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr Intersect(const Topology::Ptr& kpOtherTopology = nullptr, const bool kTransferDictionary = false);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpOtherTopology"></param>
		/// <param name="kTransferDictionary"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr Merge(const Topology::Ptr& kpOtherTopology = nullptr, const bool kTransferDictionary = false);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr SelfMerge();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpTool"></param>
		/// <param name="kTransferDictionary"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr Slice(const Topology::Ptr& kpTool = nullptr, const bool kTransferDictionary = false);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpOtherTopology"></param>
		/// <param name="kTransferDictionary"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr Union(const Topology::Ptr& kpOtherTopology = nullptr, const bool kTransferDictionary = false);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpOtherTopology"></param>
		/// <param name="kTransferDictionary"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr XOR(const Topology::Ptr& kpOtherTopology = nullptr, const bool kTransferDictionary = false);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpTool"></param>
		/// <param name="kTransferDictionary"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr Divide(const Topology::Ptr& kpTool = nullptr, const bool kTransferDictionary = false);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape1"></param>
		/// <param name="kpTopology2"></param>
		TOPOLOGIC_API static void TransferContents(const TopoDS_Shape& rkOcctShape1, const Topology::Ptr& kpTopology2);

		static void TransferContents(const TopoDS_Shape& rkOcctShape1, const Topology::Ptr& kpTopology2, const TopTools_ListOfShape& rkOcctDeletedSubshapes);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rOcctGeometries"></param>
		virtual void Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const = 0;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rContents"></param>
		TOPOLOGIC_API void Contents(std::list<Topology::Ptr>& rContents) const;

		static void Contents(const TopoDS_Shape& rkOcctShape, std::list<Topology::Ptr>& rContents);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rApertures"></param>
		TOPOLOGIC_API void Apertures(std::list<std::shared_ptr<Aperture>>& rApertures) const;

		static void Apertures(const TopoDS_Shape& rkOcctShape, std::list<std::shared_ptr<Aperture>>& rApertures);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rSubContents"></param>
		TOPOLOGIC_API void SubContents(std::list<Topology::Ptr>& rSubContents) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		/// <param name="rSubContents"></param>
		static TOPOLOGIC_API void SubContents(const TopoDS_Shape& rkOcctShape, std::list<Topology::Ptr>& rSubContents);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API bool Contexts(std::list<std::shared_ptr<Context>>& rContexts) const;

		static bool Contexts(const TopoDS_Shape& rkOcctShape, std::list<std::shared_ptr<Context>>& rContexts);

		/// <summary>
		/// Returns True if this Topology is manifold, otherwise a False.
		/// </summary>
		/// <returns name="bool">True if this Topology is manifold, otherwise a False.</returns>
		TOPOLOGIC_API virtual bool IsManifold() const = 0;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkFilePath"></param>
		/// <returns></returns>
		TOPOLOGIC_API bool ExportToBRep(const std::string& rkFilePath) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkFilePath"></param>
		/// <returns></returns>
		TOPOLOGIC_API static Topology::Ptr ByImportedBRep(const std::string& rkFilePath);

		TOPOLOGIC_API static Topology::Ptr ByString(const std::string& rkBrepString);

		TOPOLOGIC_API std::string String() const;

		static TOPOLOGIC_API void Filter(const std::list<Topology::Ptr>& rkTopologies, const int kTypeFilter, std::list<Topology::Ptr>& rFilteredTopologies);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rOcctShape"></param>
		/// <returns></returns>
		static TopoDS_Shape Simplify(TopoDS_Shape& rOcctShape);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rOcctShape"></param>
		/// <returns></returns>
		static TopoDS_Shape BooleanSubTopologyContainment(TopoDS_Shape& rOcctShape);

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API std::string Analyze();

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		int Dimensionality() const
		{
			return m_dimensionality;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual TopoDS_Shape& GetOcctShape() = 0;

		virtual void SetOcctShape(const TopoDS_Shape& rkOcctShape) = 0;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual const TopoDS_Shape& GetOcctShape() const = 0;

		virtual TopologyType GetType() const = 0;

		/// <summary>
		/// Returns all sub-entities that have no other parents than this topology, i.e. do not belong to other entities.
		/// </summary>
		/// <param name="rSubTopologies">The sub-topologies</param>
		TOPOLOGIC_API void SubTopologies(std::list<Topology::Ptr>& rSubTopologies) const;

		int NumOfSubTopologies() const;

		/// <summary>
		/// Gets the type of this Topology as a String.
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API virtual std::string GetTypeAsString() const = 0;


		/// <summary>
		/// 
		/// </summary>
		/// <param name="rShells"></param>
		TOPOLOGIC_API void Shells(std::list<std::shared_ptr<Shell>>& rShells) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rEdges"></param>
		TOPOLOGIC_API void Edges(std::list<std::shared_ptr<Edge>>& rEdges) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rFaces"></param>
		TOPOLOGIC_API void Faces(std::list<std::shared_ptr<Face>>& rFaces) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rVertices"></param>
		TOPOLOGIC_API virtual void Vertices(std::list<std::shared_ptr<Vertex>>& rVertices) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rWires"></param>
		TOPOLOGIC_API void Wires(std::list<std::shared_ptr<Wire>>& rWires) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rCells"></param>
		TOPOLOGIC_API void Cells(std::list<std::shared_ptr<Cell>>& rCells) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rCellComplexes"></param>
		TOPOLOGIC_API void CellComplexes(std::list<std::shared_ptr<CellComplex>>& rCellComplexes) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kOcctShapeType"></param>
		/// <param name="rMembers"></param>
		template <class Subclass>
		void Navigate(std::list<std::shared_ptr<Subclass>>& rMembers) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rAncestors"></param>
		template <class Subclass>
		void UpwardNavigation(std::list<std::shared_ptr<Subclass>>& rAncestors) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctHostTopology"></param>
		/// <param name="rAncestors"></param>
		template <class Subclass>
		void UpwardNavigation(const TopoDS_Shape& rkOcctHostTopology, std::list<std::shared_ptr<Subclass>>& rAncestors) const;

		TOPOLOGIC_API void UpwardNavigation(const TopoDS_Shape& rkOcctHostTopology, const int kTopologyType, std::list<std::shared_ptr<Topology>>& rAncestors) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rMembers"></param>
		template <class Subclass>
		void DownwardNavigation(std::list<std::shared_ptr<Subclass>>& rMembers) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		/// <param name="rkShapeEnum"></param>
		/// <param name="rOcctMembers"></param>
		static void DownwardNavigation(const TopoDS_Shape& rkOcctShape, const TopAbs_ShapeEnum& rkShapeEnum, TopTools_MapOfShape& rOcctMembers);

		/// <summary>
		/// Copy the whole content/context hierarchy.
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr DeepCopy();

		/// <summary>
		/// Copy the whole content/context hierarchy.
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr ShallowCopy();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		/// <returns></returns>
		static TopoDS_Shape CopyOcct(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOriginalSubshape"></param>
		/// <param name="rkNewSubshape"></param>
		/// <returns></returns>
		TOPOLOGIC_API void ReplaceSubentity(const Topology::Ptr& rkOriginalSubshape, const Topology::Ptr& rkNewSubshape);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctOriginalSubshape"></param>
		/// <param name="rkOcctNewSubshape"></param>
		void ReplaceSubentity(const TopoDS_Shape& rkOcctOriginalSubshape, const TopoDS_Shape& rkOcctNewSubshape);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpTopology"></param>
		/// <returns></returns>
		TOPOLOGIC_API bool IsSame(const Topology::Ptr& kpTopology) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rOcctMembers"></param>
		void Members(TopTools_ListOfShape& rOcctMembers) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rMembers"></param>
		void Members(std::list<Topology::Ptr>& rMembers) const;

		static void Members(const TopoDS_Shape& rkOcctShape, TopTools_ListOfShape& rOcctMembers);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpTopology"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr ClosestSimplestSubshape(const Topology::Ptr& kpTopology) const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpSelector"></param>
		/// <param name="kTypeFilter"></param>
		/// <returns></returns>
		TOPOLOGIC_API Topology::Ptr SelectSubtopology(const std::shared_ptr<Vertex>& kpSelector, const int kTypeFilter = TOPOLOGY_ALL) const;

		static TopoDS_Shape SelectSubtopology(
			const TopoDS_Shape& rkOcctShape, 
			const TopoDS_Shape& rkOcctSelectorShape, 
			const int kTypeFilter = TOPOLOGY_ALL, 
			const double kDistanceThreshold = std::numeric_limits<double>::max());

		static TopoDS_Shape SelectSubtopology(
			const TopoDS_Shape& rkOcctShape,
			const TopoDS_Shape& rkOcctSelectorShape,
			double& rMinDistance,
			const int kTypeFilter = TOPOLOGY_ALL,
			const double kDistanceThreshold = std::numeric_limits<double>::max());

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API virtual std::shared_ptr<Vertex> CenterOfMass() const = 0;

		static TopoDS_Vertex CenterOfMass(const TopoDS_Shape& rkOcctShape);

		TOPOLOGIC_API std::shared_ptr<Vertex> Centroid() const;

		/// <summary>
		/// <para>
		/// Identifies the class type by GUID. Used in the factory class system since GUID is easily extendable.
		/// WARNING: Do not use this to identify the instance!
		/// </para>
		/// </summary>
		/// <returns></returns>
		TOPOLOGIC_API virtual std::string GetClassGUID() const = 0;

		/// <summary>
		/// Identifies the instance type by GUID. Used in the factory class system since GUID is easily extendable.
		/// </summary>
		/// <returns>The GUID</returns>
		TOPOLOGIC_API const std::string GetInstanceGUID() const;

		static const std::string GetInstanceGUID(const TopoDS_Shape& rkOcctShape);

		void SetInstanceGUID(const TopoDS_Shape& rkOcctShape, const std::string& rkGuid);

		static TopologyType GetTopologyType(const TopAbs_ShapeEnum& rkOcctType);

		static TopAbs_ShapeEnum GetOcctTopologyType(const TopologyType& rkType);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkShape"></param>
		/// <param name="rSubTopologies"></param>
		static void SubTopologies(const TopoDS_Shape& rkShape, TopTools_ListOfShape& rSubTopologies);

		virtual bool IsContainerType() = 0;

		static bool IsContainerType(const TopoDS_Shape& rkOcctShape);

#ifdef _DEBUG
		TOPOLOGIC_API void GlobalClusterSubTopologies(std::list<Topology::Ptr>& rSubTopologies) const;
#endif

		TOPOLOGIC_API void RegisterFactory(const std::string& rkGuid, const std::shared_ptr<TopologyFactory>& kpTopologyFactory);

		TOPOLOGIC_API bool IsReversed();

		Topology::Ptr DeepCopyAttributesFrom(const std::list<Topology::Ptr>& kpOriginTopologies);

		TOPOLOGIC_API void SetDictionary(const Dictionary& dictionary);

		TOPOLOGIC_API Dictionary GetDictionary();

	protected:
		Topology(const int kDimensionality, const TopoDS_Shape& rkOcctShape, const std::string& rkGuid = "");
        void AddUnionInternalStructure(const TopoDS_Shape& rkOcctShape, TopTools_ListOfShape& rUnionArguments);

		static TopoDS_Shape FixShape(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		/// <returns></returns>
		static TopoDS_CompSolid MakeCompSolid(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkShape"></param>
		/// <param name="kLevel"></param>
		/// <returns></returns>
		static std::string Analyze(const TopoDS_Shape& rkShape, const int kLevel = 0);

		template <class Subclass>
		static TopAbs_ShapeEnum CheckOcctShapeType();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpOtherTopology"></param>
		/// <param name="rOcctCellsBuilder"></param>
		/// <param name="rOcctCellsBuildersOperandsA"></param>
		/// <param name="rOcctCellsBuildersOperandsB"></param>
		/// <param name="rOcctMapFaceToFixedFaceA"></param>
		/// <param name="rOcctMapFaceToFixedFaceB"></param>
		void AddBooleanOperands(
			const Topology::Ptr& kpOtherTopology,
			BOPAlgo_CellsBuilder& rOcctCellsBuilder,
			TopTools_ListOfShape& rOcctCellsBuildersOperandsA,
			TopTools_ListOfShape& rOcctCellsBuildersOperandsB,
			TopTools_DataMapOfShapeShape& rOcctMapFaceToFixedFaceA,
			TopTools_DataMapOfShapeShape& rOcctMapFaceToFixedFaceB);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpOtherTopology"></param>
		/// <param name="rOcctCellsBuildersOperandsA"></param>
		/// <param name="rOcctCellsBuildersOperandsB"></param>
		void AddBooleanOperands(
			const Topology::Ptr& kpOtherTopology,
			TopTools_ListOfShape& rOcctCellsBuildersOperandsA,
			TopTools_ListOfShape& rOcctCellsBuildersOperandsB);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kpOtherTopology"></param>
		/// <param name="rOcctCellsBuilder"></param>
		/// <param name="rOcctCellsBuildersOperandsA"></param>
		/// <param name="rOcctCellsBuildersOperandsB"></param>
		/// <param name="rOcctMapFaceToFixedFaceA"></param>
		/// <param name="rOcctMapFaceToFixedFaceB"></param>
		void NonRegularBooleanOperation(
			const Topology::Ptr& kpOtherTopology,
			BOPAlgo_CellsBuilder& rOcctCellsBuilder,
			TopTools_ListOfShape& rOcctCellsBuildersOperandsA,
			TopTools_ListOfShape& rOcctCellsBuildersOperandsB,
			TopTools_DataMapOfShapeShape& rOcctMapFaceToFixedFaceA,
			TopTools_DataMapOfShapeShape& rOcctMapFaceToFixedFaceB);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctArgumentsA"></param>
		/// <param name="rkOcctArgumentsB"></param>
		/// <param name="rOcctCellsBuilder"></param>
		static void NonRegularBooleanOperation(
			const TopTools_ListOfShape& rkOcctArgumentsA,
			const TopTools_ListOfShape& rkOcctArgumentsB,
			BOPAlgo_CellsBuilder& rOcctCellsBuilder);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctArgumentsA"></param>
		/// <param name="rkOcctArgumentsB"></param>
		/// <param name="rOcctBooleanOperation"></param>
		static void RegularBooleanOperation(
			const TopTools_ListOfShape& rkOcctArgumentsA,
			const TopTools_ListOfShape& rkOcctArgumentsB,
			BRepAlgoAPI_BooleanOperation& rOcctBooleanOperation);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rOcctBooleanResult"></param>
		/// <returns></returns>
		TopoDS_Shape PostprocessBooleanResult(
			TopoDS_Shape& rOcctBooleanResult
		);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctMakeShape"></param>
		/// <param name="rkShapes"></param>
		static void TransferMakeShapeContents(BRepBuilderAPI_MakeShape & rkOcctMakeShape, const std::list<Topology::Ptr>& rkShapes);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctMakeShape"></param>
		/// <param name="rkOcctShapes"></param>
		static void TransferMakeShapeContents(BRepBuilderAPI_MakeShape& rkOcctMakeShape, const TopTools_ListOfShape& rkOcctShapes);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		/// <returns></returns>
		TopoDS_Shape FixBooleanOperandCell(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		/// <returns></returns>
		TopoDS_Shape FixBooleanOperandShell(const TopoDS_Shape& rkOcctShape);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="rkOcctShape"></param>
		/// <param name="rMapFaceToFixedFace"></param>
		/// <returns></returns>
		TopoDS_Shape FixBooleanOperandFace(const TopoDS_Shape& rkOcctShape, TopTools_DataMapOfShapeShape& rMapFaceToFixedFace);

		TopoDS_Shape FixBooleanOperandFace(const TopoDS_Shape& rkOcctShape);

		void GetDeletedBooleanSubtopologies(const TopoDS_Shape& rkOcctShape, BOPAlgo_CellsBuilder& rOcctCellsBuilder, TopTools_ListOfShape& rOcctDeletedShapes);

		void GetDeletedBooleanSubtopologies(const TopoDS_Shape& rkOcctShape, BRepAlgoAPI_BooleanOperation& rOcctBooleanOperation, TopTools_ListOfShape& rOcctDeletedShapes);

		Topology::Ptr TrackContextAncestor();

		static Topology::Ptr IntersectEdgeFace(const Topology::Ptr kpMergeTopology, Edge const * const kpkEdge, Face const * const kpkFace);

		static Topology::Ptr IntersectFaceFace(const Topology::Ptr kpMergeTopology, Face const* const kpkFace, Face const* const kpkOtherFace);

		static Topology::Ptr IntersectEdgeShell(Edge * const kpkEdge, Shell const * const kpkShell);

		int m_dimensionality;
		static int m_numOfTopologies;
	};

	template<class Subclass>
	void Topology::Navigate(std::list<std::shared_ptr<Subclass>>& rMembers) const
	{
		if (Subclass::Type() > GetType())
		{
			UpwardNavigation(rMembers);
		}
		else if (Subclass::Type() < GetType())
		{
			DownwardNavigation(rMembers);
		}
		else
		{
			rMembers.push_back(TopologicalQuery::Downcast<Subclass>(ByOcctShape(GetOcctShape(), GetInstanceGUID())));
		}
	}

	template <class Subclass>
	void Topology::UpwardNavigation(std::list<std::shared_ptr<Subclass>>& rAncestors) const
	{
		UpwardNavigation(GlobalCluster::GetInstance().GetOcctCompound(), rAncestors);
	}

	template<class Subclass>
	inline void Topology::UpwardNavigation(const TopoDS_Shape& rkOcctHostTopology, std::list<std::shared_ptr<Subclass>>& rAncestors) const
	{
		static_assert(std::is_base_of<Topology, Subclass>::value, "Subclass not derived from Topology");

		TopAbs_ShapeEnum occtShapeType = CheckOcctShapeType<Subclass>();

		TopTools_MapOfShape occtAncestorMap;
		TopTools_IndexedDataMapOfShapeListOfShape occtShapeMap;
		TopExp::MapShapesAndUniqueAncestors(
			rkOcctHostTopology,
			GetOcctShape().ShapeType(),
			occtShapeType,
			occtShapeMap);

		TopTools_ListOfShape occtAncestors;
		bool isInShape = occtShapeMap.FindFromKey(GetOcctShape(), occtAncestors);
		if (!isInShape)
		{
			return;
		}

		for (TopTools_ListIteratorOfListOfShape occtAncestorIterator(occtAncestors);
			occtAncestorIterator.More();
			occtAncestorIterator.Next())
		{
			const TopoDS_Shape& rkOcctAncestor = occtAncestorIterator.Value();
			bool isAncestorAdded = occtAncestorMap.Contains(rkOcctAncestor);
			if (rkOcctAncestor.ShapeType() == occtShapeType && !isAncestorAdded)
			{
				occtAncestorMap.Add(rkOcctAncestor);

				Topology::Ptr pTopology = ByOcctShape(rkOcctAncestor, "");
				rAncestors.push_back(Downcast<Subclass>(pTopology));
			}
		}
	}

	template <class Subclass>
	void Topology::DownwardNavigation(std::list<std::shared_ptr<Subclass>>& rMembers) const
	{
		static_assert(std::is_base_of<Topology, Subclass>::value, "Subclass not derived from Topology");

		TopAbs_ShapeEnum occtShapeType = CheckOcctShapeType<Subclass>();
		TopTools_MapOfShape occtShapes;
		for (TopExp_Explorer occtExplorer(GetOcctShape(), occtShapeType); occtExplorer.More(); occtExplorer.Next())
		{
			const TopoDS_Shape& occtCurrent = occtExplorer.Current();
			if (!occtShapes.Contains(occtCurrent))
			{
				occtShapes.Add(occtCurrent);
				Topology::Ptr pChildTopology = ByOcctShape(occtCurrent, "");
				rMembers.push_back(Downcast<Subclass>(pChildTopology));
			}
		}
	}

	template <class Subclass>
	//static TopAbs_ShapeEnum Topology::CheckOcctShapeType()
	TopAbs_ShapeEnum Topology::CheckOcctShapeType()
	{
		if (std::is_same<Subclass, Vertex>::value)
		{
			return TopAbs_VERTEX;
		} 
		else if (std::is_same<Subclass, Edge>::value)
		{
			return TopAbs_EDGE;
		}
		else if (std::is_same<Subclass, Wire>::value)
		{
			return TopAbs_WIRE;
		}
		else if (std::is_same<Subclass, Face>::value)
		{
			return TopAbs_FACE;
		}
		else if (std::is_same<Subclass, Shell>::value)
		{
			return TopAbs_SHELL;
		}
		else if (std::is_same<Subclass, Cell>::value)
		{
			return TopAbs_SOLID;
		}
		else if (std::is_same<Subclass, CellComplex>::value)
		{
			return TopAbs_COMPSOLID;
		}
		else if (std::is_same<Subclass, Cluster>::value)
		{
			return TopAbs_COMPOUND;
		}
		
		throw std::runtime_error("Other subclasses are invalid.");
	}

	/// <summary>
	/// Used by TopologyFactory to pass smart pointers
	/// </summary>
	struct TopologyPtr
	{
		TOPOLOGIC_API TopologyPtr(const std::shared_ptr<Topology>& pTopologyPtr)
			: topologyPtr(pTopologyPtr)
		{
		}

		const std::shared_ptr<Topology>& topologyPtr;
	};

	struct TopologyCompare
	{
		explicit TopologyCompare(const std::shared_ptr<Topology> &baseline) : baseline(baseline) {}
		bool operator() (const std::shared_ptr<Topology> &arg)
		{
			return arg->IsSame(baseline);
		}
		std::shared_ptr<Topology> baseline;
	};
}
