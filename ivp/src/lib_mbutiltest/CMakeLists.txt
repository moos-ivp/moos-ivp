OPTION(IVP_BUILD_mbutiltest "Build library mbutiltest" ON)

IF("${IVP_BUILD_mbutiltest}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(mbutiltest ${SRC})

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "mbutiltest: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS mbutiltest
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools-dev)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_mbutiltest
         COMPONENT ivp-tools-dev
         )

   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_mbutiltest}" STREQUAL "ON")
