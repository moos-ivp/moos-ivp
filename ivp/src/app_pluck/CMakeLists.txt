#--------------------------------------------------------
# The CMakeLists.txt for:                       app_pluck
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  Plucker.cpp
  Plucker_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pluck ${SRC})

TARGET_LINK_LIBRARIES(pluck
  mbutil
  ${SYSTEM_LIBS}
)
