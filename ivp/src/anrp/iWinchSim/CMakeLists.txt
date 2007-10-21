FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iWinchSim "Build program iWinchSim" ON)

IF("${IVP_BUILD_iWinchSim}" STREQUAL "ON")
   ADD_EXECUTABLE(iWinchSim ${SRC})
   
   TARGET_LINK_LIBRARIES(iWinchSim 
   MOOS
   MOOSGen
   anrp_util
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "iWinchSim: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS iWinchSim
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_iWinchSim}" STREQUAL "ON")
