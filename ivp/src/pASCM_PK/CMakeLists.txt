#--------------------------------------------------------
# The CMakeLists.txt for:                      pASCM_PK
# Author(s):                                   Phong TRan
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pASCM_PK ${SRC})

TARGET_LINK_LIBRARIES(pASCM_PK 
MOOS
MOOSGen
MOOSUtility
mbutil
ais
m
pthread)
