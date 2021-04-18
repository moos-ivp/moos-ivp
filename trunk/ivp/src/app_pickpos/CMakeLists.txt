#--------------------------------------------------------
# The CMakeLists.txt for:                     app_pickpos
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(pickpos ${SRC})

TARGET_LINK_LIBRARIES(pickpos
  geometry
  mbutil
  ${SYSTEM_LIBS}
)
