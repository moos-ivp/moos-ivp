#--------------------------------------------------------
# The CMakeLists.txt for:                   uProcessWatch
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(uProcessWatch ${SRC})
 
TARGET_LINK_LIBRARIES(uProcessWatch 
  MOOS
  MOOSGen
  mbutil
  m
  pthread)


IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
   INSTALL(
      TARGETS uProcessWatch
      DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
      COMPONENT ivp-core)
ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
