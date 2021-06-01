project(TopologicCore CXX)

# Specify where to install files
IF(NOT INCLUDEDIR)
    set(INCLUDEDIR include)
ENDIF()
IF(NOT IS_ABSOLUTE ${INCLUDEDIR})
    set(INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/${INCLUDEDIR})
ENDIF()
MESSAGE(STATUS "INCLUDEDIR: ${INCLUDEDIR}")

IF(NOT LIBDIR)
    set(LIBDIR lib)
ENDIF()
IF(NOT IS_ABSOLUTE ${LIBDIR})
    set(LIBDIR ${CMAKE_INSTALL_PREFIX}/${LIBDIR})
ENDIF()
MESSAGE(STATUS "LIBDIR: ${LIBDIR}")

# Create cache entries if absent for environment variables
MACRO(UNIFY_ENVVARS_AND_CACHE VAR)
        IF ((NOT DEFINED ${VAR}) AND (NOT "$ENV{${VAR}}" STREQUAL ""))
                SET(${VAR} "$ENV{${VAR}}" CACHE STRING "${VAR}" FORCE)
        ENDIF()
ENDMACRO()

UNIFY_ENVVARS_AND_CACHE(OCC_INCLUDE_DIR)

################################################################################
# Source groups
################################################################################
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

file(GLOB TOPOLOGICCORE_H_FILES include/*.h)
file(GLOB TOPOLOGICCORE_UTILITIES_H_FILES include/Utilities/*.h)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION "0.0.0" SOVERSION "0")

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE TopologicCore)

################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/output/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/output/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/output/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/output/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/output/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/output/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################

# Find opencascade
IF("${OCC_INCLUDE_DIR}" STREQUAL "")
        SET(OCC_INCLUDE_DIR "/usr/include/opencascade/" CACHE FILEPATH "Open CASCADE header files")
        MESSAGE(STATUS "Looking for Open CASCADE include files in: ${OCC_INCLUDE_DIR}")
        MESSAGE(STATUS "Use OCC_INCLUDE_DIR to specify another directory")
ELSE()
        SET(OCC_INCLUDE_DIR ${OCC_INCLUDE_DIR} CACHE FILEPATH "Open CASCADE header files")
        MESSAGE(STATUS "Looking for Open CASCADE include files in: ${OCC_INCLUDE_DIR}")
ENDIF()

FIND_FILE(gp_Pnt_hxx "gp_Pnt.hxx" ${OCC_INCLUDE_DIR})
IF(gp_Pnt_hxx)
        MESSAGE(STATUS "Header files found")
ELSE()
    SET(OCC_INCLUDE_DIR "C:/OpenCASCADE-7.5.0-vc14-64/opencascade-7.5.0/inc/" CACHE FILEPATH "Open CASCADE header files" FORCE)
    MESSAGE(STATUS "Looking for Open CASCADE include files in: ${OCC_INCLUDE_DIR}")
    FIND_FILE(gp_Pnt_hxx "gp_Pnt.hxx" ${OCC_INCLUDE_DIR})
    IF(gp_Pnt_hxx)
        MESSAGE(STATUS "Header files found")
        link_directories("C:/OpenCASCADE-7.5.0-vc14-64/opencascade-7.5.0/win64/vc14/lib")
    ELSE()
        SET (OCC_INCLUDE_DIR "/usr/local/include/opencascade/")
        MESSAGE(STATUS "Looking for Open CASCADE include files in: ${OCC_INCLUDE_DIR}")
        FIND_FILE(gp_Pnt_hxx "gp_Pnt.hxx" ${OCC_INCLUDE_DIR})
        IF(gp_Pnt_hxx)
            MESSAGE(STATUS "Header files found")
        ELSE()
            MESSAGE(FATAL_ERROR "Unable to find header files, aborting. Set OCC_INCLUDE_DIR")
        ENDIF()
    ENDIF()
ENDIF()

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include;"
        ${OCC_INCLUDE_DIR}
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include;"
        ${OCC_INCLUDE_DIR}
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include;"
        ${OCC_INCLUDE_DIR}
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "TOPOLOGIC_EXPORTS;"
        "_WINDLL;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "TOPOLOGICCORE_EXPORTS;"
        "_WINDLL;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "TOPOLOGIC_EXPORTS;"
        "_WINDLL;"
        "_MBCS"
    )
endif()

################################################################################
# Compile and link options
################################################################################

set_xcode_property(${PROJECT_NAME}  CLANG_CXX_LANGUAGE_STANDARD "c++14")
set_xcode_property(${PROJECT_NAME}  CLANG_CXX_LIBRARY "libc++")
if(MSVC)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od;
                ${DEFAULT_CXX_DEBUG_RUNTIME_LIBRARY}
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                ${DEFAULT_CXX_RUNTIME_LIBRARY};
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od;
                ${DEFAULT_CXX_DEBUG_RUNTIME_LIBRARY}
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                ${DEFAULT_CXX_RUNTIME_LIBRARY};
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od;
                ${DEFAULT_CXX_DEBUG_RUNTIME_LIBRARY}
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                ${DEFAULT_CXX_RUNTIME_LIBRARY};
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG:FULL
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG:FULL
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG:FULL
        )
    endif()
endif()

################################################################################
# Post build events
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMANDS
        COMMAND $<CONFIG:Release> copy "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>references\\opencascade-7.5.0\\win32\\vc14\\bin\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMANDS
        COMMAND   $<CONFIG:Debug> copy "C:\\OpenCASCADE-7.5.0-vc14-64\\opencascade-7.5.0\\win64\\vc14\\bin\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
        COMMAND   $<CONFIG:Debug> copy "C:\\OpenCASCADE-7.5.0-vc14-64\\ffmpeg-3.3.4-64\\bin\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
        COMMAND   $<CONFIG:Debug> copy "C:\\OpenCASCADE-7.5.0-vc14-64\\freeimage-3.17.0-vc14-64\\bin\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
        COMMAND   $<CONFIG:Debug> copy "C:\\OpenCASCADE-7.5.0-vc14-64\\tbb_2017.0.100\\bin\\intel64\\vc14\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
        COMMAND   $<CONFIG:Debug> copy "C:\\OpenCASCADE-7.5.0-vc14-64\\vtk-6.1.0-vc14-64\\bin\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
        COMMAND   $<CONFIG:Debug> copy "C:\\OpenCASCADE-7.5.0-vc14-64\\tcltk-86-64\\bin\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
        COMMAND   $<CONFIG:Debug> copy "C:\\OpenCASCADE-7.5.0-vc14-64\\qt5.11.2-vc14-64\\bin\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
        COMMAND   $<CONFIG:Debug> copy "C:\\OpenCASCADE-7.5.0-vc14-64\\freetype-2.5.5-vc14-64\\bin\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
        COMMAND $<CONFIG:Release> copy "C:\\OpenCASCADE-7.5.0-vc14-64\\opencascade-7.5.0\\win64\\vc14\\bin\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
        COMMAND $<CONFIG:Release> copy "C:\\OpenCASCADE-7.5.0-vc14-64\\ffmpeg-3.3.4-64\\bin\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
        COMMAND $<CONFIG:Release> copy "C:\\OpenCASCADE-7.5.0-vc14-64\\freeimage-3.17.0-vc14-64\\bin\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
        COMMAND $<CONFIG:Release> copy "C:\\OpenCASCADE-7.5.0-vc14-64\\tbb_2017.0.100\\bin\\intel64\\vc14\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
        COMMAND $<CONFIG:Release> copy "C:\\OpenCASCADE-7.5.0-vc14-64\\vtk-6.1.0-vc14-64\\bin\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
        COMMAND $<CONFIG:Release> copy "C:\\OpenCASCADE-7.5.0-vc14-64\\tcltk-86-64\\bin\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
        COMMAND $<CONFIG:Release> copy "C:\\OpenCASCADE-7.5.0-vc14-64\\qt5.11.2-vc14-64\\bin\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
        COMMAND $<CONFIG:Release> copy "C:\\OpenCASCADE-7.5.0-vc14-64\\freetype-2.5.5-vc14-64\\bin\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    add_custom_command_if(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMANDS
        COMMAND $<CONFIG:Release> copy "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>references\\opencascade-7.5.0\\win32\\vc14\\bin\\*.dll" "$<SHELL_PATH:${CMAKE_SOURCE_DIR}/>output\\${CMAKE_VS_PLATFORM_NAME}\\$<CONFIG>"
    )
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "TKCAF;"
    "TKLCAF;"
    "TKOffset;"
    "TKIGES;"
    "TKPrim;"
    "TKMesh;"
    "TKXSBase;"
    "TKBool;"
    "TKBO;"
    "TKShHealing;"
    "TKG3d;"
    "TKG2d;"
    "TKGeomBase;"
    "TKGeomAlgo;"
    "TKTopAlgo;"
    "TKernel;"
    "TKMath;"
    "uuid;"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "TKCAF;"
    "TKLCAF;"
    "TKOffset;"
    "TKIGES;"
    "TKPrim;"
    "TKMesh;"
    "TKXSBase;"
    "TKBool;"
    "TKBO;"
    "TKShHealing;"
    "TKG3d;"
    "TKG2d;"
    "TKGeomBase;"
    "TKGeomAlgo;"
    "TKTopAlgo;"
    "TKernel;"
    "TKMath;"
    "uuid;"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
    "TKCAF;"
    "TKLCAF;"
    "TKOffset;"
    "TKIGES;"
    "TKPrim;"
    "TKMesh;"
    "TKXSBase;"
    "TKBool;"
    "TKBO;"
    "TKShHealing;"
    "TKG3d;"
    "TKG2d;"
    "TKGeomBase;"
    "TKGeomAlgo;"
    "TKTopAlgo;"
    "TKernel;"
    "TKMath;"
    "uuid;"
    )
endif()

if (MSVC OR APPLE)
    set (ADDITIONAL_LIBRARY_DEPENDENCIES "${ADDITIONAL_LIBRARY_DEPENDENCIES};TKBrep;TKStep;")
endif()

#find_library(TEST_LIB NAMES TKCAF)

#target_link_libraries(${PROJECT_NAME} PUBLIC "${TEST_LIB};${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if(MSVC)
    SET (CUSTOM_LIB_PATH "C:/OpenCASCADE-7.5.0-vc14-64/opencascade-7.5.0/win64/vc14/lib")
ELSE()
    SET (CUSTOM_LIB_PATH "/usr/lib64")
    
ENDIF()

MESSAGE (STATUS "Target: ${CMAKE_VS_PLATFORM_NAME}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Debug>:"
            "${CUSTOM_LIB_PATH}"
        ">"
        "$<$<CONFIG:Release>:"
            "${CUSTOM_LIB_PATH}"
        ">"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Debug>:"
            "${CUSTOM_LIB_PATH}"
        ">"
        "$<$<CONFIG:Release>:"
            "${CUSTOM_LIB_PATH}"
        ">"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_link_directories(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Debug>:"
            "${CUSTOM_LIB_PATH}"
        ">"
        "$<$<CONFIG:Release>:"
            "${CUSTOM_LIB_PATH}"
        ">"
    )
endif()

#find_library(TEST_LIB NAMES TKCAF)


IF (APPLE)
    FOREACH (LIB ${ADDITIONAL_LIBRARY_DEPENDENCIES})
        #set (FIND_LIB "")
        find_library(FIND_LIB NAMES "${LIB}")
        
        
        IF (NOT (${FIND_LIB} STREQUAL "FIND_LIB-NOTFOUND"))
            target_link_libraries(${PROJECT_NAME} PUBLIC ${FIND_LIB})
            MESSAGE (STATUS "${LIB}: ${FIND_LIB}")
            #MESSAGE (STATUS "${LIB}: ${FIND_LIB}")
        ENDIF()
        unset (FIND_LIB CACHE)
    ENDFOREACH()
ELSE ()
    target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
ENDIF()


# CMake installation targets
INSTALL(FILES ${TOPOLOGICCORE_H_FILES}
        DESTINATION ${INCLUDEDIR}/${PROJECT_NAME}
)
INSTALL(FILES ${TOPOLOGICCORE_UTILITIES_H_FILES}
        DESTINATION ${INCLUDEDIR}/${PROJECT_NAME}/Utilities
)

INSTALL(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${LIBDIR}
)

