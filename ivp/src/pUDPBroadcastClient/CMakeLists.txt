FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pUDPBroadcastClient "Build program pUDPBroadcastClient" ON)

IF("${IVP_BUILD_pUDPBroadcastClient}" STREQUAL "ON")
   ADD_EXECUTABLE(pUDPBroadcastClient ${SRC})
   
   TARGET_LINK_LIBRARIES(pUDPBroadcastClient 
   MOOS
   MOOSGen
   opt
   newmat
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pUDPBroadcastClient: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pUDPBroadcastClient
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pUDPBroadcastClient}" STREQUAL "ON")
