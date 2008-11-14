PROJECT(EX4)


#what files are needed?
SET(EX4_SRCS
Simulator.cpp
Simulator.h
main.cpp
)

# Add executable called MOOSDB
ADD_EXECUTABLE( Ex4  ${EX4_SRCS})

# link against libMOOS and libMOOSGen (which should be in your LIBRARY_PATH)
TARGET_LINK_LIBRARIES(Ex4 ${MOOSLIBS} ${PLATFORM_LIBS} )
