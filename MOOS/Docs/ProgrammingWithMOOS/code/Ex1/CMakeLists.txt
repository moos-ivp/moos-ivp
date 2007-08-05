PROJECT(Ex1)


#what files are needed?
SET(EX1_SRCS
SimpleApp.cpp
SimpleApp.h
main.cpp
)

# Add executable called MOOSDB
ADD_EXECUTABLE( Ex1  ${EX1_SRCS})

# indicate how to link - these variables are defined in the top level
# CMakeLists.txt
TARGET_LINK_LIBRARIES(Ex1 ${MOOSLIBS} ${PLATFORM_LIBS} )
