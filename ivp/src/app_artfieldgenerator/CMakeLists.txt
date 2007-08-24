FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_artfieldgenerator "Build program artfieldgenerator" ON)

IF("${IVP_BUILD_artfieldgenerator}" STREQUAL "ON")
   ADD_EXECUTABLE(artfieldgenerator ${SRC})
   
   TARGET_LINK_LIBRARIES(artfieldgenerator MOOS MOOSGen mbutil geometry)
ENDIF("${IVP_BUILD_artfieldgenerator}" STREQUAL "ON")
