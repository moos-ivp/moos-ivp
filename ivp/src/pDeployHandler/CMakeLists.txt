FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pDeployHandler "Build program pDeployHandler" ON)

IF("${IVP_BUILD_pDeployHandler}" STREQUAL "ON")
   ADD_EXECUTABLE(pDeployHandler ${SRC})
   
   TARGET_LINK_LIBRARIES(pDeployHandler 
   MOOS
   MOOSGen
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pDeployHandler: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pDeployHandler
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pDeployHandler}" STREQUAL "ON")
