PROJECT(IVP)

#===============================================================================
# FINDING MOOS' HEADERS AND LIBRARIES...
#===============================================================================

# *** REPLACE "../../MOOS" WITH WHATEVER VALUE IS REQUIRED.  THE DIRECTORY 
# *** SHOULD HAVE CHILD DIRECTORIES SUCH AS "Core", "Essentials", ETC.
SET(MOOS_BASE_DIR ../../MOOS)


# Using absolute pathnames seems to work better then relative, because you're 
# never sure from which directory a relative path will be employed in commands
# such as LINK_DIRECTORIES() ...
GET_FILENAME_COMPONENT(MOOS_BASE_DIR_ABSOLUTE ../../MOOS ABSOLUTE)

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

SET(CMAKE_CXX_FLAGS "-g")


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

# Tell CMake (and thus C++) where to find IvP's header files...
FILE(GLOB IVP_LIBRARY_DIRS lib_*)
INCLUDE_DIRECTORIES(${IVP_LIBRARY_DIRS})


# This is used to supply a preprocessor definition that gets compiled into some
# code, such as lib_marineview.  I really dislike this approach because it's 
# very brittle w.r.t. anything getting moved around, or if you use the code on
# a different system than that on which it was compiled. -CJC
GET_FILENAME_COMPONENT(IVP_DATA_DIR ../data ABSOLUTE)

#---------------------------------------------------------------------
#  Build the Libraries
#---------------------------------------------------------------------
FILE(GLOB IVP_LIBS_TO_BUILD
  lib_behaviors
  lib_behaviors-ajshafer
  lib_behaviors-colregs
  lib_behaviors-don
  lib_behaviors-henrik
  lib_behaviors-marine
  lib_behaviors-sandbox
  lib_bhvutil
  lib_fltk-1.1.7
  lib_genutil
  lib_geometry
  lib_helmivp
  lib_ipfview
  lib_ivpbuild
  lib_ivpcore
  lib_logic
  lib_marineview
  lib_mbutil
  lib_navplot
  lib_ais
)

FOREACH(L ${IVP_LIBS_TO_BUILD})
  # As a matter of preference, each built IvP library file (.a) should be placed 
  # in its source directory.
  SET( LIBRARY_OUTPUT_PATH "" CACHE PATH "" FORCE )
  ADD_SUBDIRECTORY(${L})
ENDFOREACH(L)

#---------------------------------------------------------------------
#  Build the Applications
#---------------------------------------------------------------------
FILE(GLOB IVP_APPS_TO_BUILD
  app_ffview
  app_geoview
  app_logview
  app_splug
  iMarineSim
  pDeployHandler
  pEchoVar
  pHelmIvP
  pMarinePID
  pMarineViewer
  pShipsideViewer
  pTransponderAIS
  pViewerRelay
  uProcessWatch
  uRepeater
  uTermCommand
  uXMS
  iRawAIS
  iPlaybackAIS
  iParserAIS
  pASCM_PK
)

FOREACH(A ${IVP_APPS_TO_BUILD})
  # Programs get placed in ivp/src/bin/
  SET( EXECUTABLE_OUTPUT_PATH ${IVP_SOURCE_DIR}/bin CACHE PATH "" FORCE )
  ADD_SUBDIRECTORY(${A})
ENDFOREACH(A)


