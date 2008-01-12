#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_ipfview
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

OPTION(IVP_BUILD_ipfview "Build library ipfview" ON)

IF("${IVP_BUILD_ipfview}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(ipfview ${SRC})

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS ipfview
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools-dev)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_ipfview
         COMPONENT ivp-tools-dev
         )

   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_ipfview}" STREQUAL "ON")
