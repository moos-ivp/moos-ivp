FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_logparse "Build program logparse" ON)

IF("${IVP_BUILD_logparse}" STREQUAL "ON")
   ADD_EXECUTABLE(logparse ${SRC})
   
   TARGET_LINK_LIBRARIES(logparse 
   mbutil
   m)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "logparse: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS logparse
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_logparse}" STREQUAL "ON")
