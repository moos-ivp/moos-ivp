#--------------------------------------------------------
# The CMakeLists.txt for:                      uLoadWatch
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  LoadWatch.cpp
  LoadWatch_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(uLoadWatch ${SRC})

TARGET_LINK_LIBRARIES(uLoadWatch
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

