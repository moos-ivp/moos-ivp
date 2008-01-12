#--------------------------------------------------------
# The CMakeLists.txt for:                            uXMS
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_uXMS "Build program uXMS" ON)

IF("${IVP_BUILD_uXMS}" STREQUAL "ON")
   ADD_EXECUTABLE(uXMS ${SRC})
   
   TARGET_LINK_LIBRARIES(uXMS 
   MOOS
   MOOSGen
   genutil
   mbutil
   m
   pthread)

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS uXMS
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_uXMS}" STREQUAL "ON")
