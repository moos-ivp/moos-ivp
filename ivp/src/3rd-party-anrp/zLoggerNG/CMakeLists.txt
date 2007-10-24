FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_zLoggerNG "Build program zLoggerNG" ON)

IF("${IVP_BUILD_zLoggerNG}" STREQUAL "ON")
   ADD_EXECUTABLE(zLoggerNG ${SRC})
   
   TARGET_LINK_LIBRARIES(zLoggerNG 
   MOOS
   MOOSGen
   anrp_util
   m
   pthread
   z)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "zLoggerNG: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS zLoggerNG
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_zLoggerNG}" STREQUAL "ON")
