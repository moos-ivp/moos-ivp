FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pExtTracker "Build program pExtTracker" ON)

IF("${IVP_BUILD_pExtTracker}" STREQUAL "ON")
   ADD_EXECUTABLE(pExtTracker ${SRC})
   
   TARGET_LINK_LIBRARIES(pExtTracker 
   MOOS
   MOOSGen
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pExtTracker: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pExtTracker
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pExtTracker}" STREQUAL "ON")
