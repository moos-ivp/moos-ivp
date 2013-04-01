#--------------------------------------------------------
# The CMakeLists.txt for:                       pTestClassifyQueue
# Author(s):                              Mike Benjamin
#--------------------------------------------------------

SET(SRC
  TestClassifyQueue.cpp
  TestClassifyQueue_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pTestClassifyQueue ${SRC})

TARGET_LINK_LIBRARIES(pTestClassifyQueue
   ${MOOS_LIBRARIES}
   ufld_hazards
   apputil
   mbutil
   m
   pthread)

