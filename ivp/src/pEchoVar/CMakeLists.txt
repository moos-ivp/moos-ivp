#--------------------------------------------------------
# The CMakeLists.txt for:                        pEchoVar
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pEchoVar "Build program pEchoVar" ON)

IF("${IVP_BUILD_pEchoVar}" STREQUAL "ON")
   ADD_EXECUTABLE(pEchoVar ${SRC})
   
   TARGET_LINK_LIBRARIES(pEchoVar 
   MOOS
   MOOSGen
   mbutil
   m
   pthread)

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pEchoVar
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pEchoVar}" STREQUAL "ON")
