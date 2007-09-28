FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pTrafficControl "Build program pTrafficControl" OFF)

IF("${IVP_BUILD_pTrafficControl}" STREQUAL "ON")
   ADD_EXECUTABLE(pTrafficControl ${SRC})
   
   TARGET_LINK_LIBRARIES(pTrafficControl 
   MOOS
   MOOSGen
   geometry
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pTrafficControl: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pTrafficControl
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pTrafficControl}" STREQUAL "ON")
