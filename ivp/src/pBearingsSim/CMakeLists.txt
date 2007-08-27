FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pBearingsSim "Build program pBearingsSim" ON)

IF("${IVP_BUILD_pBearingsSim}" STREQUAL "ON")
   ADD_EXECUTABLE(pBearingsSim ${SRC})
   
   TARGET_LINK_LIBRARIES(pBearingsSim 
   MOOS
   MOOSGen
   opt
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pBearingsSim: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pBearingsSim
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pBearingsSim}" STREQUAL "ON")
