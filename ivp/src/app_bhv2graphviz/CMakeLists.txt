#--------------------------------------------------------
# The CMakeLists.txt for:                    bhv2graphviz
# Author(s):                               Toby Schneider
#                                       toby@gobysoft.org
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
  bhv2graphviz.cpp
)

ADD_EXECUTABLE(bhv2graphviz ${SRC})
   
TARGET_LINK_LIBRARIES(bhv2graphviz
  helmivp
  contacts
  ufield
  behaviors-marine
  behaviors-colregs
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
