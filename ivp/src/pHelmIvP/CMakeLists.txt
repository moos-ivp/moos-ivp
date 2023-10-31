#--------------------------------------------------------
# The CMakeLists.txt for:                        pHelmIvP
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS
    wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS
    dl
    m
    pthread)
endif (${WIN32})

SET(SRC
  HelmIvP.cpp
  HelmEngine.cpp
  HelmIvP_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pHelmIvP ${SRC})
   
TARGET_LINK_LIBRARIES(pHelmIvP 
  ${MOOS_LIBRARIES}
  helmivp
  dep_behaviors
  behaviors-marine
  bhv_fixedturn
  turngeo
  contacts
  behaviors-colregs
  ufield
  behaviors
  bhvutil	
  ivpbuild 
  ivpcore
  ivpsolve 
  geometry
  apputil
  mbutil 
  logic 
  genutil
  ${SYSTEM_LIBS})


