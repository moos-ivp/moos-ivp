FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_zlog2alog "Build program zlog2alog" ON)

IF("${IVP_BUILD_zlog2alog}" STREQUAL "ON")
   ADD_EXECUTABLE(zlog2alog ${SRC})
   
   TARGET_LINK_LIBRARIES(zlog2alog 
   anrp_util
   m
   pthread
   z)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "zlog2alog: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS zlog2alog
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_zlog2alog}" STREQUAL "ON")
