#--------------------------------------------------------
# The CMakeLists.txt for:                     lib_bhvutil
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

OPTION(IVP_BUILD_bhvutil "Build library bhvutil" ON)

IF("${IVP_BUILD_bhvutil}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(bhvutil ${SRC})

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS bhvutil
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core-dev)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_bhvutil
         COMPONENT ivp-core-dev
         )

   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_bhvutil}" STREQUAL "ON")
