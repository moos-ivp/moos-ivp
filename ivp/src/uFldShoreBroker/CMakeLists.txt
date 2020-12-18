#--------------------------------------------------------
# The CMakeLists.txt for:                 uFldShoreBroker
# Author(s):                                Mike Benjamin
#--------------------------------------------------------
 
SET(SRC
  main.cpp
  ShoreBroker.cpp
  ShoreBroker_Info.cpp
)

ADD_EXECUTABLE(uFldShoreBroker ${SRC})

TARGET_LINK_LIBRARIES(uFldShoreBroker
   ${MOOS_LIBRARIES}
   mbutil
   apputil
   ufield
   m
   pthread)

