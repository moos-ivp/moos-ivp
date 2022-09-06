#--------------------------------------------------------
# The CMakeLists.txt for:                     pSearchGrid
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  SearchGrid.cpp
  SearchGrid_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pSearchGrid ${SRC})

TARGET_LINK_LIBRARIES(pSearchGrid
   ${MOOS_LIBRARIES}
   bhvutil
   contacts
   geometry
   mbutil
   apputil
   ${SYSTEM_LIBS}
)

