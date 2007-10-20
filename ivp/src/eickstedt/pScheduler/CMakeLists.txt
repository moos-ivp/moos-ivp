FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pScheduler "Build program pScheduler" ON)

IF("${IVP_BUILD_pScheduler}" STREQUAL "ON")
   ADD_EXECUTABLE(pScheduler ${SRC})
   
   TARGET_LINK_LIBRARIES(pScheduler 
   MOOS
   MOOSGen
   genutil
   mbutil
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pScheduler: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pScheduler
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pScheduler}" STREQUAL "ON")
