FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_uMicroModemAddPPS "Build program uMicroModemAddPPS" OFF)

IF("${IVP_BUILD_uMicroModemAddPPS}" STREQUAL "ON")
   ADD_EXECUTABLE(uMicroModemAddPPS ${SRC})
   
   TARGET_LINK_LIBRARIES(uMicroModemAddPPS 
   MOOS
   MOOSGen
   MOOSUtility
   anrp_util
   abahr_util
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "uMicroModemAddPPS: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS uMicroModemAddPPS
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT none)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_uMicroModemAddPPS}" STREQUAL "ON")
