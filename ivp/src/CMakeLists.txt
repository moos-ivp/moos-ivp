CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(IVP)

#======================================================================
# Part 1: Set Compiler Flags
#======================================================================
IF(${WIN32})
  # Define Windows Compiler flags here
  SET(CMAKE_CXX_FLAGS " ")
ELSE()
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
ENDIF()

# Added by mikerb July 30th, 2022, silence harmless warnings on unix blds
# compiler flag not recognize in Apple machines
IF(UNIX AND NOT APPLE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-cast-function-type ")
ENDIF()

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

#======================================================================
# Part 2: FINDING MOOS' HEADERS AND LIBRARIES...
#======================================================================
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

#=====================================================================
# Part 3: FINDING MOOSGeodesy' HEADERS AND LIBRARIES...  
#=====================================================================
# moos - http://www.robots.ox.ac.uk/~mobile/MOOS/wiki/pmwiki.php            
find_package(MOOSGeodesy)
include_directories(${MOOSGeodesy_INCLUDE_DIRS})
#link_directories(${MOOSGeodesy_LIBRARY_PATH})

message("+++++++++++++++++++++++++++++++++++++++++")
message("MOOSGeodesy_INCLUDE_DIRS:" ${MOOSGeodesy_INCLUDE_DIRS})
message("MOOSGeodesy_LIBRARY_PATH:" ${MOOSGeodesy_LIBRARY_PATH})
message("+++++++++++++++++++++++++++++++++++++++++")

#=============================================================================
# Part 4: Set defaults and expose user build options GUI, MinRobot, UTM
#=============================================================================
SET(IVP_BUILD_GUI_CODE ON CACHE BOOL "Build IvP's GUI-related libs and apps.")
SET(IVP_BUILD_BOT_CODE_ONLY OFF CACHE BOOL "Build IvP's min set of bot apps.")
SET(USE_UTM ON CACHE BOOL "Use UTM coordinates vs Local coordinates.")

IF("${USE_UTM}" STREQUAL "ON")
  ADD_DEFINITIONS(-DUSE_UTM)
ENDIF()

message("----------------------------------------Use UTM:" ${USE_UTM})


#=====================================================================
# Part 5: Tell CMake/C++ where to find libs/headers from packages
#=====================================================================
IF (${APPLE})
  # If Mac users installed libtiff using 'MacPorts', it may appear in
  # '/opt/local' directory, which is not in the CMake search path.
  IF(EXISTS /opt/local/include AND EXISTS /opt/local/lib)
    message(STATUS "Detected use of 'MacPorts' for dependencies")
    INCLUDE_DIRECTORIES(/opt/local/include)
    LINK_DIRECTORIES(/opt/local/lib)
  ENDIF()
  
  IF(EXISTS /opt/homebrew/include AND EXISTS /opt/homebrew/lib)
    message(STATUS "Detected use of 'HomeBrew' for dependencies")
    INCLUDE_DIRECTORIES(/opt/homebrew/include)
    LINK_DIRECTORIES(/opt/homebrew/lib)
  ENDIF()
  
  # If Mac users installed libtiff using 'fink', it will likely appear
  # under the '/sw' directory, which is not in the CMake search path.
  IF(EXISTS /sw/include AND EXISTS /sw/lib )
    message(STATUS "Detected use of 'fink' for dependencies")
    INCLUDE_DIRECTORIES(/sw/include)
    LINK_DIRECTORIES(/sw/lib)
  ENDIF()
  
  # If Mac users installed libtiff using 'Homebrew', it may appear in
  # '/opt/local/Cellar' directory, which is not in CMake search path.
  IF(EXISTS /usr/local/Cellar)
    message(STATUS "Detected use of 'Homebrew' for dependencies")
    INCLUDE_DIRECTORIES(/usr/local/include)
    LINK_DIRECTORIES(/usr/local/lib)
  ENDIF()
  
ENDIF()


#=====================================================================
# Part 6: Ensure IVP_, LIB_, and BIN_DIRECTORY set properly
#=====================================================================
IF("${IVP_LIB_DIRECTORY}" STREQUAL "")
  # IVP_SOURCE_DIRECTORY defined by CMake, since project name is IVP
  GET_FILENAME_COMPONENT(IVP_LIB_DIRECTORY "${IVP_SOURCE_DIR}/../../lib" ABSOLUTE)
ENDIF()

IF("${IVP_INC_DIRECTORY}" STREQUAL "")
  # IVP_SOURCE_DIRECTORY defined by CMake, since project name is IVP
  GET_FILENAME_COMPONENT(IVP_INC_DIRECTORY "${IVP_SOURCE_DIR}/../../include" ABSOLUTE)
ENDIF()

IF("${IVP_BIN_DIRECTORY}" STREQUAL "")
  # IVP_SOURCE_DIRECTORY defined by CMake, since project name is IVP
  GET_FILENAME_COMPONENT(IVP_BIN_DIRECTORY "${IVP_SOURCE_DIR}/../../bin" ABSOLUTE)
ENDIF()

message("+++++++++++++++++++++++++++++++++++++++++")
message("IVP_LIB_DIRECTORY:" ${IVP_LIB_DIRECTORY})
message("IVP_INC_DIRECTORY:" ${IVP_INC_DIRECTORY})
message("IVP_BIN_DIRECTORY:" ${IVP_BIN_DIRECTORY})
message("+++++++++++++++++++++++++++++++++++++++++")

#=====================================================================
# Part 7: Set Library/App categories and membership
#=====================================================================
SET(IVP_NON_GUI_LIBS
  lib_behaviors      lib_bhvutil         lib_behaviors-marine     
  lib_genutil        lib_geometry        lib_behaviors-colregs
  lib_polar          lib_helmivp         lib_ivpbuild
  lib_ivpsolve       lib_logic           lib_ivpcore
  lib_evalutil       lib_encounters      lib_apputil  
  lib_contacts       lib_obstacles       lib_realm
  lib_ufield         lib_logutils        lib_mbutil
  lib_manifest       lib_marine_pid      lib_turngeo
  lib_bhv_fixedturn  lib_dep_behaviors
)
SET(IVP_GUI_LIBS
  lib_ipfview        lib_marineview
  lib_zaicview       lib_ucommand
)
SET(ROBOT_APPS
  app_alogpick       app_nzero           app_projpt
  app_alogbin        app_alogavg         app_alogtm
  app_alogeval       app_alogload        app_aloggrep
  app_alogscan       app_alogcd          app_alogpare
  app_alogeplot      app_alogrm          app_alogiter
  app_alogcat        app_alogclip        app_aloghelm
  app_nsplug         app_pickpos         app_manifest_test
  app_tagrep         app_gen_moos_app    app_alogmhash
  pRealm             pEchoVar            pHelmIvP
  pDeadManPost       pNodeReporter       pObstacleMgr
  uFldNodeBroker     uHelmScope          uFldMessageHandler
  uTimerScript       uProcessWatch       uMemWatch
  uLoadWatch         uTermCommand        uXMS
  uMAC               uPokeDB             uQueryDB
  pHostInfo          iSay                pContactMgrV20
  pMarinePIDV22      uSimMarineV22       uSimMarineV23
  pMissionHash       pMissionEval        iBlinkStick
)
SET(IVP_NON_GUI_APPS
  app_alogsplit      app_alogsort        app_alogcheck
  app_alogtest       app_gen_obstacles   app_bhv2graphviz
  pXRelay            uFldCollObDetect    uFldCollisionDetect
  uFldPathCheck      uFldShoreBroker     uFldObstacleSim
  uFldScope          uFldNodeComms       uFldBeaconRangeSensor
  pSearchGrid        uFldGenericSensor   uFldContactRangeSensor
  uFldDelve          app_bweb            app_mhash_gen
  app_projfield      pMapMarkers
)
SET(IVP_GUI_APPS
  app_ffview         app_geoview         app_alogview
  app_zaic_hleq      app_zaic_vect       app_zaic_hdg
  app_zaic_spd       app_zaic_peak       pMarineViewer
  uFunctionVis       uMACView            uPlotViewer
  uCommand           
# app_vzaicview      app_gzaicview
)

# We also consider the below to be robot apps just in case needed
SET(IVP_NON_GUI_APPS_DEPRECATED
  dep_pBasicContactMgr
  dep_uSimMarine
  dep_pMarinePID
)

message(" IVP_NON_GUI_LIBS: ${IVP_NON_GUI_LIBS}")
message(" IVP_GUI_LIBS: ${IVP_GUI_LIBS}")
message(" ROBOT_APPS: ${ROBOT_APPS}")
message(" IVP_NON_GUI_APPS: ${IVP_NON_GUI_APPS}")
message(" IVP_GUI_APPS: ${IVP_GUI_APPS}")
message(" IVP_NON_GUI_APPS_DEP: ${IVP_NON_GUI_APPS_DEPRECATED}")

#=====================================================================
# Part 8: Set actual libs/apps to build based on user preferences
#=====================================================================
SET(IVP_LIBS_TO_BUILD ${IVP_NON_GUI_LIBS})
SET(IVP_APPS_TO_BUILD ${ROBOT_APPS})

message("+++++++++++++++++++++++++++++++++++++++++")
message(" IVP_BUILD_BOT_CODE_ONLY: ${IVP_BUILD_BOT_CODE_ONLY}")
message("+++++++++++++++++++++++++++++++++++++++++")

#IF("${IVP_BUILD_BOT_CODE_ONLY}" STREQUAL "OFF")
LIST(APPEND IVP_APPS_TO_BUILD ${IVP_NON_GUI_APPS})
#ENDIF()

IF("${IVP_BUILD_GUI_CODE}" STREQUAL "ON")
  LIST(APPEND IVP_LIBS_TO_BUILD ${IVP_GUI_LIBS})  
  LIST(APPEND IVP_APPS_TO_BUILD ${IVP_GUI_APPS})
  message("+++++++++++++++++++++++++++++++++++++++++")
  message("IVP_GUI_CODE will be built......         ")
  message("+++++++++++++++++++++++++++++++++++++++++")
ENDIF()

LIST(APPEND IVP_APPS_TO_BUILD ${IVP_NON_GUI_APPS_DEPRECATED})


INCLUDE_DIRECTORIES(${IVP_LIBS_TO_BUILD})

#=====================================================================
# Part 9: Add CMake subdirs for each library and application
#=====================================================================
FOREACH(L ${IVP_LIBS_TO_BUILD} )
  SET( LIBRARY_OUTPUT_PATH "${IVP_LIB_DIRECTORY}" CACHE PATH "" FORCE ) 
  ADD_SUBDIRECTORY(${L})
ENDFOREACH(L)

FOREACH(A ${IVP_APPS_TO_BUILD})
  SET( EXECUTABLE_OUTPUT_PATH "${IVP_BIN_DIRECTORY}" CACHE PATH "" FORCE )
  ADD_SUBDIRECTORY(${A})
ENDFOREACH(A)

FOREACH(A ${IVP_APPS_TO_BUILD_DEPRECATED})
  SET( EXECUTABLE_OUTPUT_PATH "${IVP_BIN_DIRECTORY}" CACHE PATH "" FORCE )
  ADD_SUBDIRECTORY(${A})
ENDFOREACH(A)

#=====================================================================
# make install (tes 2012-05-08)
#=====================================================================

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
