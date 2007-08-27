FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pBearings "Build program pBearings" ON)

IF("${IVP_BUILD_pBearings}" STREQUAL "ON")
   ADD_EXECUTABLE(pBearings ${SRC})
   
   TARGET_LINK_LIBRARIES(pBearings 
   MOOS
   MOOSGen
   opt
   newmat10D
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "pBearings: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pBearings
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pBearings}" STREQUAL "ON")
