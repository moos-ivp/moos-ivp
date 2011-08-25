################################################################################
#
# Usage:
# FindMOOS.cmake is designed to help a CMake-using project to easily find the
#   header files and libraries needed to build a MOOS application.
#
# You should use this file be having your CMakeLists.txt file(s) issue the
# command:
#     FIND_PACKAGE(MOOS REQUIRED ...)
#
# (If you directly INCLUDE(...) this file into your CMakeLists.txt file, then
# the MOOS_LIBS variable might not get set properly.)
#
# If the MOOS libraries and headers could *not* be successfully located, then
# this code will set the CMake variable MOOS_ALL_FILES_FOUND to FALSE.
# 
# If those headers and libraries *could* be found, then this code does the
# following:
# - Sets MOOS_ALL_FILES_FOUND to TRUE.
# - Sets MOOS_LIBS to contains a list of the pathnames of the MOOS library
#    files.  The idea is that you'll want to eventually call 
#       TARGET_LINK_LIBRARIES(yourApp, ${MOOS_LIBS})
#    in your project's CMakeLists.txt file.
# - Sets MOOS_HEADER_DIRECTORIES to the set of directories containing the
#    header files needed to use the MOOS libraries.  Then your CMakeLists.txt
#    file(s) would call:
#       INCLUDE_DIRECTORIES(${MOOS_HEADER_DIRECTORIES})
#
################################################################################

# MOOS_DIR is set to this directory (the one containing moos-config.cmake) 
# by the same invocation of FIND_PACKAGE that causes this file 
# (moos-config.cmake) to be processed.

SET(MOOS_BASE_DIR "${MOOS_DIR}/../../MOOS")

################################################################################
# Find library and header directory for libMOOS.a
################################################################################
FIND_LIBRARY(MOOS_LIBRARY
            MOOS
            PATHS "${MOOS_BASE_DIR}/MOOSBin"
            DOC "Full path to MOOS's libMOOS.a file."
            NO_DEFAULT_PATH
            )
            
FIND_PATH(MOOS_LIB_HEADERS MOOSLib.h
   "${MOOS_BASE_DIR}/Core/MOOSLIB"
   NO_DEFAULT_PATH
   )

################################################################################
# Find library and header directory for libMOOSUtility.a
################################################################################
FIND_LIBRARY(MOOS_UTILITY_LIBRARY 
            NAMES MOOSUtility
            PATHS "${MOOS_BASE_DIR}/MOOSBin"
            DOC "Full path to MOOS's libMOOSUtility.a file."
            NO_DEFAULT_PATH
            )
            
FIND_PATH(MOOS_UTILITY_LIB_HEADERS MOOSGeodesy.h
   "${MOOS_BASE_DIR}/Essentials/MOOSUtilityLib"
   NO_DEFAULT_PATH
   )

################################################################################
# Find library and header directory for libMOOSGen.a
################################################################################
FIND_LIBRARY(MOOS_GEN_LIBRARY
            NAMES MOOSGen
            PATHS "${MOOS_BASE_DIR}/MOOSBin"
            DOC "Full path to MOOS's libMOOSGen.a file."
            NO_DEFAULT_PATH
            )

FIND_PATH(MOOS_GEN_LIB_HEADERS MOOSGenLib.h
   "${MOOS_BASE_DIR}/Core/MOOSGenLib"
   NO_DEFAULT_PATH
   )


################################################################################
# Consolidate the variables for the users' convenience...
################################################################################
SET(MOOS_LIBS 
   ${MOOS_LIBRARY}
   ${MOOS_UTILITY_LIBRARY}
   ${MOOS_GEN_LIBRARY}
   )
   
SET(MOOS_HEADER_DIRECTORIES
   ${MOOS_LIB_HEADERS}
   ${MOOS_UTILITY_LIB_HEADERS}
   ${MOOS_GEN_LIB_HEADERS}
   # Some MOOS code has #include's relative to these directories...
   "${MOOS_BASE_DIR}/Core"
   "${MOOS_BASE_DIR}/Essentials"
   )

# Rely on CMake's... "expansive" ... definition of Boolean expressions.  If a
# variable's value is of the form <var-name>"-NOTFOUND", then it's treated as
# TRUE when used in Boolean expressions.  It's insane, but that's exactly what 
# we want here...
SET(MOOS_ALL_FILES_FOUND TRUE)

IF (${MOOS_LIBRARY})
   SET(MOOS_ALL_FILES_FOUND FALSE)
ENDIF(${MOOS_LIBRARY})

IF (${MOOS_UTILITY_LIBRARY})
   SET(MOOS_ALL_FILES_FOUND FALSE)
ENDIF(${MOOS_UTILITY_LIBRARY})

IF (${MOOS_GEN_LIBRARY})
   SET(MOOS_ALL_FILES_FOUND FALSE)
ENDIF(${MOOS_GEN_LIBRARY})

IF (${MOOS_LIB_HEADERS})
   SET(MOOS_ALL_FILES_FOUND FALSE)
ENDIF(${MOOS_LIB_HEADERS})

IF (${MOOS_UTILITY_LIB_HEADERS})
   SET(MOOS_ALL_FILES_FOUND FALSE)
ENDIF(${MOOS_UTILITY_LIB_HEADERS})

IF (${MOOS_GEN_LIB_HEADERS})
   SET(MOOS_ALL_FILES_FOUND FALSE)
ENDIF(${MOOS_GEN_LIB_HEADERS})
