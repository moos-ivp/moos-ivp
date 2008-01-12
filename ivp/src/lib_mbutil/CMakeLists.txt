#--------------------------------------------------------
# The CMakeLists.txt for:                      lib_mbutil
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

OPTION(IVP_BUILD_mbutil "Build library mbutil" ON)

IF("${IVP_BUILD_mbutil}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(mbutil ${SRC})

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS mbutil
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core-dev)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_mbutil
         COMPONENT ivp-core-dev
         )

   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_mbutil}" STREQUAL "ON")
