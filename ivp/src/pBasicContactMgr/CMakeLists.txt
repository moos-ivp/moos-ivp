#--------------------------------------------------------
# The CMakeLists.txt for:                pBasicContactMgr
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
     BasicContactMgr.cpp
     BCM_ExampleConfig.cpp
     ContactRecord.cpp
     PlatformAlertRecord.cpp
     BasicContactMgrMain.cpp
)

ADD_EXECUTABLE(pBasicContactMgr ${SRC})

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS m pthread)
endif (${WIN32})

TARGET_LINK_LIBRARIES(pBasicContactMgr
   MOOS
   MOOSGen
   mbutil
   geometry
   contacts
   ${SYSTEM_LIBS}
   )

