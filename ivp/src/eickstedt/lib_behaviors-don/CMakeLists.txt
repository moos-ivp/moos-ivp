OPTION(IVP_BUILD_behaviors-don "Build library behaviors-don" ON)

IF("${IVP_BUILD_behaviors-don}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(behaviors-don ${SRC})

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "behaviors-don: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS behaviors-don
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core-dev)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_behaviors-don
         COMPONENT ivp-core-dev
         )

   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_behaviors-don}" STREQUAL "ON")
