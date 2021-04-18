#--------------------------------------------------------
# The CMakeLists.txt for:                    gen_moos_app
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SRC
   AppGenerator.cpp
   AppGenerator_Info.cpp
   main.cpp
)


ADD_EXECUTABLE(gen_moos_app ${SRC})

TARGET_LINK_LIBRARIES(gen_moos_app
  mbutil
  ${SYSTEM_LIBS}
)
