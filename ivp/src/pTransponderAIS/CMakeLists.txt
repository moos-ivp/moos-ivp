FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pTransponderAIS "Build program pTransponderAIS" ON)

IF("${IVP_BUILD_pTransponderAIS}" STREQUAL "ON")
   ADD_EXECUTABLE(pTransponderAIS ${SRC})
   
   TARGET_LINK_LIBRARIES(pTransponderAIS 
   MOOS
   MOOSGen
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pTransponderAIS: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pTransponderAIS
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pTransponderAIS}" STREQUAL "ON")
