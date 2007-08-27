FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pLoiterControl "Build program pLoiterControl" ON)

IF("${IVP_BUILD_pLoiterControl}" STREQUAL "ON")
   ADD_EXECUTABLE(pLoiterControl ${SRC})
   
   TARGET_LINK_LIBRARIES(pLoiterControl 
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pLoiterControl: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pLoiterControl
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pLoiterControl}" STREQUAL "ON")
