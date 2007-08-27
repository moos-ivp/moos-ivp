FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pMarinePID "Build program pMarinePID" ON)

IF("${IVP_BUILD_pMarinePID}" STREQUAL "ON")
   ADD_EXECUTABLE(pMarinePID ${SRC})
   
   TARGET_LINK_LIBRARIES(pMarinePID 
   MOOS
   MOOSGen
   mbutil
   geometry
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pMarinePID: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pMarinePID
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pMarinePID}" STREQUAL "ON")
