#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_ivpcore
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

OPTION(IVP_BUILD_ivpcore "Build library ivpcore" ON)

IF("${IVP_BUILD_ivpcore}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(ivpcore ${SRC})

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS ivpcore
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core-dev)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_ivpcore
         COMPONENT ivp-core-dev
         )

   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_ivpcore}" STREQUAL "ON")
