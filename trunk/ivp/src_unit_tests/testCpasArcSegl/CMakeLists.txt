#--------------------------------------------------------
# The CMakeLists.txt for:                 testCpasArcSegl
# Author(s):                                Mike Benjamin
#--------------------------------------------------------
	
FILE(GLOB SRC	
  main.cpp)		
  
ADD_EXECUTABLE(testCpasArcSegl ${SRC})
   				   
TARGET_LINK_LIBRARIES(testCpasArcSegl
  geometry
  mbutil
  m)

