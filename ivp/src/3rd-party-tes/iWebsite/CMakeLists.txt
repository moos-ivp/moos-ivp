FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iWebsite "Build program iWebsite" ON)

IF("${IVP_BUILD_iWebsite}" STREQUAL "ON")
   ADD_EXECUTABLE(iWebsite ${SRC})
   
   TARGET_LINK_LIBRARIES(iWebsite 
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "iWebsite: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS iWebsite
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT none)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_iWebsite}" STREQUAL "ON")
