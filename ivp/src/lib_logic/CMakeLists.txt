#--------------------------------------------------------
# The CMakeLists.txt for:                       lib_logic
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

OPTION(IVP_BUILD_logic "Build library logic" ON)

IF("${IVP_BUILD_logic}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(logic ${SRC})

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS logic
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core-dev)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_logic
         COMPONENT ivp-core-dev
         )

   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_logic}" STREQUAL "ON")
