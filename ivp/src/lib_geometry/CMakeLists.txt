#--------------------------------------------------------
# The CMakeLists.txt for:                    lib_geometry
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

OPTION(IVP_BUILD_geometry "Build library geometry" ON)

IF("${IVP_BUILD_geometry}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(geometry ${SRC})

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS geometry
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core-dev)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/lib_geometry
         COMPONENT ivp-core-dev
         )

   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_geometry}" STREQUAL "ON")
