FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pSensorSim "Build program pSensorSim" ON)

IF("${IVP_BUILD_pSensorSim}" STREQUAL "ON")
   ADD_EXECUTABLE(pSensorSim ${SRC})
   
   TARGET_LINK_LIBRARIES(pSensorSim 
   MOOS
   MOOSGen
   mbutil
   m
   pthread
   artifacts
   geometry)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pSensorSim: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pSensorSim
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pSensorSim}" STREQUAL "ON")
