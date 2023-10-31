#--------------------------------------------------------
# The CMakeLists.txt for:               lib_dep_behaviors
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  BHV_FixTurn.cpp
)

SET(HEADERS
  BHV_FixTurn.h
) 

ADD_LIBRARY(dep_behaviors ${SRC})

TARGET_LINK_LIBRARIES(dep_behaviors
   mbutil
   helmivp
   behaviors
   geometry
   )

