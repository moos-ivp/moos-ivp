PROJECT(Ex2)


#what files are needed?
SET(EX2_SRCS
SimpleApp.cpp
SimpleApp.h
main.cpp
)

# Add executable called MOOSDB
ADD_EXECUTABLE( Ex2  ${EX2_SRCS})

# indicate how to link - these variables are defined in the top level
# CMakeLists.txt
TARGET_LINK_LIBRARIES(Ex2 ${MOOSLIBS} ${PLATFORM_LIBS} )
