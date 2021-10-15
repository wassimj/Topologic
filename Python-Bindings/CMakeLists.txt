cmake_minimum_required(VERSION 3.10)
project(topologic)

set(CMAKE_CXX_STANDARD 17)
list(APPEND CMAKE_PREFIX_PATH "./pybind")

find_package(PythonLibs REQUIRED)
find_package(pybind11 REQUIRED)

include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(../TopologicCore/include)
include_directories(./include)

# This is a hardcoded path as we expect OpenCASCADE to be exactly
# in this directory. Revisit if this changes!
include_directories(C:/OpenCASCADE-7.4.0-vc14-64/opencascade-7.4.0/inc)
SET(CUSTOM_LIB_PATH "C:/OpenCASCADE-7.4.0-vc14-64/opencascade-7.4.0/win64/vc14/lib")

add_library(TopologicCore SHARED IMPORTED)
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "TKernel;"
)

SET_PROPERTY(TARGET TopologicCore PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/../output/${CMAKE_GENERATOR_PLATFORM}/${CMAKE_BUILD_TYPE}/TopologicCore.dll)
SET_PROPERTY(TARGET TopologicCore PROPERTY IMPORTED_IMPLIB ${CMAKE_CURRENT_LIST_DIR}/../output/${CMAKE_GENERATOR_PLATFORM}/${CMAKE_BUILD_TYPE}/TopologicCore.lib)

set(SRC_FILES 
  ./src/TopologicalQuery.cppwg.cpp
  ./src/Topology.cppwg.cpp
  ./src/Vertex.cppwg.cpp
  ./src/Edge.cppwg.cpp
  ./src/Wire.cppwg.cpp
  ./src/Face.cppwg.cpp
  ./src/Shell.cppwg.cpp
  ./src/Cell.cppwg.cpp
  ./src/CellComplex.cppwg.cpp
  ./src/Cluster.cppwg.cpp
  ./src/Aperture.cppwg.cpp
  ./src/Graph.cppwg.cpp
  ./src/Attribute.cppwg.cpp
  ./src/Dictionary.cppwg.cpp
  ./src/ContentManager.cppwg.cpp
  ./src/Context.cppwg.cpp
  ./src/IntAttribute.cppwg.cpp
  ./src/StringAttribute.cppwg.cpp
  ./src/DoubleAttribute.cppwg.cpp
  ./src/ListAttribute.cppwg.cpp
  ./src/main.cpp
)

pybind11_add_module(${PROJECT_NAME} SHARED ${SRC_FILES})
target_link_directories(${PROJECT_NAME} PUBLIC ${CUSTOM_LIB_PATH})
target_link_libraries(${PROJECT_NAME} PRIVATE TopologicCore ${ADDITIONAL_LIBRARY_DEPENDENCIES})