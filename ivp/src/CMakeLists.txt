# We require CMake 2.6 or higher because there are some syntatic constructs
# we use that aren't supported in earlier versions of CMake. -CJC
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(IVP)

# Force -fPIC because gcc complains when we don't use it with x86_64 code.
# Note sure why: -fPIC should only be needed for shared objects, and
# AFAIK, CMake gets that right when building shared objects. -CJC
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g")

#===============================================================================
# FINDING MOOS' HEADERS AND LIBRARIES...
#===============================================================================

# *** REPLACE "../../MOOS" WITH WHATEVER VALUE IS REQUIRED.  THE DIRECTORY 
# *** SHOULD HAVE CHILD DIRECTORIES SUCH AS "Core", "Essentials", ETC.
IF (${WIN32})
  SET(MOOS_BASE_DIR ../../MOOS-2263-Jul3009)
ELSE (${WIN32})
  SET(MOOS_BASE_DIR ../../MOOS)
ENDIF (${WIN32})

# Using absolute pathnames seems to work better then relative, because you're 
# never sure from which directory a relative path will be employed in commands
# such as LINK_DIRECTORIES() ...
GET_FILENAME_COMPONENT(MOOS_BASE_DIR_ABSOLUTE ${MOOS_BASE_DIR} ABSOLUTE)

# Confirm that MOOS_BASE_DIR seems to be set to a valid value...
IF(NOT EXISTS ${MOOS_BASE_DIR_ABSOLUTE}/Core)
   MESSAGE(FATAL_ERROR 
      "The CMake variable MOOS_BASE_DIR_ABSOLUTE has the value "
      "'${MOOS_BASE_DIR_ABSOLUTE}'.\n\n"
      "That directory doesn't have a 'Core' subdirectory, which means it's "
      "definitely not the base directory of a MOOS source tree."
      "\n\n"
      "You need to set the CMake variable 'MOOS_BASE_DIR' to a different value."
)
ENDIF(NOT EXISTS ${MOOS_BASE_DIR_ABSOLUTE}/Core)

SET(MOOS_LIB_DIR ${MOOS_BASE_DIR_ABSOLUTE}/MOOSBin)
LINK_DIRECTORIES(${MOOS_LIB_DIR})

# No harm in giving all IvP software access to MOOS's include libraries.
SET(MOOS_INCLUDE_DIRS
   ${MOOS_BASE_DIR_ABSOLUTE}/Essentials
   ${MOOS_BASE_DIR_ABSOLUTE}/Core
   ${MOOS_BASE_DIR_ABSOLUTE}/Essentials/MOOSUtilityLib
   ${MOOS_BASE_DIR_ABSOLUTE}/Core/MOOSLIB
   ${MOOS_BASE_DIR_ABSOLUTE}/Core/MOOSGenLib
   )
INCLUDE_DIRECTORIES(${MOOS_INCLUDE_DIRS})
   
#===============================================================================
# BUILDING IvP...
#===============================================================================

#-------------------------------------------------------------------------------
# Force "-Wall" compiler option if using GCC.
#-------------------------------------------------------------------------------
# This was added by Andrew Shafer...
IF(CMAKE_COMPILER_IS_GNUCXX)
   SET( WALL_ON OFF CACHE BOOL "tell me about all compiler warnings (-Wall) ")
   IF(WALL_ON)
            SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
   ENDIF( WALL_ON)
ELSE(CMAKE_COMPILER_IS_GNUCXX)
   IF(MSVC)
      SET( WALL_ON OFF CACHE BOOL "tell me about all compiler warnings (-Wall) ")
      IF(WALL_ON)
         SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
      ENDIF(WALL_ON)
   ELSE(MSVC)
   # Other compilers go here      
   
   ENDIF(MSVC)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

#-------------------------------------------------------------------------------
# Let users control whether or not GUI-related libraries/apps are built...
#-------------------------------------------------------------------------------
SET(IVP_BUILD_GUI_CODE ON CACHE BOOL "Build IvP's GUI-related libraries and apps.")

#-------------------------------------------------------------------------------
# Tell CMake (and thus C++) where to find IvP's header files...
#-------------------------------------------------------------------------------
FILE(GLOB IVP_LIBRARY_DIRS lib_*)
INCLUDE_DIRECTORIES(${IVP_LIBRARY_DIRS})

# If Mac users installed libtiff.a using 'fink', it will likely appear under
# '/sw', which current versions of CMake don't include in their default
# search paths.
IF (${APPLE})
   INCLUDE_DIRECTORIES(/sw/include)
   LINK_DIRECTORIES(/sw/lib)
ENDIF(${APPLE})

#---------------------------------------------------------------------
#  Build the Libraries
#---------------------------------------------------------------------

SET(IVP_NON_GUI_LIBS
   lib_behaviors
   lib_behaviors-marine
   lib_bhvutil
   lib_genutil
   lib_geometry
   lib_helmivp
   lib_ivpbuild
   lib_ivpcore
   lib_logic
   lib_logutils
   lib_mbutil
   lib_navplot
   lib_newmat10D
   )


IF (${WIN32})
   # Set the Non-IVP GUI libraries for Windows
   SET(IVP_GUI_LIBS
      lib_ipfview
      lib_marineview
      lib_fltk-1.1.7
      lib_tiff
      )
ELSE (${WIN32})
   # Linux, Mac
   SET(IVP_GUI_LIBS
      lib_ipfview
      lib_marineview
      lib_fltk-1.1.7
      )
ENDIF (${WIN32})

SET(IVP_LIBS_TO_BUILD ${IVP_NON_GUI_LIBS})
IF("${IVP_BUILD_GUI_CODE}" STREQUAL "ON")
   LIST(APPEND IVP_LIBS_TO_BUILD ${IVP_GUI_LIBS})
ENDIF()

# As a matter of preference, each built IvP library file (.a) should be placed 
# in its source directory.
SET( LIBRARY_OUTPUT_PATH "" CACHE PATH "" FORCE )

FOREACH(L ${IVP_LIBS_TO_BUILD} )
  ADD_SUBDIRECTORY(${L})
  # We're actually fighting the lib_fltk-1.1.7 CMake files for control over this
  # detail, so we have to reset it after adding the FLTK subdirectory.
  SET( LIBRARY_OUTPUT_PATH "" CACHE PATH "" FORCE )
ENDFOREACH(L)

#---------------------------------------------------------------------
#  Build the Applications
#---------------------------------------------------------------------
  
SET(IVP_NON_GUI_APPS
  app_alogscan
  app_aloggrep
  app_alogrm
  app_alogclip
  app_splug
  app_nsplug
  app_domquery
  iMarineSim
  pEchoVar
  pXRelay
  pHelmIvP
  pMarinePID
  pTransponderAIS
  pNodeReporter
  uTimerScript
  uHelmScope
  uProcessWatch
  uTermCommand
  uXMS
  uPokeDB
  )

SET(IVP_GUI_APPS
  app_ffview
  app_geoview
  app_logview
  pMarineViewer
  uFunctionVis
  )
  
SET(IVP_APPS_TO_BUILD ${IVP_NON_GUI_APPS})
IF("${IVP_BUILD_GUI_CODE}" STREQUAL "ON")
   LIST(APPEND IVP_APPS_TO_BUILD ${IVP_GUI_APPS})
ENDIF("${IVP_BUILD_GUI_CODE}" STREQUAL "ON")



FOREACH(A ${IVP_APPS_TO_BUILD})
  # Programs get placed in ivp/src/bin/
  SET( EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "" FORCE )
  ADD_SUBDIRECTORY(${A})
ENDFOREACH(A)
