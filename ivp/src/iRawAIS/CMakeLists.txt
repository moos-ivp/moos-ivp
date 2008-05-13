#--------------------------------------------------------
# The CMakeLists.txt for:                      iRawAIS
# Author(s):                                Phong Tran
#--------------------------------------------------------

SET(SRC
   RawAIS.cpp
   mainRawAIS.cpp
)

ADD_EXECUTABLE(iRawAIS ${SRC})

TARGET_LINK_LIBRARIES(iRawAIS 
MOOS
MOOSGen
MOOSUtility
mbutil
ais
m
pthread)
