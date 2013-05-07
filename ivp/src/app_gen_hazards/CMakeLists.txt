#--------------------------------------------------------
# The CMakeLists.txt for:                     gen_hazards
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)


ADD_EXECUTABLE(gen_hazards ${SRC})

TARGET_LINK_LIBRARIES(gen_hazards
  ufld_hazards
  geometry
  mbutil
  ${SYSTEM_LIBS}
)
