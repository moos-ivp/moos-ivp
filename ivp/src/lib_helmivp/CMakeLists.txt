#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_helmivp
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

OPTION(IVP_BUILD_helmivp "Build library helmivp" ON)

IF("${IVP_BUILD_helmivp}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(helmivp ${SRC})

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS helmivp
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core-dev)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_helmivp
         COMPONENT ivp-core-dev
         )

   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_helmivp}" STREQUAL "ON")
