OPTION(IVP_BUILD_artifacts "Build library artifacts" ON)

IF("${IVP_BUILD_artifacts}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(artifacts ${SRC})

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "artifacts: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS artifacts
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools-dev)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_artifacts
         COMPONENT ivp-tools-dev
         )

   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_artifacts}" STREQUAL "ON")
