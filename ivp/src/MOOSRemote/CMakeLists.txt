FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_MOOSRemote "Build program MOOSRemote" ON)

IF("${IVP_BUILD_MOOSRemote}" STREQUAL "ON")
   ADD_EXECUTABLE(MOOSRemote ${SRC})
   
   TARGET_LINK_LIBRARIES(MOOSRemote 
   MOOS
   MOOSGen
   mbutil
   curses
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "MOOSRemote: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS MOOSRemote
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_MOOSRemote}" STREQUAL "ON")
