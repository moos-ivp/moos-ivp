###########################################################
#   Top level CMakeLists.txt file for the MOOS Project
#   pnewman@robots.ox.ac.uk
###########################################################


CMAKE_MINIMUM_REQUIRED(VERSION 2.0)


###########################################################
		    PROJECT(MOOS)
###########################################################


INCLUDE(CPack)

#####################################################
#default set up for location of built files
#NB user can change this through UI
SET(MOOSBIN ${MOOS_BINARY_DIR}/MOOSBin)

#dump libs and executables in the same place
SET( LIBRARY_OUTPUT_PATH ${MOOSBIN} CACHE PATH
     "Output directory for the MOOS libraries" )
SET( EXECUTABLE_OUTPUT_PATH ${MOOSBIN} CACHE PATH
     "Output directory for the MOOS executables" )


SET(MOOS_VERSION "" CACHE STRING "The version of MOOS that you're building.  I.e., 7.0.1")
IF ("${MOOS_VERSION}" STREQUAL "")
   MESSAGE("If you don't set MOOS_VERSION, no INSTALL directives will "
      "be called.  This is because they use MOOS_VERSION as part of their "
      "naming scheme.  This means that 'make install' won't put the project's "
      "files in the ideal locations, and our scripts for creating packags "
      "(such as Debian .deb files) won't work either."
      )
ENDIF ("${MOOS_VERSION}" STREQUAL "")


############# CORE ? ################
SET( BUILD_CORE_MOOS ON CACHE BOOL "Build Core MOOS")
IF(BUILD_CORE_MOOS)
    #here we name all the things we want built as part of the core MOOS
    SET(BUILD_DIRS ${BUILD_DIRS} Core)
ENDIF(BUILD_CORE_MOOS)




############# ESSENTIAL APPS ? ################
SET( BUILD_ESSENTIAL_MOOS ON CACHE BOOL "Build Essential MOOS Processes (like pLogger pAntler etc)")
IF(BUILD_ESSENTIAL_MOOS)
    #here we name all the things we want built as part of the Essential MOOS
    SET(BUILD_DIRS ${BUILD_DIRS} Essentials)
ENDIF(BUILD_ESSENTIAL_MOOS)

############# CONTROL AND NAV ? ################
SET( BUILD_CONTROL_AND_NAV OFF CACHE BOOL "Build control and navigation Apps iRemote pHelm pNav")
IF(BUILD_CONTROL_AND_NAV)
    #here we name all the things we want built under the navigation and control heading
    SET(BUILD_DIRS ${BUILD_DIRS} NavigationAndControl )
ENDIF(BUILD_CONTROL_AND_NAV)

############# TOOLS ? ################
SET( BUILD_TOOLS OFF CACHE BOOL "Build GUI tools, simulators and matlab access")
IF(BUILD_TOOLS)
    #here we tell CMake to descend into the tools directory. More 
    #decisions will be made in there
    SET(BUILD_DIRS ${BUILD_DIRS} Tools)
ENDIF(BUILD_TOOLS)


############# INSTRUMENTS ? ################
SET( BUILD_INSTRUMENTS OFF CACHE BOOL "Build Instruments ")
IF(BUILD_INSTRUMENTS)
    SET(BUILD_DIRS ${BUILD_DIRS} Instruments)
ENDIF(BUILD_INSTRUMENTS)


############# OTHER BRANCHES ? ################
SET( ALSO_BUILD  CACHE STRING "Specify other directories you want to include in the build (semicolon separated list)")
SET(BUILD_DIRS ${BUILD_DIRS} ${ALSO_BUILD})


############## Thirdparty ?######################
#we'll always descend into thirdparty but don't always build stuff there
SET(BUILD_DIRS ${BUILD_DIRS} Thirdparty)
SET(THIRDPARTY_DIR ${MOOS_SOURCE_DIR}/Thirdparty)


############# UTILITY LIBRARY ? ################
SET( BUILD_UTILITY_LIBRARY ON CACHE BOOL "Build MOOSUtility Library")
IF(BUILD_UTILITY_LIBRARY)
    #here we build a goody box that many folk will want to use
    SET(BUILD_DIRS ${BUILD_DIRS} Essentials/MOOSUtilityLib)
    INCLUDE_DIRECTORIES(${MOOS_SOURCE_DIR}/Essentials )
    SET(MOOSLIBS ${MOOSLIBS} MOOSUtility)
ENDIF(BUILD_UTILITY_LIBRARY)


#####################################################
# some includes are always needed...
INCLUDE_DIRECTORIES(${MOOS_SOURCE_DIR}/Core )



#####################################################
# some definitions are always needed...
IF(WIN32)
    ADD_DEFINITIONS(-DWINDOWS_NT -D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_DEPRECATE)
ENDIF(WIN32)
IF(UNIX)
    ADD_DEFINITIONS(-DUNIX)
ENDIF(UNIX)


#####################################################
# some platform libs are always required...
IF(WIN32)
  SET(PLATFORM_LIBS wsock32 comctl32)
ENDIF(WIN32)
IF(UNIX)
  SET(PLATFORM_LIBS m pthread)

  IF("${CMAKE_SYSTEM_NAME}" MATCHES "SunOS")
	SET(PLATFORM_LIBS ${PLATFORM_LIBS} socket nsl rt)
  ENDIF("${CMAKE_SYSTEM_NAME}" MATCHES "SunOS")	

ENDIF(UNIX)

#name the MOOSLIBS
SET(MOOSLIBS ${MOOSLIBS} MOOS MOOSGen)

#####################################################
#name OPENGL LIBS (useful for child projects)
IF(WIN32)
  SET(OPENGL_LIBS glu32 opengl32 )
ENDIF(WIN32)

IF(UNIX)
  SET(OPENGL_LIBS GL GLU)
ENDIF(UNIX)


#####################################################
# be helpful and let all projects look for libraries here
LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})


# GO FORTH!
SUBDIRS(${BUILD_DIRS})





###########################################################

