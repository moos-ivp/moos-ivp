#--------------------------------------------------------
# The CMakeLists.txt for:                   uFldNodeComms
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  FldNodeComms.cpp
  FldNodeComms_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(uFldNodeComms ${SRC})

TARGET_LINK_LIBRARIES(uFldNodeComms
   ${MOOS_LIBRARIES}
   mbutil
   apputil
   contacts
   ufield
   geometry
   m
   pthread)

