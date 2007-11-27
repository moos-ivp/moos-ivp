FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_artfieldgenerator "Build program artfieldgenerator" ON)

IF("${IVP_BUILD_artfieldgenerator}" STREQUAL "ON")
   ADD_EXECUTABLE(artfieldgenerator ${SRC})
   
   TARGET_LINK_LIBRARIES(artfieldgenerator 
      MOOS 
      MOOSGen
      geometry
      mbutil)

   IF("${IVP_PACKAGE_VERSION}" STREQUAL "")
      MESSAGE(STATUS "artfieldgenerator: IVP_PACKAGE_VERSION not set, so not creating install component.")
   ELSE("${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS artfieldgenerator
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-tools)
   ENDIF("${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_artfieldgenerator}" STREQUAL "ON")
