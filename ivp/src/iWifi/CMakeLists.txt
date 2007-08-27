FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iWifi "Build program iWifi" ON)

IF("${IVP_BUILD_iWifi}" STREQUAL "ON")
   ADD_EXECUTABLE(iWifi ${SRC})
   
   TARGET_LINK_LIBRARIES(iWifi 
   MOOS
   MOOSGen
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "iWifi: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS iWifi
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_iWifi}" STREQUAL "ON")
