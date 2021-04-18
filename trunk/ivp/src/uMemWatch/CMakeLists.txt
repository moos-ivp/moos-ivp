#--------------------------------------------------------
# The CMakeLists.txt for:                       uMemWatch
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  MemWatch.cpp
  MemWatch_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(uMemWatch ${SRC})

TARGET_LINK_LIBRARIES(uMemWatch
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

