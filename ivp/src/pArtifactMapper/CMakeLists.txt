FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_pArtifactMapper "Build program pArtifactMapper" OFF)

IF("${IVP_BUILD_pArtifactMapper}" STREQUAL "ON")
   ADD_EXECUTABLE(pArtifactMapper ${SRC})
   
   TARGET_LINK_LIBRARIES(pArtifactMapper 
   MOOS
   MOOSGen
   artifacts
   geometry
   mbutil
   m
   pthread)
ENDIF("${IVP_BUILD_pArtifactMapper}" STREQUAL "ON")
