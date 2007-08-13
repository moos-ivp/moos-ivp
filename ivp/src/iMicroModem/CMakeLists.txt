FILE(GLOB SRC *.cpp)

OPTION(IVP_BUILD_iMicroModem "Build program iMicroModem" ON)

IF("${IVP_BUILD_iMicroModem}" STREQUAL "ON")
   ADD_EXECUTABLE(iMicroModem ${SRC})
   
   TARGET_LINK_LIBRARIES(iMicroModem 
   MOOS 
   MOOSGen
   pthread)
ENDIF("${IVP_BUILD_iMicroModem}" STREQUAL "ON")
