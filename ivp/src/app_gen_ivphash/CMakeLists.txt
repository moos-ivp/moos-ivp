#--------------------------------------------------------
# The CMakeLists.txt for:                     gen_ivphash
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  main.cpp
)

ADD_EXECUTABLE(gen_ivphash ${SRC})

TARGET_LINK_LIBRARIES(gen_ivphash
  mbutil
  ${SYSTEM_LIBS}
)
