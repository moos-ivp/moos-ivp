
###########################################################
#   Top level CMakeLists.txt file for the core-moos Project
#   pnewman@robots.ox.ac.uk
###########################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

if(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(PROJECTNAME MOOS)

set(PACKAGE_VERSION_MAJOR 10)
set(PACKAGE_VERSION_MINOR 0)
set(PACKAGE_VERSION_PATCH 1)

set(PACKAGE_VERSION "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_PATCH}")
message(STATUS "+++  This MOOS version ${PACKAGE_VERSION} +++")


project(${PROJECTNAME})

if(NOT DEFINED CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
endif()

if(NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
endif()
if(NOT DEFINED CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
endif()


set(CMakeScripts_MODULE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMakeScripts_MODULE_DIR})


# Turn on all compiler warnings
include(EnableAllWarnings)

# Bring in helper functions for dealing with CACHE INTERNAL variables
include(CacheInternalHelpers)

# We want to create dynamic libraries
set(BUILD_SHARED_LIBS true)

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


add_subdirectory(Core)


#########################
# Config Script Creation
#########################

# Add your library's name to PROJECT_EXPORT_LIBS if you wish to have it exported
# (This is a CACHE INTERNAL variable)
option(ENABLE_EXPORT "do you want CMake to record this build so other projects can find it " ON)

if(ENABLE_EXPORT)
   message(STATUS "location of this build will be exported, set ENABLE_EXPORT to OFF if you don't want this and delete ~/.cmake/projects/MOOS ")
    if (PROJECT_EXPORT_LIBS)
        
        message(STATUS "PROJECT_EXPORT_LIBS = ${PROJECT_EXPORT_LIBS}")
        
        # Provide a cmake config script for the build directory
        export(TARGETS
            ${PROJECT_EXPORT_LIBS}
            FILE
            Use${PROJECTNAME}.cmake
        )
    
        set(CONFIG_FILE "${PROJECT_BINARY_DIR}/${PROJECTNAME}Config.cmake")
        configure_file( "cmake/ProjectConfig.cmake.in" ${CONFIG_FILE} @ONLY )
        
        # Append libary specific lines to the generated config script
        foreach(EXPORT_LIB ${PROJECT_EXPORT_LIBS})
            file(APPEND ${CONFIG_FILE} "\n# Export variables for ${EXPORT_LIB} Library\n")
            file(APPEND ${CONFIG_FILE} "set(${EXPORT_LIB}_LIBRARIES \"${${EXPORT_LIB}_LIBRARIES}\")\n")
            file(APPEND ${CONFIG_FILE} "set(${EXPORT_LIB}_INCLUDE_DIRS \"${${EXPORT_LIB}_INCLUDE_DIRS}\")\n")
            file(APPEND ${CONFIG_FILE} "set(${EXPORT_LIB}_DEPEND_LIBRARIES \"${${EXPORT_LIB}_DEPEND_LIBRARIES}\")\n")
            file(APPEND ${CONFIG_FILE} "set(${EXPORT_LIB}_DEPEND_INCLUDE_DIRS \"${${EXPORT_LIB}_DEPEND_INCLUDE_DIRS}\")\n")
        endforeach()
        
        foreach(DEFINE ${PROJECT_EXPORT_DEFINES})        
            file(APPEND ${CONFIG_FILE} "ADD_DEFINITIONS(-D${DEFINE})\n")        
            file(APPEND ${CONFIG_FILE}  "message(STATUS \"adding -D${DEFINE} as required my MOOS\")" )
        endforeach()
        
        
    endif()

# Stick build location in the central CMake user package registry, so that it may be easily found
if ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.7)
  export(PACKAGE ${PROJECTNAME})
endif()

endif()

#here we save a version file
set(VERSION_FILE "${PROJECT_BINARY_DIR}/${PROJECTNAME}ConfigVersion.cmake")
configure_file( "cmake/ProjectConfigVersion.cmake.in" ${VERSION_FILE} @ONLY )




