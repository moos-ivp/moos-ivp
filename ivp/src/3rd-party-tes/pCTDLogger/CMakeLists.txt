FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pCTDLogger "Build program pCTDLogger" ON)

IF("${IVP_BUILD_pCTDLogger}" STREQUAL "ON")
   ADD_EXECUTABLE(pCTDLogger ${SRC})
   
   TARGET_LINK_LIBRARIES(pCTDLogger 
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pCTDLogger: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pCTDLogger
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT none)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pCTDLogger}" STREQUAL "ON")
