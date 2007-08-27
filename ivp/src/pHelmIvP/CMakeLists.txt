FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pHelmIvP "Build program pHelmIvP" ON)

IF("${IVP_BUILD_pHelmIvP}" STREQUAL "ON")
   ADD_EXECUTABLE(pHelmIvP ${SRC})
   
   TARGET_LINK_LIBRARIES(pHelmIvP 
   MOOS
   MOOSGen
   helmivp
   behaviors-marine
   behaviors-sandbox
   bhvutil	
   behaviors
   mbutil 
   ivpbuild
   ivpcore 
   geometry
   logic
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pHelmIvP: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pHelmIvP
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pHelmIvP}" STREQUAL "ON")
