#--------------------------------------------------------
# The CMakeLists.txt for:                pBasicContactMgr
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC
     BasicContactMgr.cpp
     BasicContactMgr_Info.cpp
     ContactRecord.cpp
     CMAlert.cpp
     PlatformAlertRecord.cpp
     main.cpp
)

ADD_EXECUTABLE(pBasicContactMgr ${SRC})

# Set System Specific Libraries
if (${WIN32})
  SET(SYSTEM_LIBS wsock32)
else (${WIN32})
  SET(SYSTEM_LIBS m pthread)
endif (${WIN32})


TARGET_LINK_LIBRARIES(pBasicContactMgr
   ${MOOS_LIBRARIES}
   ${MOOSGeodesy_LIBRARIES}
   mbutil
   apputil
   geometry
   contacts
   ${SYSTEM_LIBS}
)

