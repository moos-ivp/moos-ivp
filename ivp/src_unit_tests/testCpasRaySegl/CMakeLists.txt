#--------------------------------------------------------
# The CMakeLists.txt for:                 testCpasRaySegl
# Author(s):                                Mike Benjamin
#--------------------------------------------------------
	
FILE(GLOB SRC	
  main.cpp)		
  
ADD_EXECUTABLE(testCpasRaySegl ${SRC})
   				   
TARGET_LINK_LIBRARIES(testCpasRaySegl
  geometry
  mbutil
  m)

