FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_MOOSBlink "Build program MOOSBlink" ON)

IF("${IVP_BUILD_MOOSBlink}" STREQUAL "ON")
   ADD_EXECUTABLE(MOOSBlink ${SRC})
   
   TARGET_LINK_LIBRARIES(MOOSBlink 
   MOOS
   MOOSGen
   anrp_util
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "MOOSBlink: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS MOOSBlink
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_MOOSBlink}" STREQUAL "ON")
