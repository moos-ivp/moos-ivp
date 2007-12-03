FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iMarineSim "Build program iMarineSim" ON)

IF("${IVP_BUILD_iMarineSim}" STREQUAL "ON")
   ADD_EXECUTABLE(iMarineSim ${SRC})
   
   TARGET_LINK_LIBRARIES(iMarineSim 
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   geometry
   m
   pthread)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "iMarineSim: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS iMarineSim
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_iMarineSim}" STREQUAL "ON")
