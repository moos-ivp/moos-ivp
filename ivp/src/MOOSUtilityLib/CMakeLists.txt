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
