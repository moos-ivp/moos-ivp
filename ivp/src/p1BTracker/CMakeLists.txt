FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_p1BTracker "Build program p1BTracker" ON)

IF("${IVP_BUILD_p1BTracker}" STREQUAL "ON")
   ADD_EXECUTABLE(p1BTracker ${SRC})
   
   TARGET_LINK_LIBRARIES(p1BTracker 
   MOOS
   MOOSGen
   opt
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "p1BTracker: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS p1BTracker
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_p1BTracker}" STREQUAL "ON")
