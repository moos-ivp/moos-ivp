OPTION(IVP_BUILD_abahr_util "Build library abahr_util" OFF)

IF("${IVP_BUILD_abahr_util}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(abahr_util ${SRC})

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "abahr_util: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS abahr_util
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT none)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_abahr_util
         COMPONENT none
         )

   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_abahr_util}" STREQUAL "ON")
