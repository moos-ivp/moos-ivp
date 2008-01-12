#--------------------------------------------------------
# The CMakeLists.txt for:           lib_behaviors-colregs
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

OPTION(IVP_BUILD_behaviors-colregs "Build library behaviors-colregs" ON)

IF("${IVP_BUILD_behaviors-colregs}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(behaviors-colregs ${SRC})

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS behaviors-colregs
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core-dev)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_behaviors-colregs
         COMPONENT ivp-core-dev
         )

   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_behaviors-colregs}" STREQUAL "ON")
