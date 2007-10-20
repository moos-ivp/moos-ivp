FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iPWMController "Build program iPWMController" ON)

IF("${IVP_BUILD_iPWMController}" STREQUAL "ON")
   ADD_EXECUTABLE(iPWMController ${SRC})
   
   TARGET_LINK_LIBRARIES(iPWMController 
   MOOS
   MOOSGen
   anrp_util
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "iPWMController: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS iPWMController
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_iPWMController}" STREQUAL "ON")
