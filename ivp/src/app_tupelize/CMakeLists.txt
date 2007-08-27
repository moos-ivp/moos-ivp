FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_tupelize "Build program tupelize" ON)

IF("${IVP_BUILD_tupelize}" STREQUAL "ON")
   ADD_EXECUTABLE(tupelize ${SRC})
   
   TARGET_LINK_LIBRARIES(tupelize 
   mbutil
   ivpbuild
   ivpcore
   m)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "tupelize: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS tupelize
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_tupelize}" STREQUAL "ON")
