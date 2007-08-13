FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(artfieldgenerator ${SRC})

TARGET_LINK_LIBRARIES(artfieldgenerator MOOS MOOSGen mbutil)
