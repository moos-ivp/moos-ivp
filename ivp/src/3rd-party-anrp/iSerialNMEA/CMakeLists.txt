FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iSerialNMEA "Build program iSerialNMEA" ON)

IF("${IVP_BUILD_iSerialNMEA}" STREQUAL "ON")
   ADD_EXECUTABLE(iSerialNMEA ${SRC})
   
   TARGET_LINK_LIBRARIES(iSerialNMEA 
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "iSerialNMEA: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS iSerialNMEA
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_iSerialNMEA}" STREQUAL "ON")
