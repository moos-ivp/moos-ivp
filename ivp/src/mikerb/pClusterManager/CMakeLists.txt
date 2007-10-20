FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pClusterManager "Build program pClusterManager" ON)

IF("${IVP_BUILD_pClusterManager}" STREQUAL "ON")
   ADD_EXECUTABLE(pClusterManager ${SRC})
   
   TARGET_LINK_LIBRARIES(pClusterManager 
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pClusterManager: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pClusterManager
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pClusterManager}" STREQUAL "ON")
