#--------------------------------------------------------
# The CMakeLists.txt for:                       mhash_gen
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
  main.cpp
)

ADD_EXECUTABLE(mhash_gen ${SRC})

TARGET_LINK_LIBRARIES(mhash_gen
  mbutil
  ${SYSTEM_LIBS}
)
