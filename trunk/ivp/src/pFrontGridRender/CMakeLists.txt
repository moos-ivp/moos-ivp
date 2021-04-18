#--------------------------------------------------------
# The CMakeLists.txt for:                pFrontGridRender
# Author(s):                              
#--------------------------------------------------------

# FILE(GLOB SRC *.cpp)

SET(SRC
  FrontGridRender.cpp
  FrontGridRender_Info.cpp
  main.cpp
)



ADD_EXECUTABLE(pFrontGridRender ${SRC})

TARGET_LINK_LIBRARIES(pFrontGridRender
   ${MOOS_LIBRARIES}
   geometry
   mbutil
   henrik_anneal
   m
   pthread)

