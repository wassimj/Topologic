cmake_minimum_required(VERSION 3.24)

project(TopologicPythonBindings CXX)
set(PROJECT_NAMESPACE Topologic)


# Import pybind11
set(PYBIND11_FINDPYTHON ON)
add_subdirectory(pybind11)


# Source groups
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
  ./src/VertexUtility.Binding.cpp
  ./src/EdgeUtility.Binding.cpp
  ./src/WireUtility.Binding.cpp
  ./src/ShellUtility.Binding.cpp
  ./src/CellUtility.Binding.cpp
  ./src/TopologyUtility.Binding.cpp
  ./src/FaceUtility.Binding.cpp
  ./src/main.cpp
)

# Python module target
pybind11_add_module(${PROJECT_NAME} MODULE ${SRC_FILES})
set(PY_MODULE_NAME topologic)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PY_MODULE_NAME})

# Include dir
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Import TopologicCore from parent dir when not building from global project
if(NOT TARGET ${PROJECT_NAMESPACE}::TopologicCore)
    add_subdirectory(${PROJECT_SOURCE_DIR}/../TopologicCore ${PROJECT_BINARY_DIR}/TopologicCore)
endif()

# Dependency: TopologicCore
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAMESPACE}::TopologicCore)


# install definitions
install(TARGETS ${PROJECT_NAME}
        DESTINATION lib/${PROJECT_NAME}
        COMPONENT ${PROJECT_NAME}
        )

# set install rpaths to origin if TopologicCore is built as a shared lib
# in this case TopologicCore is expected to be copied into the TopologicPythonBindings install dir post-install
get_property(TopologicCore_TARGET_TYPE TARGET TopologicCore PROPERTY TYPE)
if (${TopologicCore_TARGET_TYPE} STREQUAL SHARED_LIBRARY)
    if(NOT CMAKE_INSTALL_RPATH)
        if(NOT APPLE)
            set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN")
        endif()
        if(APPLE)
            set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "@loader_path") # TODO
        endif()
    endif()
endif()




#include(CMakePrintHelpers)
#cmake_print_properties(TARGETS ${PROJECT_NAME} PROPERTIES TYPE POSITION_INDEPENDENT_CODE LINK_LIBRARIES LINK_INTERFACE_LIBRARIES INTERFACE_LINK_LIBRARIES INTERFACE_LINK_LIBRARIES_DIRECT_EXCLUDE INTERFACE_COMPILE_DEFINITIONS INTERFACE_INCLUDE_DIRECTORIES INTERFACE_SYSTEM_INCLUDE_DIRECTORIES INCLUDE_DIRECTORIES INSTALL_RPATH)
