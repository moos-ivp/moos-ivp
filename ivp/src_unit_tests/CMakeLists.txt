CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(UTX)

IF (${WIN32})
  # Define Windows Compiler flags here
  SET(CMAKE_CXX_FLAGS " ")
ELSE (${WIN32})
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
ENDIF (${WIN32})

INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

IF(COMPILER_SUPPORTS_CXX11)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  ADD_DEFINITIONS(-D_USE_UNIQUE_PTR)
ELSEIF(COMPILER_SUPPORTS_CXX0X)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  ADD_DEFINITIONS(-D_USE_UNIQUE_PTR)
ELSE()
  MESSAGE(STATUS "${CMAKE_CXX_COMPILER} has no C++11 support.")
  REMOVE_DEFINITIONS(-D_USE_UNIQUE_PTR)
ENDIF()


INCLUDE_DIRECTORIES(
	../src/lib_mbutil
	../src/lib_geometry)

LINK_DIRECTORIES(../../lib)


#=====================================================================
#  Build the list of Apps to be built
#=====================================================================

SET(APPS
  utest
  testIncIntString
  testLineCircleIntPts
  testSegCircleIntPts
  testArcSegCross
  testArcSegCrossPts
  testDistPointOnArc
  testDistPointToArc
  testDistPointToArcPt
  testDistPtsOnArc
  testDistSegToArc
  testAngleDiff
  testAngleInArc
  testSegRayCPA
  testDistPointToRay
  testCpasRaySegl
  testCpasArcSegl
  )

message(" Apps to be built: ${APPS}")

SET(UTX_APPS_TO_BUILD ${APPS})

FOREACH(A ${UTX_APPS_TO_BUILD})
  SET( EXECUTABLE_OUTPUT_PATH "${UTX_BIN_DIRECTORY}" CACHE PATH "" FORCE )
  ADD_SUBDIRECTORY(${A})
ENDFOREACH(A)

