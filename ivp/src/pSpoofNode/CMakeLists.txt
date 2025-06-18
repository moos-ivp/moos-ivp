#--------------------------------------------------------
# The CMakeLists.txt for:                      pSpoofNode
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  SpoofNode.cpp
  SpoofNode_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pSpoofNode ${SRC})

TARGET_LINK_LIBRARIES(pSpoofNode
   ${MOOS_LIBRARIES}
   apputil
   contacts
   geometry
   mbutil
   m
   pthread)

