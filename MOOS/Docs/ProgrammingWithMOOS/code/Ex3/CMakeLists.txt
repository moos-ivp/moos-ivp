PROJECT(EX3)


#what files are needed?
SET(EX3_SRCS
Simulator.cpp
Simulator.h
main.cpp
)

# Add executable called MOOSDB
ADD_EXECUTABLE( Ex3  ${EX3_SRCS})

# indicate how to link - these variables are defined in the top level
# CMakeLists.txt
TARGET_LINK_LIBRARIES(Ex3 ${MOOSLIBS} ${PLATFORM_LIBS} )
