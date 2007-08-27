FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pTarget "Build program pTarget" ON)

IF("${IVP_BUILD_pTarget}" STREQUAL "ON")
   ADD_EXECUTABLE(pTarget ${SRC})
   
   TARGET_LINK_LIBRARIES(pTarget 
   MOOS
   MOOSGen
   geometry
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pTarget: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pTarget
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pTarget}" STREQUAL "ON")
