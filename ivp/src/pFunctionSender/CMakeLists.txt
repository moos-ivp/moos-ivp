FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pFunctionSender "Build program pFunctionSender" ON)

IF("${IVP_BUILD_pFunctionSender}" STREQUAL "ON")
   ADD_EXECUTABLE(pFunctionSender ${SRC})
   
   TARGET_LINK_LIBRARIES(pFunctionSender 
   MOOS
   MOOSGen
   ivpbuild
   mbutil
   ivpcore
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pFunctionSender: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pFunctionSender
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pFunctionSender}" STREQUAL "ON")
