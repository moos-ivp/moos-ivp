#--------------------------------------------------------
# The CMakeLists.txt for:           lib_behaviors-sandbox
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

OPTION(IVP_BUILD_behaviors-sandbox "Build library behaviors-sandbox" ON)

IF("${IVP_BUILD_behaviors-sandbox}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(behaviors-sandbox ${SRC})

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS behaviors-sandbox
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT none)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_behaviors-sandbox
         COMPONENT none
         )

   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_behaviors-sandbox}" STREQUAL "ON")
