FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pLawnmower "Build program pLawnmower" ON)

IF("${IVP_BUILD_pLawnmower}" STREQUAL "ON")
   ADD_EXECUTABLE(pLawnmower ${SRC})
   
   TARGET_LINK_LIBRARIES(pLawnmower 
   MOOS
   MOOSGen
   geometry
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pLawnmower: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pLawnmower
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pLawnmower}" STREQUAL "ON")
