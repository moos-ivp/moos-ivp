FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iMicroModem "Build program iMicroModem" ON)

IF("${IVP_BUILD_iMicroModem}" STREQUAL "ON")
   ADD_EXECUTABLE(iMicroModem ${SRC})
   
   TARGET_LINK_LIBRARIES(iMicroModem 
   MOOS 
   MOOSGen
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "iMicroModem: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS iMicroModem
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_iMicroModem}" STREQUAL "ON")
