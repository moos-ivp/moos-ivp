#--------------------------------------------------------
# The CMakeLists.txt for:                      iRawAIS
# Author(s):                                Phong Tran
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iRawAIS ${SRC})

TARGET_LINK_LIBRARIES(iRawAIS 
MOOS
MOOSGen
MOOSUtility
mbutil
ais
m
pthread)
