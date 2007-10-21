FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pDynamicWaypointLL "Build program pDynamicWaypointLL" ON)

IF("${IVP_BUILD_pDynamicWaypointLL}" STREQUAL "ON")
   ADD_EXECUTABLE(pDynamicWaypointLL ${SRC})
   
   TARGET_LINK_LIBRARIES(pDynamicWaypointLL 
   MOOS
   MOOSGen
   MOOSUtility
   anrp_util
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pDynamicWaypointLL: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pDynamicWaypointLL
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pDynamicWaypointLL}" STREQUAL "ON")
