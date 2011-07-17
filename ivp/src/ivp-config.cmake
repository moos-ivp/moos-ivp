################################################################################
#
# Usage:
# ivp-config.cmake is designed to help a CMake-using project to easily find the
#   header files and libraries needed to build an IvP application.
#
# You should use this file be having your CMakeLists.txt file(s) issue the
# command:
#     FIND_PACKAGE(IVP REQUIRED ...)
#
# (If you directly INCLUDE(...) this file into your CMakeLists.txt file, then
# the IVP_LIBS and IVP_HEADER_DIRECTORIES variable might not get set properly.)
#
# If the IvP libraries and headers could *not* be successfully located, then
# this code will set the CMake variable IVP_ALL_FILES_FOUND to FALSE.
# 
# If those headers and libraries *could* be found, then this code does the
# following:
# - Sets IVP_ALL_FILES_FOUND to TRUE.
# - Sets IVP_LIBS to contains a list of the pathnames of the IVP library
#    files.  The idea is that you'll want to eventually call 
#       TARGET_LINK_LIBRARIES(yourApp, ${IVP_LIBS})
#    in your project's CMakeLists.txt file.
# - Sets IVP_HEADER_DIRECTORIES to the set of directories containing the
#    header files needed to use the MOOS libraries.  Then your CMakeLists.txt
#    file(s) would call:
#       INCLUDE_DIRECTORIES(${IVP_HEADER_DIRECTORIES})
#
################################################################################

# IVP_DIR is set to this directory (the one containing ivp-config.cmake) 
# by the same invocation of FIND_PACKAGE that causes this file 
# (ivp-config.cmake) to be processed.

SET(IVP_BASE_DIR "${IVP_DIR}/ivp/src")

################################################################################
# Find libraries and headers...
################################################################################

SET(IVP_ALL_FILES_FOUND TRUE)  # Be optimistic.

#-------------------------------------------------------------------------------
# lib_anrp_util.a
#-------------------------------------------------------------------------------
FIND_LIBRARY(IVP_ANRP_UTIL_LIBRARY
            anrp_util
            PATHS "${IVP_BASE_DIR}/lib_anrp_util"
            DOC "Full path to IvP's libanrp_util.a file."
            NO_DEFAULT_PATH
            )

# Rely on CMake's... "expansive" ... definition of Boolean expressions.  If a
# variable's value is of the form <var-name>"-NOTFOUND", then it's treated as
# TRUE when used in Boolean expressions.  It's insane, but that's exactly what 
# we want here...
IF (${IVP_ANRP_UTIL_LIBRARY})
   SET(IVP_ALL_FILES_FOUND FALSE)
ELSE()
   SET(IVP_LIBS ${IVP_LIBS} ${IVP_ANRP_UTIL_LIBRARY})
ENDIF()

FIND_PATH(IVP_ANRP_UTIL_HEADERS byteorder.h
   "${IVP_BASE_DIR}/lib_anrp_util"
   NO_DEFAULT_PATH
   )

IF (${IVP_ANRP_UTIL_HEADERS})
   SET(IVP_ALL_FILES_FOUND FALSE)
ELSE()
   SET(IVP_HEADER_DIRECTORIES ${IVP_HEADER_DIRECTORIES} ${IVP_ANRP_UTIL_HEADERS})
ENDIF()

#-------------------------------------------------------------------------------
# lib_behaviors.a
#-------------------------------------------------------------------------------
FIND_LIBRARY(IVP_BEHAVIORS_LIBRARY
            behaviors
            PATHS "${IVP_BASE_DIR}/lib_behaviors"
            DOC "Full path to IvP's libbehaviors.a file."
            NO_DEFAULT_PATH
            )

IF (${IVP_BEHAVIORS_LIBRARY})
   SET(IVP_ALL_FILES_FOUND FALSE)
ELSE()
   SET(IVP_LIBS ${IVP_LIBS} ${IVP_BEHAVIORS_LIBRARY})
ENDIF()

FIND_PATH(IVP_BEHAVIORS_HEADERS IvPBehavior.h
   "${IVP_BASE_DIR}/lib_behaviors"
   NO_DEFAULT_PATH
   )

IF (${IVP_BEHAVIORS_HEADERS})
   SET(IVP_ALL_FILES_FOUND FALSE)
ELSE()
   SET(IVP_HEADER_DIRECTORIES ${IVP_HEADER_DIRECTORIES} ${IVP_BEHAVIORS_HEADERS})
ENDIF()

#-------------------------------------------------------------------------------
# lib_geometry.a
#-------------------------------------------------------------------------------
FIND_LIBRARY(IVP_GEOMETRY_LIBRARY
            geometry
            PATHS "${IVP_BASE_DIR}/lib_geometry"
            DOC "Full path to IvP's libgeometry.a file."
            NO_DEFAULT_PATH
            )

IF (${IVP_GEOMETRY_LIBRARY})
   SET(IVP_ALL_FILES_FOUND FALSE)
ELSE()
   SET(IVP_LIBS ${IVP_LIBS} ${IVP_GEOMETRY_LIBRARY})
ENDIF()

FIND_PATH(IVP_GEOMETRY_HEADERS GeomUtils.h
   "${IVP_BASE_DIR}/lib_geometry"
   NO_DEFAULT_PATH
   )

IF (${IVP_GEOMETRY_HEADERS})
   SET(IVP_ALL_FILES_FOUND FALSE)
ELSE()
   SET(IVP_HEADER_DIRECTORIES ${IVP_HEADER_DIRECTORIES} ${IVP_GEOMETRY_HEADERS})
ENDIF()
