#--------------------------------------------------------
# The CMakeLists.txt for:                       pAutoPoke
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  AutoPoke.cpp
  AutoPoke_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pAutoPoke ${SRC})

TARGET_LINK_LIBRARIES(pAutoPoke
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)

