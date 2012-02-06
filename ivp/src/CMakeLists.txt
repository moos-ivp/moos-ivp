# We require CMake 2.6 or higher because there are some syntatic constructs
# we use that aren't supported in earlier versions of CMake. -CJC
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(IVP)

ENABLE_TESTING()
INCLUDE(CTest)

IF (${WIN32})
  # Define Windows Compiler flags here
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
ELSE (${WIN32})

  # Force -fPIC because gcc complains when we don't use it with x86_64 code.
  # Note sure why: -fPIC should only be needed for shared objects, and
  # AFAIK, CMake gets that right when building shared objects. -CJC
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -g")
  IF( ${APPLE} )
    # FLTK 1.3.0 is supported for 64 bit on OS X so we no longer force 32bit
    # as done previously. Below commented-out lines left just for clarity
    #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    #SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")

  ENDIF( ${APPLE} )

ENDIF (${WIN32})

#===============================================================================
# FINDING MOOS' HEADERS AND LIBRARIES...
#===============================================================================

# *** REPLACE "../../MOOS" WITH WHATEVER VALUE IS REQUIRED.  THE DIRECTORY 
# *** SHOULD HAVE CHILD DIRECTORIES SUCH AS "Core", "Essentials", ETC.
IF (${WIN32})
  # Windows does not have symbolic links. To handle this, subversion creates 
  # a text file that represents the symbolic link with the destination of the 
  # link inside the file. Here we extract the destination of the MOOS directory 
  # link and use it set a variable.
  FILE(READ ../../MOOS MOOS_DIR OFFSET 5)
  SET(MOOS_BASE_DIR ../../${MOOS_DIR})
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
   SET( WALL_ON ON CACHE BOOL "tell me about all compiler warnings (-Wall) ")
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
   lib_logic
   lib_contacts
   lib_ufield
   lib_logutils
   lib_mbutil
   lib_navplot
   lib_ulogview
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
   SET(IVP_GUI_LIBS
      lib_ipfview
      lib_marineview
      lib_fltk-1.3.0
      lib_tiff
      )
ELSE (${WIN32})
   # Linux, Mac
   SET(IVP_GUI_LIBS
      lib_ipfview
      lib_marineview
      lib_fltk-1.3.0
      )
ENDIF (${WIN32})

SET(IVP_LIBS_TO_BUILD ${IVP_NON_GUI_LIBS})
IF("${IVP_BUILD_GUI_CODE}" STREQUAL "ON")
   LIST(APPEND IVP_LIBS_TO_BUILD ${IVP_GUI_LIBS})
ENDIF("${IVP_BUILD_GUI_CODE}" STREQUAL "ON")


# If the environment hasn't explicitly set 'IVP_LIB_DIRECTORY', give it a good
# default value...
IF("${IVP_LIB_DIRECTORY}" STREQUAL "")
   # IVP_SOURCE_DIRECTORY is defined by CMake, because this project's name is
   # IVP.
   GET_FILENAME_COMPONENT(IVP_LIB_DIRECTORY "${IVP_SOURCE_DIR}/../../lib" ABSOLUTE)
ENDIF("${IVP_LIB_DIRECTORY}" STREQUAL "")

FOREACH(L ${IVP_LIBS_TO_BUILD} )
# message("D: ${L} : IVP_LIB_DIRECTORY = ${IVP_LIB_DIRECTORY}")
  # We're actually fighting the lib_fltk CMake files for control over this
  # detail, so we have to reset it after adding the FLTK subdirectory.
  
  SET( LIBRARY_OUTPUT_PATH "${IVP_LIB_DIRECTORY}" CACHE PATH "" FORCE )  # For CMake 2.4

  # Just setting the variables without the 'CMAKE_' prefix doesn't seem to work.
  # This whole mess should go away if/when we stop building FLTK as a subproject.
  SET( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${IVP_LIB_DIRECTORY}" CACHE PATH "" FORCE ) # For CMake 2.6
  SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${IVP_LIB_DIRECTORY}" CACHE PATH "" FORCE ) # For CMake 2.6
  SET( ARCHIVE_OUTPUT_DIRECTORY "${IVP_LIB_DIRECTORY}" CACHE PATH "" FORCE ) # For CMake 2.6
  SET( LIBRARY_OUTPUT_DIRECTORY "${IVP_LIB_DIRECTORY}" CACHE PATH "" FORCE ) # For CMake 2.6

  ADD_SUBDIRECTORY(${L})
ENDFOREACH(L)


#---------------------------------------------------------------------
#  Build the Applications
#---------------------------------------------------------------------
  
SET(IVP_NON_GUI_APPS
  app_alogscan
  app_aloggrep
  app_alogrm
  app_alogclip
  app_aloghelm
  app_nsplug
  app_gen_hazards
  pEchoVar
  pXRelay
  pHelmIvP
  pMarinePID
  pBasicContactMgr
  pNodeReporter
  pSafetyRetreat
  uFldPathCheck
  uFldShoreBroker
  uFldMessageHandler
  uFldNodeBroker
  uFldScope
  uFldNodeComms
  uFldBeaconRangeSensor
  uFldContactRangeSensor
  uFldHazardSensor
  uMOOSClient
  pSearchGrid
  uTimerScript
  uHelmScope
  uProcessWatch
  uSimCurrent
  uSimMarine
  uTermCommand
  uXMS
  uPokeDB
  pHostInfo
  pBinaryTest
  )

SET(IVP_GUI_APPS
 app_ivpcheck
  app_ffview
  app_geoview
  app_currview
  app_alogview
  app_vzaicview
  app_gzaicview
  pMarineViewer
  uFunctionVis
  uLogView
  uLogViewIPF
  uLogViewHelm
  app_cpaview
  )
  
SET(IVP_APPS_TO_BUILD ${IVP_NON_GUI_APPS})
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
# Add Nightly Tests
#=============================================================================

# By default, turn nightly testing is on.
SET( TEST_NIGHTLY TRUE CACHE BOOL "Enable/Disable nightly testing")

IF( TEST_NIGHTLY )

   # Set the directory where the nightly tests are located
   GET_FILENAME_COMPONENT( NIGHTLY_TEST_DIR 
      "${IVP_SOURCE_DIR}/../missions-test/nightly" ABSOLUTE )

   FILE(GLOB NIGHTLY_TEST_DIRS ${NIGHTLY_TEST_DIR}/n* )

   FIND_PROGRAM(CMAKE_CMD "cmake")

   FOREACH(TEST_DIR ${NIGHTLY_TEST_DIRS} )
      ADD_TEST( NAME ${TEST_DIR} 
                COMMAND ${CMAKE_CMD} -DIVP_BIN_DIR=${IVP_BIN_DIRECTORY} 
                              -DMOOS_BIN_DIR=${MOOS_BASE_DIR_ABSOLUTE}/MOOSBin
                              -DTEST_DIR=${TEST_DIR}
                              -P ${TEST_DIR}/run_test.cmake )   
   ENDFOREACH(TEST_DIR)

ENDIF( TEST_NIGHTLY )



