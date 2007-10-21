FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pRouter "Build program pRouter" ON)

IF("${IVP_BUILD_pRouter}" STREQUAL "ON")
   ADD_EXECUTABLE(pRouter ${SRC})
   
   TARGET_LINK_LIBRARIES(pRouter 
   MOOS
   MOOSGen
   genutil
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pRouter: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pRouter
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT none)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pRouter}" STREQUAL "ON")
