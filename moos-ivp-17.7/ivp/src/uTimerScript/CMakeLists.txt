#--------------------------------------------------------
# The CMakeLists.txt for:                    uTimerScript
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS
    m
    pthread)
endif (${WIN32})

SET(SRC 
   TS_MOOSApp.cpp 
   TS_Info.cpp
   EnumVariable.cpp
   RandomVariable.cpp
   RandomVariableSet.cpp 
   RandVarUniform.cpp
   RandVarGaussian.cpp
   RandomPair.cpp
   RandomPairSet.cpp
   RandPairPoly.cpp
   main.cpp)

ADD_EXECUTABLE(uTimerScript ${SRC})

TARGET_LINK_LIBRARIES(uTimerScript
   ${MOOS_LIBRARIES}
   geometry
   mbutil
   apputil
   logic
   ${SYSTEM_LIBS})

