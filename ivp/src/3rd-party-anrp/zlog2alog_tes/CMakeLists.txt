FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_zlog2alog_tes "Build program zlog2alog_tes" ON)

IF("${IVP_BUILD_zlog2alog_tes}" STREQUAL "ON")
   ADD_EXECUTABLE(zlog2alog_tes ${SRC})
   
   TARGET_LINK_LIBRARIES(zlog2alog_tes 
   anrp_util
   z
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "zlog2alog_tes: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS zlog2alog_tes
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT none)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_zlog2alog_tes}" STREQUAL "ON")
