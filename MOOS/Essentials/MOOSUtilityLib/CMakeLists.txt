PROJECT(MOOSUtilityLib)

SET(MOOSUTILITYLIB_SRCS
   MOOSTerrain.cpp
   ScalarPID.cpp
   PitchZPID.cpp
   MOOSGeodesy.cpp
   ThirdPartyRequest.cpp
   MOOSTerrain.h
   ScalarPID.h
   PitchZPID.h
   MOOSGeodesy.h
   ThirdPartyRequest.h
   MOOSTimeJournal.h
   InterpBuffer.h
   MOOSThread.h
   MOOSThreadedTimeJournal.h
   TMaxPair.h
   TMinPair.h
   MOOSMemoryMapped.h
   )



ADD_LIBRARY(MOOSUtility ${MOOSUTILITYLIB_SRCS})

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

INSTALL(
   TARGETS MOOSUtility
   DESTINATION lib/moos-${MOOS_VERSION}
   COMPONENT Moos_Essentials_Dev)

# Note: If there are any header files that are truly unneeded by users of this
# library, it should be OK to remove them from the PUBLIC_HEADERS variable,
# which in turn will prevent them from being installed. -CJC
SET(PUBLIC_HEADERS 
   InterpBuffer.h
   MOOSGeodesy.h
   MOOSMemoryMapped.h
   MOOSTerrain.h
   MOOSThreadedTimeJournal.h
   MOOSThread.h
   MOOSTimeJournal.h
   PitchZPID.h
   ScalarPID.h
   ThirdPartyRequest.h
   TMaxPair.h
   TMinPair.h
)

INSTALL(
   FILES ${PUBLIC_HEADERS}
   DESTINATION include/moos-${MOOS_VERSION}/Essentials/MOOSUtilityLib
   COMPONENT Moos_Essentials_Dev)

#===============================================================================
