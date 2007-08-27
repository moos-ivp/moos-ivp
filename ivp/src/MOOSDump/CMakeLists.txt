FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_MOOSDump "Build program MOOSDump" ON)

IF("${IVP_BUILD_MOOSDump}" STREQUAL "ON")
   ADD_EXECUTABLE(MOOSDump ${SRC})
   
   TARGET_LINK_LIBRARIES(MOOSDump 
   MOOS
   MOOSGen
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "MOOSDump: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS MOOSDump
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_MOOSDump}" STREQUAL "ON")
