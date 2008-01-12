#--------------------------------------------------------
# The CMakeLists.txt for:            lib_behaviors-marine
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

OPTION(IVP_BUILD_behaviors-marine "Build library behaviors-marine" ON)

IF("${IVP_BUILD_behaviors-marine}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(behaviors-marine ${SRC})

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS behaviors-marine
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core-dev)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_behaviors-marine
         COMPONENT ivp-core-dev
         )

   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_behaviors-marine}" STREQUAL "ON")
