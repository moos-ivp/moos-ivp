#--------------------------------------------------------
# The CMakeLists.txt for:                        pHelmIvP
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pHelmIvP ${SRC})
   
TARGET_LINK_LIBRARIES(pHelmIvP 
  MOOS
  MOOSGen
  helmivp
  behaviors-ajshafer
  behaviors-marine
  behaviors-sandbox
  behaviors-colregs
  bhvutil	
  behaviors
  mbutil 
  ivpbuild 
  ivpcore 
  geometry
  logic 
  m
  pthread)

