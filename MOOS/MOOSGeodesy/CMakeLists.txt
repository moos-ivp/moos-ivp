
###########################################################
#   Top level CMakeLists.txt file for the MOOS Project
#   pnewman@robots.ox.ac.uk
###########################################################


CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

if(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(PROJECTNAME MOOSGeodesy)
project(${PROJECTNAME})

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)


set(CMakeScripts_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMakeScripts_MODULE_DIR})


# Turn on all compiler warnings
include(EnableAllWarnings)

# Bring in helper functions for dealing with CACHE INTERNAL variables
include(CacheInternalHelpers)

# We want to create dynamic libraries
set(BUILD_SHARED_LIBS true)

option(MOOSGEODESY_USE_PROJ "Use Proj for UTM coordinate transformations" ON)
set(MOOSGEODESY_FETCH_PROJ "auto" CACHE STRING
    "Fetch PROJ: auto when a compatible system library is unavailable, on always, off never")
set_property(CACHE MOOSGEODESY_FETCH_PROJ PROPERTY STRINGS auto on off)
string(TOLOWER "${MOOSGEODESY_FETCH_PROJ}" MOOSGEODESY_FETCH_PROJ)

if(NOT MOOSGEODESY_FETCH_PROJ MATCHES "^(auto|on|off)$")
    message(FATAL_ERROR
        "MOOSGEODESY_FETCH_PROJ must be one of: auto, on, off")
endif()

if(MOOSGEODESY_USE_PROJ)
    set(MOOSGEODESY_FETCH_PROJ_REQUIRED FALSE)

    if(NOT MOOSGEODESY_FETCH_PROJ STREQUAL "on")
        find_path(PROJ4_INCLUDE_DIR
            NAMES proj_api.h
            HINTS ${PROJ4_INCLUDE_DIRS}
        )
        find_library(PROJ4_LIBRARY
            NAMES proj
            HINTS ${PROJ4_LIB_PATH}
        )
    endif()

    if(MOOSGEODESY_FETCH_PROJ STREQUAL "on" OR
       (MOOSGEODESY_FETCH_PROJ STREQUAL "auto" AND
        NOT (PROJ4_INCLUDE_DIR AND PROJ4_LIBRARY)))
        set(MOOSGEODESY_FETCH_PROJ_REQUIRED TRUE)
    endif()

    if(MOOSGEODESY_FETCH_PROJ_REQUIRED)
        if(NOT DEFINED CPM_SOURCE_CACHE AND
           NOT DEFINED ENV{CPM_SOURCE_CACHE})
            if(DEFINED ENV{XDG_CACHE_HOME} AND
               IS_ABSOLUTE "$ENV{XDG_CACHE_HOME}")
                set(CPM_SOURCE_CACHE "$ENV{XDG_CACHE_HOME}/CPM"
                    CACHE PATH "Directory to download CPM dependencies")
            elseif(DEFINED ENV{HOME} AND IS_ABSOLUTE "$ENV{HOME}")
                set(CPM_SOURCE_CACHE "$ENV{HOME}/.cache/CPM"
                    CACHE PATH "Directory to download CPM dependencies")
            endif()
        endif()

        if(NOT DEFINED CPM_USE_NAMED_CACHE_DIRECTORIES AND
           NOT DEFINED ENV{CPM_USE_NAMED_CACHE_DIRECTORIES})
            set(CPM_USE_NAMED_CACHE_DIRECTORIES ON CACHE BOOL
                "Use additional directory of package name in CPM cache")
        endif()

        include(CPM)

        # MOOSGeodesy uses the legacy proj_api.h API, which was removed in
        # later PROJ releases. The patch makes the upstream 5.2.0 build
        # embeddable and compatible with current CMake releases.
        CPMAddPackage(
            NAME proj
            VERSION 5.2.0
            URL_HASH
                SHA256=11958654f92314ba4fd72fb697d975d2a9c22cd7eb3545798019b099f8285d91
            PATCH_COMMAND
                "${CMAKE_COMMAND}"
                "-DPATCH_FILE=${CMAKE_CURRENT_SOURCE_DIR}/cmake/proj-5.2.0.patch"
                "-DPATCH_WORKING_DIRECTORY=<SOURCE_DIR>"
                -P
                "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ApplyPatch.cmake"
            URL
                https://github.com/OSGeo/PROJ/archive/refs/tags/5.2.0.tar.gz
            OPTIONS
                "BUILD_LIBPROJ_SHARED OFF"
                "PROJ_TESTS OFF"
                "BUILD_CCT OFF"
                "BUILD_CS2CS OFF"
                "BUILD_GEOD OFF"
                "BUILD_GIE OFF"
                "BUILD_NAD2BIN OFF"
                "BUILD_PROJ OFF"
        )

        set_target_properties(proj PROPERTIES POSITION_INDEPENDENT_CODE ON)
        set(MOOSGEODESY_PROJ_INCLUDE_DIR ${proj_SOURCE_DIR}/src)
        set(MOOSGEODESY_PROJ_LIBRARY proj)
        message(STATUS "MOOSGeodesy: using fetched PROJ")
    elseif(PROJ4_INCLUDE_DIR AND PROJ4_LIBRARY)
        set(MOOSGEODESY_PROJ_INCLUDE_DIR ${PROJ4_INCLUDE_DIR})
        set(MOOSGEODESY_PROJ_LIBRARY ${PROJ4_LIBRARY})
        message(STATUS "MOOSGeodesy: using system PROJ: ${PROJ4_LIBRARY}")
    else()
        message(FATAL_ERROR
            "MOOSGEODESY_FETCH_PROJ=off requires a compatible system PROJ "
            "with the proj_api.h API. Set PROJ4_INCLUDE_DIRS and "
            "PROJ4_LIB_PATH to provide it.")
    endif()
endif()

# Enable Doxygen build with 'make doxygen'
option(ENABLE_DOXYGEN "Enable a 'make doc' target for Doxygen documentation")
if (ENABLE_DOXYGEN)
    include(UseDoxygen)
endif()


###########  SETTING UP PREPROCESSOR ################
include(PlatformDefines)


#####################################################
# Toby Schneider's request for fPIC on amd64
IF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
 SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC"  )
ENDIF("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

add_subdirectory(libMOOSGeodesy)

#########################
# Config Script Creation
#########################

set(MOOSGEODESY_CONFIG_INSTALL_DIR
    "${CMAKE_INSTALL_LIBDIR}/cmake/MOOSGeodesy")

# Make the independently built source tree directly consumable.
export(EXPORT MOOSGeodesyTargets
    NAMESPACE MOOSGeodesy::
    FILE "${PROJECT_BINARY_DIR}/MOOSGeodesyTargets.cmake")

set(MOOSGEODESY_CONFIG_INCLUDE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/libMOOSGeodesy/include")
configure_package_config_file(
    "cmake/ProjectConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/MOOSGeodesyConfig.cmake"
    INSTALL_DESTINATION "."
    INSTALL_PREFIX "${PROJECT_BINARY_DIR}"
    PATH_VARS MOOSGEODESY_CONFIG_INCLUDE_DIR)

# Install a relocatable package alongside the library.
install(EXPORT MOOSGeodesyTargets
    NAMESPACE MOOSGeodesy::
    FILE MOOSGeodesyTargets.cmake
    DESTINATION "${MOOSGEODESY_CONFIG_INSTALL_DIR}")

set(MOOSGEODESY_CONFIG_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}")
configure_package_config_file(
    "cmake/ProjectConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/MOOSGeodesyInstallConfig.cmake"
    INSTALL_DESTINATION "${MOOSGEODESY_CONFIG_INSTALL_DIR}"
    PATH_VARS MOOSGEODESY_CONFIG_INCLUDE_DIR)
install(FILES "${PROJECT_BINARY_DIR}/MOOSGeodesyInstallConfig.cmake"
    RENAME MOOSGeodesyConfig.cmake
    DESTINATION "${MOOSGEODESY_CONFIG_INSTALL_DIR}")

option(ENABLE_EXPORT
    "Cause CMake to record this build so other projects can find it" ON)
if(ENABLE_EXPORT)
    message(STATUS "Location of this build will be exported. Set ENABLE_EXPORT "
                   "to OFF to disable the CMake user package registry entry.")
    export(PACKAGE MOOSGeodesy)
endif()
