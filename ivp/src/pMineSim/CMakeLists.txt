FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pMineSim "Build program pMineSim" ON)

IF("${IVP_BUILD_pMineSim}" STREQUAL "ON")
   ADD_EXECUTABLE(pMineSim ${SRC})
   
   TARGET_LINK_LIBRARIES(pMineSim 
   MOOS
   MOOSGen
   geometry
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pMineSim: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pMineSim
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pMineSim}" STREQUAL "ON")
