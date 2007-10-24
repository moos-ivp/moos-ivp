OPTION(IVP_BUILD_anrp_util "Build library anrp_util" ON)

IF("${IVP_BUILD_anrp_util}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(anrp_util ${SRC})

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "anrp_util: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS anrp_util
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core-dev)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_anrp_util
         COMPONENT ivp-core-dev
         )

   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_anrp_util}" STREQUAL "ON")
