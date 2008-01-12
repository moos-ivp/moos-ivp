#--------------------------------------------------------
# The CMakeLists.txt for:                    pViewerRelay
# Author(s):                                Mike Benjamin
#--------------------------------------------------------

FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pViewerRelay "Build program pViewerRelay" ON)

IF("${IVP_BUILD_pViewerRelay}" STREQUAL "ON")
   ADD_EXECUTABLE(pViewerRelay ${SRC})
   
   TARGET_LINK_LIBRARIES(pViewerRelay 
   MOOS
   MOOSGen
   MOOSUtility
   mbutil
   m
   pthread)

   IF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")
      INSTALL(
         TARGETS pViewerRelay
         DESTINATION bin/ivp-${IVP_PACKAGE_VERSION}
         COMPONENT ivp-core)
   ENDIF(NOT "${IVP_PACKAGE_VERSION}" STREQUAL "")

ENDIF("${IVP_BUILD_pViewerRelay}" STREQUAL "ON")
