#--------------------------------------------------------
# The CMakeLists.txt for:                       uMayFinish
# Author(s):                              Mike Benjamin
#--------------------------------------------------------

SET(SRC
  MayFinish.cpp
  MayFinish_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(uMayFinish ${SRC})

TARGET_LINK_LIBRARIES(uMayFinish
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

