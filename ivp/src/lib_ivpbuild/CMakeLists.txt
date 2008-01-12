#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_ivpbuild
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

OPTION(IVP_BUILD_ivpbuild "Build library ivpbuild" ON)

IF("${IVP_BUILD_ivpbuild}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(ivpbuild ${SRC})

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS ivpbuild
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core-dev)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_ivpbuild
         COMPONENT ivp-core-dev
         )

   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_ivpbuild}" STREQUAL "ON")
