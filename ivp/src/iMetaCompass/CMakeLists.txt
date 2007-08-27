FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iMetaCompass "Build program iMetaCompass" ON)

IF("${IVP_BUILD_iMetaCompass}" STREQUAL "ON")
   ADD_EXECUTABLE(iMetaCompass ${SRC})
   
   TARGET_LINK_LIBRARIES(iMetaCompass 
   MOOS
   MOOSGen
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "iMetaCompass: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS iMetaCompass
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_iMetaCompass}" STREQUAL "ON")
