FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iHeadingMonitor "Build program iHeadingMonitor" ON)

IF("${IVP_BUILD_iHeadingMonitor}" STREQUAL "ON")
   ADD_EXECUTABLE(iHeadingMonitor ${SRC})
   
   TARGET_LINK_LIBRARIES(iHeadingMonitor 
   MOOS
   MOOSGen
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "iHeadingMonitor: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS iHeadingMonitor
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_iHeadingMonitor}" STREQUAL "ON")
