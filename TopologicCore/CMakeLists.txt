cmake_minimum_required(VERSION 3.15)

project(TopologicCore CXX)
set(PROJECT_NAMESPACE Topologic)

option(${PROJECT_NAME}_BUILD_SHARED "Build ${PROJECT_NAME} as a shared library" ${BUILD_SHARED_LIBS})

if (${PROJECT_NAME}_BUILD_SHARED)
    set(${PROJECT_NAME}_LIBRARY_TYPE SHARED)
else()
    set(${PROJECT_NAME}_LIBRARY_TYPE STATIC)
endif()


# Source groups
set(no_group_source_files
    "include/About.h"
    "include/Aperture.h"
    "include/Cell.h"
    "include/CellComplex.h"
    "include/Cluster.h"
    "include/ContentManager.h"
    "include/Context.h"
    "include/ContextManager.h"
    "include/Dictionary.h"
    "include/Edge.h"
    "include/Face.h"
    "include/GlobalCluster.h"
    "include/Graph.h"
    "include/InstanceGUIDManager.h"
    "include/Shell.h"
    "include/TopologicalQuery.h"
    "include/Topology.h"
    "include/Utilities.h"
    "include/Vertex.h"
    "include/Wire.h"
    "src/About.cpp"
    "src/Aperture.cpp"
    "src/Cell.cpp"
    "src/CellComplex.cpp"
    "src/Cluster.cpp"
    "src/ContentManager.cpp"
    "src/Context.cpp"
    "src/ContextManager.cpp"
    "src/Dictionary.cpp"
    "src/Edge.cpp"
    "src/Face.cpp"
    "src/GlobalCluster.cpp"
    "src/Graph.cpp"
    "src/InstanceGUIDManager.cpp"
    "src/Shell.cpp"
    "src/Topology.cpp"
    "src/Utilities.cpp"
    "src/Vertex.cpp"
    "src/Wire.cpp"
)
source_group("" FILES ${no_group_source_files})

set(Attributes
    "include/Attribute.h"
    "include/AttributeManager.h"
    "include/DoubleAttribute.h"
    "include/IntAttribute.h"
    "include/ListAttribute.h"
    "include/StringAttribute.h"
    "src/AttributeManager.cpp"
    "src/DoubleAttribute.cpp"
    "src/IntAttribute.cpp"
    "src/ListAttribute.cpp"
    "src/StringAttribute.cpp"
)
source_group("Attributes" FILES ${Attributes})

set(Factories
    "include/ApertureFactory.h"
    "include/CellComplexFactory.h"
    "include/CellFactory.h"
    "include/ClusterFactory.h"
    "include/EdgeFactory.h"
    "include/FaceFactory.h"
    "include/ShellFactory.h"
    "include/TopologyFactory.h"
    "include/TopologyFactoryManager.h"
    "include/VertexFactory.h"
    "include/WireFactory.h"
    "src/ApertureFactory.cpp"
    "src/CellComplexFactory.cpp"
    "src/CellFactory.cpp"
    "src/ClusterFactory.cpp"
    "src/EdgeFactory.cpp"
    "src/FaceFactory.cpp"
    "src/ShellFactory.cpp"
    "src/TopologyFactoryManager.cpp"
    "src/VertexFactory.cpp"
    "src/WireFactory.cpp"
)
source_group("Factories" FILES ${Factories})

set(Geometries
    "include/Geometry.h"
    "include/Line.h"
    "include/NurbsCurve.h"
    "include/NurbsSurface.h"
    "include/PlanarSurface.h"
    "include/Surface.h"
    "src/Geometry.cpp"
    "src/Line.cpp"
    "src/NurbsCurve.cpp"
    "src/NurbsSurface.cpp"
    "src/PlanarSurface.cpp"
    "src/Surface.cpp"
)
source_group("Geometries" FILES ${Geometries})

set(Utilities
    "include/Bitwise.h"
    "include/Utilities/CellUtility.h"
    "include/Utilities/Direction.h"
    "include/Utilities/EdgeUtility.h"
    "include/Utilities/FaceUtility.h"
    "include/Utilities/ShellUtility.h"
    "include/Utilities/TopologyUtility.h"
    "include/Utilities/TransformationMatrix2D.h"
    "include/Utilities/Vector.h"
    "include/Utilities/VertexUtility.h"
    "include/Utilities/WireUtility.h"
    "src/Bitwise.cpp"
    "src/Utilities/CellUtility.cpp"
    "src/Utilities/Direction.cpp"
    "src/Utilities/EdgeUtility.cpp"
    "src/Utilities/FaceUtility.cpp"
    "src/Utilities/ShellUtility.cpp"
    "src/Utilities/TopologyUtility.cpp"
    "src/Utilities/TransformationMatrix2D.cpp"
    "src/Utilities/Vector.cpp"
    "src/Utilities/VertexUtility.cpp"
    "src/Utilities/WireUtility.cpp"
)
source_group("Utilities" FILES ${Utilities})

set(ALL_FILES
    ${no_group_source_files}
    ${Attributes}
    ${Factories}
    ${Geometries}
    ${Utilities}
)

#file(GLOB TOPOLOGICCORE_H_FILES include/*.h)
#file(GLOB TOPOLOGICCORE_UTILITIES_H_FILES include/Utilities/*.h)


# Library target
add_library(${PROJECT_NAME} ${${PROJECT_NAME}_LIBRARY_TYPE} ${ALL_FILES})
add_library(${PROJECT_NAMESPACE}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)
# Include dir
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
# Set version
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION "0.0.0" SOVERSION "0")

if (${PROJECT_NAME}_LIBRARY_TYPE STREQUAL STATIC)
  set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE True)
endif()

# Dependency: OpenCASCADE
find_package(OpenCASCADE REQUIRED)
set(DEPENDENCIES_OpenCASCADE
        "TKOffset;"
        "TKMesh;"
        "TKBO;"
        "TKPrim;"
        "TKShHealing;"
        "TKTopAlgo;"
        "TKGeomAlgo;"
        "TKBRep;"
        "TKGeomBase;"
        "TKG3d;"
        "TKG2d;"
        "TKMath;"
        "TKernel;"
        "TKFillet;"
        "TKBool;"
        )
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEPENDENCIES_OpenCASCADE})

# hacky: remove propagated OpenCASCADE linkage when building a shared library
# the API of TopologicPythonBindings references symbols from OpenCASCADE directly
# and this results in it getting directly linked to some OpenCASCADE shared libraries
# if we want to prevent this and link it only to TopologicCore (which in turn is linked to OpenCASCADE of course)
# it can be done by removing OpenCASCADE targets from INTERFACE_LINK_LIBRARIES
if (${PROJECT_NAME}_LIBRARY_TYPE STREQUAL SHARED)
    set_property(TARGET ${PROJECT_NAME} PROPERTY INTERFACE_LINK_LIBRARIES "")
    # setting INTERFACE_LINK_LIBRARIES to an empty list since there are not other dependencies for now
endif()
# a downside that this also prevents the propagation of include dirs
# so in this case TopologicPythonBindings has to declare OpenCASCADE as direct dependency
# and find its include dirs to compile
#  might revisit later if doing all this is worth it at all


#include(CMakePrintHelpers)
#cmake_print_properties(TARGETS ${PROJECT_NAME} PROPERTIES TYPE POSITION_INDEPENDENT_CODE LINK_LIBRARIES LINK_INTERFACE_LIBRARIES INTERFACE_LINK_LIBRARIES INTERFACE_COMPILE_DEFINITIONS INTERFACE_INCLUDE_DIRECTORIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES INCLUDE_DIRECTORIES)
