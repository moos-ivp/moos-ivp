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

SET(INSTALL_HEADERS
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

# install headers
INSTALL( FILES ${INSTALL_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MOOSUtilityLib )

# install libraries
INSTALL(TARGETS
MOOSUtility
RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

