#--------------------------------------------------------
# The CMakeLists.txt for:                       newmat10D
# Author(s):                                
#--------------------------------------------------------

OPTION(IVP_BUILD_newmat10D "Build library newmat10D" ON)

IF("${IVP_BUILD_newmat10D}" STREQUAL "ON")
   FILE(GLOB SRC *.cpp *.c)
   ADD_LIBRARY(newmat10D ${SRC})

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS newmat10D
         DESTINATION lib/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core-dev)

      FILE(GLOB IVP_PUBLIC_HEADERS *.h)

      INSTALL(
         FILES ${IVP_PUBLIC_HEADERS}
         DESTINATION include/ivp-${IVP_PACKAGE_VERSION}/newmat10D
         COMPONENT ivp-core-dev
         )

   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_newmat10D}" STREQUAL "ON")
