FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pEchoVar "Build program pEchoVar" ON)

IF("${IVP_BUILD_pEchoVar}" STREQUAL "ON")
   ADD_EXECUTABLE(pEchoVar ${SRC})
   
   TARGET_LINK_LIBRARIES(pEchoVar 
   MOOS
   MOOSGen
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pEchoVar: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pEchoVar
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pEchoVar}" STREQUAL "ON")
