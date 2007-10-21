FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pWatchDog "Build program pWatchDog" ON)

IF("${IVP_BUILD_pWatchDog}" STREQUAL "ON")
   ADD_EXECUTABLE(pWatchDog ${SRC})
   
   TARGET_LINK_LIBRARIES(pWatchDog 
   MOOS
   MOOSGen
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pWatchDog: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pWatchDog
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT none)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pWatchDog}" STREQUAL "ON")
