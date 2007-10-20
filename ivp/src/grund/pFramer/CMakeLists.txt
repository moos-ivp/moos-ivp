FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pFramer "Build program pFramer" ON)

IF("${IVP_BUILD_pFramer}" STREQUAL "ON")
   ADD_EXECUTABLE(pFramer ${SRC})
   
   TARGET_LINK_LIBRARIES(pFramer 
   MOOS
   MOOSGen
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pFramer: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pFramer
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pFramer}" STREQUAL "ON")
