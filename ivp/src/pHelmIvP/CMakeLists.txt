#--------------------------------------------------------
# The CMakeLists.txt for:                        pHelmIvP
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
  HelmIvP.cpp
  pHelmIvPMain.cpp)

ADD_EXECUTABLE(pHelmIvP ${SRC})
   
TARGET_LINK_LIBRARIES(pHelmIvP 
  MOOS
  MOOSGen
  helmivp
  behaviors-ajshafer
  behaviors-marine
  behaviors-sandbox
  behaviors-colregs
  behaviors-don
  behaviors-henrik
  bhvutil	
  behaviors
  mbutil 
  ivpbuild 
  ivpcore 
  geometry
  logic 
  m
  dl
  genutil
  pthread)

