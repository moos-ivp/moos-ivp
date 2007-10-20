FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_uMicroModemWatchPPS "Build program uMicroModemWatchPPS" ON)

IF("${IVP_BUILD_uMicroModemWatchPPS}" STREQUAL "ON")
   ADD_EXECUTABLE(uMicroModemWatchPPS ${SRC})
   
   TARGET_LINK_LIBRARIES(uMicroModemWatchPPS 
   MOOS
   MOOSGen
   anrp_util
   abahr_util
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "uMicroModemWatchPPS: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS uMicroModemWatchPPS
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT none)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_uMicroModemWatchPPS}" STREQUAL "ON")
