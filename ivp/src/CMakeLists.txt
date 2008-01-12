PROJECT(IVP)

#******************************************************************************
#  NOTE: WHAT TO MODIFY IF CMAKE/MAKE CAN'T FIND CERTAIN FILES/DIRECTORIES!
#
#  MOOS:  This file assumes that standard the MOOS 7.0 directory structure is
#         in place, and that it's rooted at the directory given by the CMake
#         variable "MOOS_BASE_DIR".  To specify where that directory is, you
#         can modify the line below that looks like this:
#
#         GET_FILENAME_COMPONENT(MOOS_BASE_DIR ../../MOOS ABSOLUTE)
#
#         by replacing the "../../MOOS" with the correct value.
#
#  IVP_DATA_DIR: This is set below by a command that looks like the following:
#
#         GET_FILENAME_COMPONENT(IVP_DATA_DIR ./data ABSOLUTE)
#
#         You can modify the "../data" pathname as needed, but this shouldn't
#         normally require and change.

#===============================================================================
# Some details the user (whose building IvP) can control...
#===============================================================================

# Note: The value of IVP_BINARY_DIR was automatically established by CMake when
# our top-level CMakeLists.txt file invoked the command PROJECT(IVP). -CJC

# The MOOS build will have set its own values for these variables, and we don't
# want those values to affect where IvP's libs and executables go.  So change 
# their values within the context of IvP's targets.
#
# By avoiding use of the CACHE option, we force the (temporary) value of these
# variables rather than picking up the cached values used in MOOS.
SET( LIBRARY_OUTPUT_PATH "")

SET( EXECUTABLE_OUTPUT_PATH ${IVP_SOURCE_DIR}/bin)

#===============================================================================
# Specify where to find MOOS's headers and libraries...
#===============================================================================

# Using absolute pathnames seems to work better then relative, because you're 
# never sure from which directory a relative path will be employed in commands
# such as LINK_DIRECTORIES() ...
GET_FILENAME_COMPONENT(MOOS_BASE_DIR ../../MOOS ABSOLUTE)
SET(MOOS_LIB_DIR ${MOOS_BASE_DIR}/MOOSBin)
LINK_DIRECTORIES(${MOOS_LIB_DIR})

# No harm in giving all IvP software access to MOOS's include libraries.
# MOOS and IvP both have a copy of the 'newmat' library (different versions),
# but we don't seem to need to include MOOS's copy in our include path.
SET(MOOS_INCLUDE_DIRS
   ${MOOS_BASE_DIR}/Essentials
   ${MOOS_BASE_DIR}/Core
   ${MOOS_BASE_DIR}/NavigationAndControl
   ${MOOS_BASE_DIR}/Essentials/MOOSUtilityLib
   ${MOOS_BASE_DIR}/Core/MOOSLIB
   ${MOOS_BASE_DIR}/Core/MOOSGenLib
   ${MOOS_BASE_DIR}/Thirdparty/FLTKVW
   )
INCLUDE_DIRECTORIES(${MOOS_INCLUDE_DIRS})

#===============================================================================
# Specify which IvP subdirectories to build, and where to find IvP's header 
# files...
#===============================================================================

# This is used to supply a preprocessor definition that gets compiled into some
# code, such as lib_marineview.  I really dislike this approach because it's 
# very brittle w.r.t. anything getting moved around, or if you use the code on
# a different system than that on which it was compiled. -CJC
GET_FILENAME_COMPONENT(IVP_DATA_DIR ../data ABSOLUTE)

# Sometimes people introduce their own subdirectories into the build tree so
# that, among other reasons, their behaviors can be used by pHelmIvP.
# (We expect this practice to end once each behavior is running in its own
# MOOSApp.)
# Anyway, we'll have them indicate the presence of such directories with the
# presence (and contents) of the file:
#
# ExtraCMakeIncludeDirs.txt : should contain some CMake INCLUDE_DIRECTORIES
#    commands, so that the the compiler can find that code's header files
#    in case other programs / libs in IvP (probably just pHelmIvP, 
#    realistically) try to #include them.
#
# Iff one of these extra subdirectories contains a ExtraCMakeIncludeDirs.txt,
# we'll also take that to mean that the subdirectory should be built by this
# build process.
#
# The reason a new subdirectory must be explicitly flagged for inclusion
# (via the presense of a ExtraCMakeIncludeDirs.txt file) is that this present
# CMake file explicitly lists the subdirectories build, rather than just finding
# them all with one big "FILE(GLOB *)" command.

SET(IVP_DIRS_TO_BUILD "")
FILE(GLOB_RECURSE CUSTOM_ADDED_INCLUDES ExtraCMakeIncludeDirs.txt)
FOREACH(F ${CUSTOM_ADDED_INCLUDES})
  INCLUDE(${F})
  GET_FILENAME_COMPONENT(DIR ${F} PATH)
  MESSAGE("Addding supplemental source directory: " ${DIR})
  SET(IVP_DIRS_TO_BUILD ${IVP_DIRS_TO_BUILD} ${DIR})
ENDFOREACH(F)

FILE(GLOB IVP_LIB_BASE_SUBDIRS
   lib_ivpcore*
   lib_ivpbuild
   lib_mbutil*           
   lib_bhvutil*          
   lib_geometry*         
   lib_logic*            
   newmat10D*            
   lib_behaviors
   lib_behaviors-marine* 
   lib_behaviors-colregs*
   lib_behaviors-sandbox*
   lib_helmivp*
   lib_genutil
   )

FILE(GLOB IVP_APP_BASE_SUBDIRS
   pHelmIvP*
   pEchoVar*      
   pMarinePID*    
   uXMS*          
   iMarineSim*    
   pTransponderAIS*
   uTermCommand*
   uProcessWatch*
   uRepeater*
   )

FILE(GLOB IVP_LIB_TOOLS_SUBDIRS
   lib_marineview*
   lib_navplot*
   lib_ipfview*
   lib_artifacts*
   )

FILE(GLOB IVP_APP_TOOLS_SUBDIRS
   app_*		
   pMarineViewer*
   pShipsideViewer*
   )

SET(IVP_DIRS_TO_BUILD 
   lib_fltk-1.1.7
   ${IVP_DIRS_TO_BUILD}
   ${IVP_LIB_BASE_SUBDIRS}
   ${IVP_APP_BASE_SUBDIRS}
   ${IVP_LIB_TOOLS_SUBDIRS}
   ${IVP_APP_TOOLS_SUBDIRS}
   )

INCLUDE_DIRECTORIES(
   lib_fltk-1.1.7
   ${IVP_LIB_BASE_SUBDIRS}
   ${IVP_LIB_TOOLS_SUBDIRS}
   )


FOREACH(D ${IVP_DIRS_TO_BUILD})
  ADD_SUBDIRECTORY(${D})
ENDFOREACH(D)

#===============================================================================
# DEFINE INSTALLATION COMPONENTS...
#===============================================================================

SET(IVP_VERSION 0.1-1)

# Define the installation components for the libraries...
FOREACH(D ${IVP_DIRS_TO_BUILD})
   GET_FILENAME_COMPONENT(SUBDIR_NAME ${D} NAME)

   # Only deal with directories that start with lib_ ...
   STRING(REGEX MATCH lib_* X ${D})
   IF(NOT "${X}" STREQUAL "")
      FILE(GLOB IVP_HEADER_FILES ${SUBDIR_NAME}/*.h)

      INSTALL(
         FILES ${IVP_HEADER_FILES}
         DESTINATION include/ivp-${IVP_VERSION}/${SUBDIR_NAME}
         COMPONENT IvP_Dev
         )
   ENDIF(NOT "${X}" STREQUAL "")
ENDFOREACH(D)
#
