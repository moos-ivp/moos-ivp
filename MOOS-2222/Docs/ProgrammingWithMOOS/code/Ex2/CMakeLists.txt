PROJECT(Ex2)


#what files are needed?
SET(EX2_SRCS
SimpleApp.cpp
SimpleApp.h
main.cpp
)

# Add executable called MOOSDB
ADD_EXECUTABLE( Ex2  ${EX2_SRCS})

# link against libMOOS and libMOOSGen (which should be in your LIBRARY_PATH)
TARGET_LINK_LIBRARIES(Ex2 ${MOOSLIBS} ${PLATFORM_LIBS} )
