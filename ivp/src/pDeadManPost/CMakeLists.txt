#--------------------------------------------------------
# The CMakeLists.txt for:                    pDeadManPost
# Author(s):                              Mike Benjamin
#--------------------------------------------------------

SET(SRC
  DeadManPost.cpp
  DeadManPost_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pDeadManPost ${SRC})

TARGET_LINK_LIBRARIES(pDeadManPost
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

