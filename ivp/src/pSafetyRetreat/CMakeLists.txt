#--------------------------------------------------------
# The CMakeLists.txt for:                  pSafetyRetreat
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET( SYSTEM_LIBS 
       wsock32
     )
else (${WIN32})
  SET( SYSTEM_LIBS 
       m
       pthread
     )
endif (${WIN32})

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pSafetyRetreat ${SRC})

TARGET_LINK_LIBRARIES(pSafetyRetreat
   ${MOOS_LIBRARIES}
   mbutil
   geometry
   ${SYSTEM_LIBS})

