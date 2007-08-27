FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_ltest "Build program ltest" ON)

IF("${IVP_BUILD_ltest}" STREQUAL "ON")
   ADD_EXECUTABLE(ltest ${SRC})
   
   TARGET_LINK_LIBRARIES(ltest 
   logic
   mbutil
   m)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "ltest: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS ltest
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_ltest}" STREQUAL "ON")
