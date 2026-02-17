cmake_minimum_required(VERSION 3.24)

if (BUILD_VERSION)
    set(BUILD_VERSION ${BUILD_VERSION})
    unset(BUILD_VERSION CACHE)
else()
    set(BUILD_VERSION 8.0.3) # Default version if not set in BUILD_VERSION.
endif()

project(Topologic VERSION ${BUILD_VERSION} LANGUAGES CXX)
message("CMAKE_PROJECT_VERSION=${CMAKE_PROJECT_VERSION}")

# Sub-projects
add_subdirectory(TopologicCore)
add_subdirectory(TopologicPythonBindings)

SET(CPACK_SOURCE_PACKAGE_FILE_NAME "Topologic-${BUILD_VERSION}")
SET(CPACK_SOURCE_GENERATOR "ZIP")
SET(CPACK_SOURCE_IGNORE_FILES "\\\\.git" "/build" "/BUILD")

INCLUDE(CPack)
