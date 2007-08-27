FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_logclip "Build program logclip" ON)

IF("${IVP_BUILD_logclip}" STREQUAL "ON")
   ADD_EXECUTABLE(logclip ${SRC})
   
   TARGET_LINK_LIBRARIES(logclip 
   mbutil
   m)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "logclip: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS logclip
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_logclip}" STREQUAL "ON")
