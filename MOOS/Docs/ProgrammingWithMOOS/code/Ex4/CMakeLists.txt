PROJECT(EX4)


#what files are needed?
SET(EX4_SRCS
Simulator.cpp
Simulator.h
main.cpp
)

# Add executable called MOOSDB
ADD_EXECUTABLE( Ex4  ${EX4_SRCS})

# indicate how to link - these variables are defined in the top level
# CMakeLists.txt
TARGET_LINK_LIBRARIES(Ex4 ${MOOSLIBS} ${PLATFORM_LIBS} )
