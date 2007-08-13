#===============================================================================
# Some details the user (whose building IvP) can control...
#===============================================================================

# Note: The value of IVP_BINARY_DIR was automatically established by CMake when
# our top-level CMakeLists.txt file invoked the command PROJECT(IVP). -CJC

# SET( LIBRARY_OUTPUT_PATH ${IVP_BINARY_DIR}/bin CACHE PATH
#      "Output directory for the IvP libraries" )

SET( EXECUTABLE_OUTPUT_PATH ${IVP_BINARY_DIR}/bin CACHE PATH
     "Output directory for the IvP executables" )

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
   ${MOOS_BASE_DIR}/Thirdparty/FLTKVW
   ${MOOS_BASE_DIR}/Essentials/MOOSUtilityLib
   ${MOOS_BASE_DIR}/Core/MOOSLIB
   ${MOOS_BASE_DIR}/Core/MOOSGenLib
   ${MOOS_BASE_DIR}/NavigationAndControl/MOOSNavLib
   ${MOOS_BASE_DIR}/NavigationAndControl/MOOSTaskLib
   ${MOOS_BASE_DIR}/Thirdparty/FLTKVW
   )
INCLUDE_DIRECTORIES(${MOOS_INCLUDE_DIRS})

#===============================================================================
# Specify which IvP subdirectories to build, and where to find IvP's header 
# files...
#===============================================================================

FILE(GLOB IVP_LIB_BASE_SUBDIRS
   lib_ivpcore*
   lib_ivpbuild
   lib_mbutil*           
   lib_behaviors         
   lib_bhvutil*          
   lib_behaviors-don*    
   lib_behaviors-sandbox*
   lib_geometry*         
   lib_logic*            
   newmat10D*            
   lib_anrp_util*        
   lib_behaviors-marine* 
   lib_behaviors-colregs*
   lib_helmivp*
   lib_genutil
   )


# In the original Makefile-based build system, the Makefile for 
# lib_behaviors-attic had an "all" target that was a no-op.  However, 
# lib_behaviors-attic cannot be fully hidden from the build system:
# lib_behaviors-sandbox/BHV_WaypointTOL.cpp must be able to #include
# the file AOF_WPT3D.h, which resides in the lib_behaviors-attic directory.
#
# So define a list of directories that will be added to the include search path,
# but that won't be treated during the build process as containing code to be
# built.
FILE(GLOB IVP_LIB_BASE_EXTRA_INCLUDE_DIRS
   lib_behaviors-attic
   )

FILE(GLOB IVP_APP_BASE_SUBDIRS
   pHelmIvP*
   pEchoVar*      
   pLoiterControl*
   pMarinePID*    
   uXMS*          
   uTMS*          
   iMarineSim*    
   iPNICompass*   
   MOOSDump*   
   MOOSRemote
# CMake doesn't like double-listed subdirectories, and app_splug is picked up
# by the glob that sets the IVP_APP_TOOLS_SUBDIRS variable.
#    app_splug*     
   pTransponderAIS*
   uTermCommand*
   iMetaCompass*
   iPWMController*
   uProcessWatch*
   )

FILE(GLOB IVP_LIB_TOOLS_SUBDIRS
   lib_marineview*
# CMake doesn't like having the same directory listed more than once, and this
# one is also listed in IVP_LIB_BASE_SUBDIRS.  (These lists mirror what appears
# in the project's original GNU Make makefiles.)
#    lib_behaviors-don*    
   lib_navplot*
   lib_ivpbuild-extra*
   lib_track*
   lib_ipfview*
   uVoice
   )

FILE(GLOB IVP_APP_TOOLS_SUBDIRS
   app_*		
   p1BTracker*	
   iHeadingMonitor*
   p2BTracker*	
   pMarineViewer*
   pBearingsSim*	
   pClusterManager*
   pACom*		
   pExtTracker*
   iWifi*	    	
   pFunctionSender*
   pMineSim*
   pTarget*	        
   uFunctionVis*
   uFunctionViewer
   iMicroModem*
   pBearings
#    uPlayBack*       I don't see this file in the src directory. -CJC	        
#    pDrone*          I don't see this file in the src directory. -CJC
#    pMessageSim*     I don't see this file in the src directory. -CJC
   )

SET(IVP_DIRS_TO_BUILD 
   ${IVP_LIB_BASE_SUBDIRS}
   ${IVP_APP_BASE_SUBDIRS}
   ${IVP_LIB_TOOLS_SUBDIRS}
   ${IVP_APP_TOOLS_SUBDIRS}
   )


INCLUDE_DIRECTORIES(
   ${IVP_LIB_BASE_SUBDIRS}
   ${IVP_LIB_TOOLS_SUBDIRS}
   ${IVP_LIB_BASE_EXTRA_INCLUDE_DIRS}
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
