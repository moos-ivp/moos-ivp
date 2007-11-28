FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pSamplingControl "Build program pSamplingControl" ON)

IF("${IVP_BUILD_pSamplingControl}" STREQUAL "ON")
   ADD_EXECUTABLE(pSamplingControl ${SRC})
   
   TARGET_LINK_LIBRARIES(pSamplingControl 
   MOOS
   MOOSGen
   MOOSUtility
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pSamplingControl: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pSamplingControl
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pSamplingControl}" STREQUAL "ON")
