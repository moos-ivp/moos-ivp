FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pSoundSpeed "Build program pSoundSpeed" ON)

IF("${IVP_BUILD_pSoundSpeed}" STREQUAL "ON")
   ADD_EXECUTABLE(pSoundSpeed ${SRC})
   
   TARGET_LINK_LIBRARIES(pSoundSpeed 
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pSoundSpeed: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pSoundSpeed
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT none)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pSoundSpeed}" STREQUAL "ON")
