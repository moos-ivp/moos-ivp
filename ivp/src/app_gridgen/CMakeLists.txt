FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_gridgen "Build program gridgen" ON)

IF("${IVP_BUILD_gridgen}" STREQUAL "ON")
   ADD_EXECUTABLE(gridgen ${SRC})
   
   TARGET_LINK_LIBRARIES(gridgen 
   ivpbuild
   ivpcore
   geometry
   mbutil
   m)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "gridgen: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS gridgen
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_gridgen}" STREQUAL "ON")
