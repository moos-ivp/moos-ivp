FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_generatelawnmower "Build program generatelawnmower" ON)

IF("${IVP_BUILD_generatelawnmower}" STREQUAL "ON")
   ADD_EXECUTABLE(generatelawnmower ${SRC})
   
   TARGET_LINK_LIBRARIES(generatelawnmower 
      geometry
      mbutil)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "generatelawnmower: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS generatelawnmower
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_generatelawnmower}" STREQUAL "ON")
