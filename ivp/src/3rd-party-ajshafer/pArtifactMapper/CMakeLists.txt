FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pArtifactMapper "Build program pArtifactMapper" ON)

IF("${IVP_BUILD_pArtifactMapper}" STREQUAL "ON")
   ADD_EXECUTABLE(pArtifactMapper ${SRC})
   
   TARGET_LINK_LIBRARIES(pArtifactMapper 
   MOOS
   MOOSGen
   artifacts
   geometry
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pArtifactMapper: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pArtifactMapper
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pArtifactMapper}" STREQUAL "ON")
