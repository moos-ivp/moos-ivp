#--------------------------------------------------------
# The CMakeLists.txt for:                        alogpare
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

SET(SYSTEM_LIBS m)

SET(SRC main.cpp PareEngine.cpp)

ADD_EXECUTABLE(alogpare ${SRC})
   
TARGET_LINK_LIBRARIES(alogpare
  logutils
  mbutil
  ${SYSTEM_LIBS})

