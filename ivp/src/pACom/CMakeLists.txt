FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pACom "Build program pACom" ON)

IF("${IVP_BUILD_pACom}" STREQUAL "ON")
   ADD_EXECUTABLE(pACom ${SRC})
   
   TARGET_LINK_LIBRARIES(pACom 
   MOOS
   MOOSGen
   MOOSUtility
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pACom: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pACom
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pACom}" STREQUAL "ON")
