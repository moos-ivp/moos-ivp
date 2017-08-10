#--------------------------------------------------------
# The CMakeLists.txt for:                uFldLoiterAssign
# Author(s):                             Michael Benjamin        
#--------------------------------------------------------

SET(SRC
  LoiterAssign.cpp
  LoiterAssign_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(uFldLoiterAssign ${SRC})

TARGET_LINK_LIBRARIES(uFldLoiterAssign
   ${MOOS_LIBRARIES}
   geometry
   apputil
   mbutil
   m
   pthread)

