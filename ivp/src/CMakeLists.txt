CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(IVP)

IF (${WIN32})
  # Define Windows Compiler flags here
  SET(CMAKE_CXX_FLAGS " ")
ELSE (${WIN32})
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
ENDIF (${WIN32})

#===============================================================================
# FINDING MOOS' HEADERS AND LIBRARIES...
#===============================================================================

# moos - http://www.robots.ox.ac.uk/~mobile/MOOS/wiki/pmwiki.php
find_package(MOOS 10.0)

message("+++++++++++++++++++++++++++++++++++++++++")
message("FIND_PACKAGE CONSIDERED:" ${MOOS_CONSIDERED_CONFIGS})
message("+++++++++++++++++++++++++++++++++++++++++")


if(NOT DEFINED MOOS_LIBRARIES)
  message("Defining MOOS_LIBRARIES")
  set(MOOS_LIBRARIES MOOS)
endif()

include_directories(${MOOS_INCLUDE_DIRS})

message("+++++++++++++++++++++++++++++++++++++++++")
message("MOOS_INCLUDE_DIRS:" ${MOOS_INCLUDE_DIRS})
message("MOOS_LIBRARIES:   " ${MOOS_LIBRARIES})
message("+++++++++++++++++++++++++++++++++++++++++")

#===============================================================================
# FINDING MOOSGeodesy' HEADERS AND LIBRARIES...  
#===============================================================================
# moos - http://www.robots.ox.ac.uk/~mobile/MOOS/wiki/pmwiki.php            
find_package(MOOSGeodesy)
include_directories(${MOOSGeodesy_INCLUDE_DIRS})
link_directories(${MOOSGeodesy_LIBRARY_PATH})

message("+++++++++++++++++++++++++++++++++++++++++")
message("MOOSGeodesy_INCLUDE_DIRS:" ${MOOSGeodesy_INCLUDE_DIRS})
message("MOOSGeodesy_LIBRARY_PATH:" ${MOOSGeodesy_LIBRARY_PATH})
message("+++++++++++++++++++++++++++++++++++++++++")

#===============================================================================
# BUILDING IvP...
#===============================================================================

#-------------------------------------------------------------------------------
# Let users control whether or not GUI-related libraries/apps are built...
#-------------------------------------------------------------------------------
SET(IVP_BUILD_GUI_CODE ON CACHE BOOL "Build IvP's GUI-related libraries and apps.")
SET(IVP_BUILD_BOT_CODE_ONLY OFF CACHE BOOL "Build IvP's minimal set of bot apps.")

#-------------------------------------------------------------------------------
# Let users control whether or UTM or Local Coords are used
#-------------------------------------------------------------------------------
SET(USE_UTM ON CACHE BOOL "Use UTM coordinates vs Local coordinates.")

#-------------------------------------------------------------------------------
# Tell CMake (and thus C++) where to find IvP's header files...
#-------------------------------------------------------------------------------
IF (${APPLE})

   # If Mac users installed libtiff using 'MacPorts', it will likely appear
   # under the '/opt/local' directory, which is not in the CMake search path.
   IF( EXISTS /opt/local/include AND EXISTS /opt/local/lib )
      INCLUDE_DIRECTORIES(/opt/local/include)
      LINK_DIRECTORIES(/opt/local/lib)
   ENDIF( EXISTS /opt/local/include AND EXISTS /opt/local/lib )

   # If Mac users installed libtiff using 'fink', it will likely appear
   # under the '/sw' directory, which is not in the CMake search path.
   IF( EXISTS /sw/include AND EXISTS /sw/lib )
      INCLUDE_DIRECTORIES(/sw/include)
      LINK_DIRECTORIES(/sw/lib)
   ENDIF( EXISTS /sw/include AND EXISTS /sw/lib )

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
   lib_ivpsolve
   lib_logic
   lib_apputil
   lib_contacts
   lib_ufield
   lib_ufld_hazards
   lib_logutils
   lib_mbutil
   )

IF (${WIN32})
  SET(TIFF_SRC_DIR lib_tiff/tiff-3.8.2/libtiff)
  # For Windows use the preconfigure headers provided by libtiff
  # Use CONFIGURE_FILE to copy the supplied files to their approtiate
  # locations.
  CONFIGURE_FILE(${TIFF_SRC_DIR}/tif_config.h.vc 
    ${TIFF_SRC_DIR}/tif_config.h COPYONLY)
  CONFIGURE_FILE(${TIFF_SRC_DIR}/tiffconf.h.vc 
    ${TIFF_SRC_DIR}/tiffconf.h COPYONLY)
  # Add the libtiff headers directory
  INCLUDE_DIRECTORIES( AFTER ${TIFF_SRC_DIR} )
  # Set the IVP GUI libraries for Windows
  SET(IVP_GUI_LIBS lib_ipfview lib_marineview lib_tiff)
  
ELSE (${WIN32})
  # Linux, Mac
  SET(IVP_GUI_LIBS lib_ipfview lib_marineview)
ENDIF (${WIN32})


SET(IVP_LIBS_TO_BUILD ${IVP_NON_GUI_LIBS})

IF("${IVP_BUILD_GUI_CODE}" STREQUAL "ON")
   LIST(APPEND IVP_LIBS_TO_BUILD ${IVP_GUI_LIBS})

   message("+++++++++++++++++++++++++++++++++++++++++")
   message("IVP_GUI_CODE will be built......         ")
   message("+++++++++++++++++++++++++++++++++++++++++")

ENDIF("${IVP_BUILD_GUI_CODE}" STREQUAL "ON")

IF("${USE_UTM}" STREQUAL "ON")
   ADD_DEFINITIONS(-DUSE_UTM)
   message("USING UTM")
ENDIF("${USE_UTM}" STREQUAL "ON")


# If the environment hasn't explicitly set 'IVP_LIB_DIRECTORY', give it a good
# default value...
IF("${IVP_LIB_DIRECTORY}" STREQUAL "")
   # IVP_SOURCE_DIRECTORY is defined by CMake, because this project's name is
   # IVP.
   GET_FILENAME_COMPONENT(IVP_LIB_DIRECTORY "${IVP_SOURCE_DIR}/../../lib" ABSOLUTE)
ENDIF("${IVP_LIB_DIRECTORY}" STREQUAL "")

IF("${IVP_INC_DIRECTORY}" STREQUAL "")
   # IVP_SOURCE_DIRECTORY is defined by CMake, because this project's name is IVP
   GET_FILENAME_COMPONENT(IVP_INC_DIRECTORY "${IVP_SOURCE_DIR}/../../include" ABSOLUTE)
ENDIF("${IVP_INC_DIRECTORY}" STREQUAL "")


message("+++++++++++++++++++++++++++++++++++++++++")
message("IVP_LIB_DIRECTORY:" ${IVP_LIB_DIRECTORY})
message("+++++++++++++++++++++++++++++++++++++++++")


INCLUDE_DIRECTORIES(${IVP_LIBS_TO_BUILD})

FOREACH(L ${IVP_LIBS_TO_BUILD} )
  SET( LIBRARY_OUTPUT_PATH "${IVP_LIB_DIRECTORY}" CACHE PATH "" FORCE ) 
  ADD_SUBDIRECTORY(${L})
ENDFOREACH(L)


#=====================================================================
#  Build the list of Apps to be built
#=====================================================================

#---------------------------------------------------------------------
# Part 1: ROBOT_APPS
#         Minimal set of apps for running on a robot (with slower CPU)
#---------------------------------------------------------------------

SET(ROBOT_APPS
  app_aloggrep
  app_alogscan
  app_alogrm
  app_alogiter
  app_alogclip
  app_aloghelm
  app_nsplug
  pEchoVar
  pHelmIvP
  pMarinePID
  pBasicContactMgr
  pDeadManPost
  pNodeReporter
  uFldMessageHandler
  uFldNodeBroker
  uHelmScope
  uTimerScript
  uProcessWatch
  uLoadWatch
  uTermCommand
  uXMS
  uMAC
  uPokeDB
  pHostInfo
  iSay
  )

#---------------------------------------------------------------------
# pSafetyRetreat is excluded in moos-ivp-aro downloads to check if the 
# source code is present before adding it to the build list.
IF( EXISTS pSafetyRetreat )
  LIST(APPEND ROBOT_APPS pSafetyRetreat
)  
ENDIF( EXISTS pSafetyRetreat )


#---------------------------------------------------------------------
# Part 2 of 3: IVP_NON_GUI_APPS
#              Larger set of apps but still, but still short of GUI apps.
#---------------------------------------------------------------------

SET(IVP_NON_GUI_APPS
  app_alogsplit
  app_alogsort
  app_alogcheck
  app_gen_hazards
  app_bhv2graphviz
  pXRelay
  uFldCollisionDetect
  uFldPathCheck
  uFldShoreBroker
  uFldScope
  uFldNodeComms
  uFldBeaconRangeSensor
  uFldContactRangeSensor
  uFldHazardSensor
  uFldHazardMgr
  uFldHazardMetric
  uFldGenericSensor
  uFldWrapDetect
  pSearchGrid
  uSimMarine
  )

#---------------------------------------------------------------------
# uSimCurrent is excluded in moos-ivp-aro downloads to check if the 
# source code is present before adding it to the build list.
IF( EXISTS uSimCurrent )
  LIST(APPEND IVP_NON_GUI_APPS uSimCurrent)  
ENDIF( EXISTS uSimCurrent )


#---------------------------------------------------------------------
# Part 3 of 3: IVP_GUI_APPS
#              Include on the GUI applications
#---------------------------------------------------------------------

SET(IVP_GUI_APPS
  app_ffview
  app_geoview
  app_alogview
  app_vzaicview
  app_gzaicview
  pMarineViewer
  uFunctionVis
  uMACView
  uPlotViewer
  )

SET(IVP_APPS_TO_BUILD ${ROBOT_APPS})

message("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")
message(" IVP_BUILD_BOT_CODE_ONLY: ${IVP_BUILD_BOT_CODE_ONLY}")
message("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++")

IF("${IVP_BUILD_BOT_CODE_ONLY}" STREQUAL "OFF")
  LIST(APPEND IVP_APPS_TO_BUILD ${IVP_NON_GUI_APPS})
ENDIF("${IVP_BUILD_BOT_CODE_ONLY}" STREQUAL "OFF")


IF("${IVP_BUILD_GUI_CODE}" STREQUAL "ON")
   LIST(APPEND IVP_APPS_TO_BUILD ${IVP_GUI_APPS})
ENDIF("${IVP_BUILD_GUI_CODE}" STREQUAL "ON")


# If the environment hasn't explicitly set 'IVP_BIN_DIRECTORY', give it a good
# default value...
IF("${IVP_BIN_DIRECTORY}" STREQUAL "")
   # IVP_SOURCE_DIRECTORY is defined by CMake, because this project's name is
   # IVP.
   GET_FILENAME_COMPONENT(IVP_BIN_DIRECTORY "${IVP_SOURCE_DIR}/../../bin" ABSOLUTE)
ENDIF("${IVP_BIN_DIRECTORY}" STREQUAL "")

FOREACH(A ${IVP_APPS_TO_BUILD})
  SET( EXECUTABLE_OUTPUT_PATH "${IVP_BIN_DIRECTORY}" CACHE PATH "" FORCE )
  ADD_SUBDIRECTORY(${A})
ENDFOREACH(A)

#=============================================================================
# make install (tes 2012-05-08)
#=============================================================================

install(DIRECTORY ${IVP_BIN_DIRECTORY}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/bin 
  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE )
install(DIRECTORY ${IVP_LIB_DIRECTORY}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

message("Include directory: ${IVP_INC_DIRECTORY}")
file(GLOB_RECURSE INCLUDE_FILES RELATIVE ${IVP_SOURCE_DIR} lib_*/*.h)
foreach(I ${INCLUDE_FILES})
  if(BUILD_LOCAL_FLTK OR (NOT ${I} MATCHES .*lib_fltk.*))
    get_filename_component(I_NAMEONLY ${I} NAME)
    configure_file(${IVP_SOURCE_DIR}/${I} ${IVP_INC_DIRECTORY}/ivp/${I_NAMEONLY} COPYONLY)
  endif()
endforeach()

install(DIRECTORY ${IVP_INC_DIRECTORY}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  FILES_MATCHING REGEX .*\\.h)
