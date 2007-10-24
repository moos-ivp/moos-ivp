OPTION(IVP_BUILD_behaviors-attic "Build library behaviors-attic" ON)

IF("${IVP_BUILD_behaviors-attic}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(behaviors-attic ${SRC})

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "behaviors-attic: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS behaviors-attic
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT none)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_behaviors-attic
         COMPONENT none
         )

   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_behaviors-attic}" STREQUAL "ON")
