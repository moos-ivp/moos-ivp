FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_peval "Build program peval" ON)

IF("${IVP_BUILD_peval}" STREQUAL "ON")
   ADD_EXECUTABLE(peval ${SRC})
   
   TARGET_LINK_LIBRARIES(peval 
   navplot
   mbutil
   geometry
   m)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "peval: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS peval
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_peval}" STREQUAL "ON")
