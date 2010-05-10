#--------------------------------------------------------
# The CMakeLists.txt for:                pBasicContactMgr
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
     BasicContactMgr.cpp
     ContactRecord.cpp
     PlatformAlertRecord.cpp
     BasicContactMgrMain.cpp
)

ADD_EXECUTABLE(pBasicContactMgr ${SRC})

TARGET_LINK_LIBRARIES(pBasicContactMgr
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread)

