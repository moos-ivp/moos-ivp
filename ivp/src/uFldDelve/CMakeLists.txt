#--------------------------------------------------------
# The CMakeLists.txt for:                       uFldDelve
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  Delver.cpp
  Delver_Info.cpp
  TransRecord.cpp
  main.cpp
)

ADD_EXECUTABLE(uFldDelve ${SRC})

TARGET_LINK_LIBRARIES(uFldDelve
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

