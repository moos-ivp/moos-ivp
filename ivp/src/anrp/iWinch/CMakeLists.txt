FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iWinch "Build program iWinch" ON)

IF("${IVP_BUILD_iWinch}" STREQUAL "ON")
   ADD_EXECUTABLE(iWinch ${SRC})
   
   TARGET_LINK_LIBRARIES(iWinch 
   MOOS
   MOOSGen
   anrp_util
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "iWinch: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS iWinch
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_iWinch}" STREQUAL "ON")
