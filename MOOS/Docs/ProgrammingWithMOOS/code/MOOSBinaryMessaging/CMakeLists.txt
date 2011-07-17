PROJECT(MOOSBinaryMessaging)


#what files are needed?
SET(SRCS
main.cpp
)

# Add executable  
ADD_EXECUTABLE( binary_comms_example  ${SRCS})

TARGET_LINK_LIBRARIES( binary_comms_example ${MOOSLIBS} ${PLATFORM_LIBS} )
