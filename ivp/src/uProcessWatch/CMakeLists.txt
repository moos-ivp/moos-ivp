FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_uProcessWatch "Build program uProcessWatch" ON)

IF("${IVP_BUILD_uProcessWatch}" STREQUAL "ON")
   ADD_EXECUTABLE(uProcessWatch ${SRC})
   
   TARGET_LINK_LIBRARIES(uProcessWatch 
   MOOS
   MOOSGen
   mbutil
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "uProcessWatch: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS uProcessWatch
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_uProcessWatch}" STREQUAL "ON")
