PROJECT(IVP_LIB_BEHAVIORS)

FILE(GLOB SRC *.cpp)

ADD_LIBRARY(behaviors ${SRC})

# Do I need this? -CJC
TARGET_LINK_LIBRARIES(behaviors logic)
