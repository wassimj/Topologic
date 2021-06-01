# Install script for directory: C:/Users/wassimj/topologicbim/Topologic/TopologicCore

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Topologic")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/Topologic/include/TopologicCore/About.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Aperture.h;C:/Program Files (x86)/Topologic/include/TopologicCore/ApertureFactory.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Attribute.h;C:/Program Files (x86)/Topologic/include/TopologicCore/AttributeManager.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Bitwise.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Cell.h;C:/Program Files (x86)/Topologic/include/TopologicCore/CellComplex.h;C:/Program Files (x86)/Topologic/include/TopologicCore/CellComplexFactory.h;C:/Program Files (x86)/Topologic/include/TopologicCore/CellFactory.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Cluster.h;C:/Program Files (x86)/Topologic/include/TopologicCore/ClusterFactory.h;C:/Program Files (x86)/Topologic/include/TopologicCore/ContentManager.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Context.h;C:/Program Files (x86)/Topologic/include/TopologicCore/ContextManager.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Dictionary.h;C:/Program Files (x86)/Topologic/include/TopologicCore/DoubleAttribute.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Edge.h;C:/Program Files (x86)/Topologic/include/TopologicCore/EdgeFactory.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Face.h;C:/Program Files (x86)/Topologic/include/TopologicCore/FaceFactory.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Geometry.h;C:/Program Files (x86)/Topologic/include/TopologicCore/GlobalCluster.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Graph.h;C:/Program Files (x86)/Topologic/include/TopologicCore/InstanceGUIDManager.h;C:/Program Files (x86)/Topologic/include/TopologicCore/IntAttribute.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Line.h;C:/Program Files (x86)/Topologic/include/TopologicCore/ListAttribute.h;C:/Program Files (x86)/Topologic/include/TopologicCore/NurbsCurve.h;C:/Program Files (x86)/Topologic/include/TopologicCore/NurbsSurface.h;C:/Program Files (x86)/Topologic/include/TopologicCore/PlanarSurface.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Shell.h;C:/Program Files (x86)/Topologic/include/TopologicCore/ShellFactory.h;C:/Program Files (x86)/Topologic/include/TopologicCore/StringAttribute.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Surface.h;C:/Program Files (x86)/Topologic/include/TopologicCore/TopologicalQuery.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Topology.h;C:/Program Files (x86)/Topologic/include/TopologicCore/TopologyFactory.h;C:/Program Files (x86)/Topologic/include/TopologicCore/TopologyFactoryManager.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Utilities.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Vertex.h;C:/Program Files (x86)/Topologic/include/TopologicCore/VertexFactory.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Wire.h;C:/Program Files (x86)/Topologic/include/TopologicCore/WireFactory.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/Program Files (x86)/Topologic/include/TopologicCore" TYPE FILE FILES
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/About.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Aperture.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/ApertureFactory.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Attribute.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/AttributeManager.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Bitwise.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Cell.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/CellComplex.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/CellComplexFactory.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/CellFactory.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Cluster.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/ClusterFactory.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/ContentManager.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Context.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/ContextManager.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Dictionary.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/DoubleAttribute.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Edge.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/EdgeFactory.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Face.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/FaceFactory.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Geometry.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/GlobalCluster.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Graph.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/InstanceGUIDManager.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/IntAttribute.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Line.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/ListAttribute.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/NurbsCurve.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/NurbsSurface.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/PlanarSurface.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Shell.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/ShellFactory.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/StringAttribute.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Surface.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/TopologicalQuery.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Topology.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/TopologyFactory.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/TopologyFactoryManager.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Utilities.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Vertex.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/VertexFactory.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Wire.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/WireFactory.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Program Files (x86)/Topologic/include/TopologicCore/Utilities/CellUtility.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Utilities/Direction.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Utilities/EdgeUtility.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Utilities/FaceUtility.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Utilities/ShellUtility.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Utilities/TopologyUtility.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Utilities/TransformationMatrix2D.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Utilities/Vector.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Utilities/VertexUtility.h;C:/Program Files (x86)/Topologic/include/TopologicCore/Utilities/WireUtility.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "C:/Program Files (x86)/Topologic/include/TopologicCore/Utilities" TYPE FILE FILES
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Utilities/CellUtility.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Utilities/Direction.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Utilities/EdgeUtility.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Utilities/FaceUtility.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Utilities/ShellUtility.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Utilities/TopologyUtility.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Utilities/TransformationMatrix2D.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Utilities/Vector.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Utilities/VertexUtility.h"
    "C:/Users/wassimj/topologicbim/Topologic/TopologicCore/include/Utilities/WireUtility.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/wassimj/topologicbim/Topologic/output/x64/Release/TopologicCore.lib")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/wassimj/topologicbim/Topologic/output/x64/Release/TopologicCore.dll")
endif()

