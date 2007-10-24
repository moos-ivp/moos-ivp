FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_uMessageForward "Build program uMessageForward" ON)

IF("${IVP_BUILD_uMessageForward}" STREQUAL "ON")
   ADD_EXECUTABLE(uMessageForward ${SRC})
   
   TARGET_LINK_LIBRARIES(uMessageForward 
   MOOS
   MOOSGen
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "uMessageForward: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS uMessageForward
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_uMessageForward}" STREQUAL "ON")
