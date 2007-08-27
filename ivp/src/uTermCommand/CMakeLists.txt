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

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "uTermCommand: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS uTermCommand
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_uTermCommand}" STREQUAL "ON")
