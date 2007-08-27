FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_p2BTracker "Build program p2BTracker" ON)

IF("${IVP_BUILD_p2BTracker}" STREQUAL "ON")
   ADD_EXECUTABLE(p2BTracker ${SRC})
   
   TARGET_LINK_LIBRARIES(p2BTracker 
   MOOS
   MOOSGen
   opt
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "p2BTracker: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS p2BTracker
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_p2BTracker}" STREQUAL "ON")
