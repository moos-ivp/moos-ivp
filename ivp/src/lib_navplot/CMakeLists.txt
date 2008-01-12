#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_navplot
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

OPTION(IVP_BUILD_navplot "Build library navplot" ON)

IF("${IVP_BUILD_navplot}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(navplot ${SRC})

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS navplot
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools-dev)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_navplot
         COMPONENT ivp-tools-dev
         )

   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_navplot}" STREQUAL "ON")
