FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_uTMS "Build program uTMS" ON)

IF("${IVP_BUILD_uTMS}" STREQUAL "ON")
   ADD_EXECUTABLE(uTMS ${SRC})
   
   TARGET_LINK_LIBRARIES(uTMS 
   MOOS
   MOOSGen
   genutil
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "uTMS: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS uTMS
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_uTMS}" STREQUAL "ON")
