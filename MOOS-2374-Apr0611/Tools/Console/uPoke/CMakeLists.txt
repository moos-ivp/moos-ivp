PROJECT(uPoke)

#what files are needed?
SET(SRCS 	
	main.cpp)


# Add executable called uMVS
ADD_EXECUTABLE( uPoke ${SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(uPoke ${MOOSLIBS} ${PLATFORM_LIBS} )
