FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iWinchCtl "Build program iWinchCtl" ON)

IF("${IVP_BUILD_iWinchCtl}" STREQUAL "ON")
   ADD_EXECUTABLE(iWinchCtl ${SRC})
   
   TARGET_LINK_LIBRARIES(iWinchCtl 
   MOOS
   MOOSGen
   anrp_util
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "iWinchCtl: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS iWinchCtl
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_iWinchCtl}" STREQUAL "ON")
