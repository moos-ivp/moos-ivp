CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# --------------------------------------------------------------
# Indicate CMake 2.7 and above that we don't want to mix relative
#  and absolute paths in linker lib lists.
# Run "cmake --help-policy CMP0003" for more information.
# --------------------------------------------------------------
if(COMMAND cmake_policy)
      cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(alogTools)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_subdirectory(alogTools)
add_subdirectory(bin)



if (false)
  #########################
  # CONFIG SCRIPT CREATION
  #########################

  # Provide a cmake config script for the build directory
  export(TARGETS alogTools FILE UsealogTools.cmake)
  configure_file( "alogToolsConfig.cmake.in" "${PROJECT_BINARY_DIR}/alogToolsConfig.cmake" @ONLY )

  # Stick build location in the central CMake user package registry, so that it may be easily found
  if ("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" GREATER 2.7)
    export(PACKAGE alogTools)
  endif()
endif()