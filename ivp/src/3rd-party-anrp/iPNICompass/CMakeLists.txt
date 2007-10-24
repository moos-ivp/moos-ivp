FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iPNICompass "Build program iPNICompass" ON)

IF("${IVP_BUILD_iPNICompass}" STREQUAL "ON")
   ADD_EXECUTABLE(iPNICompass ${SRC})
   
   TARGET_LINK_LIBRARIES(iPNICompass 
   MOOS
   MOOSGen
   MOOSUtility
   anrp_util
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "iPNICompass: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS iPNICompass
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_iPNICompass}" STREQUAL "ON")
