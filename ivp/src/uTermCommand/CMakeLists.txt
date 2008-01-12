#--------------------------------------------------------
# The CMakeLists.txt for:                    uTermCommand
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_uTermCommand "Build program uTermCommand" ON)

IF("${IVP_BUILD_uTermCommand}" STREQUAL "ON")
   ADD_EXECUTABLE(uTermCommand ${SRC})
   
   TARGET_LINK_LIBRARIES(uTermCommand 
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   genutil
   m
   pthread)

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS uTermCommand
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_uTermCommand}" STREQUAL "ON")
