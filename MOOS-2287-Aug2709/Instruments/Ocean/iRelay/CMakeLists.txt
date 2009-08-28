PROJECT(iRelay)


#what files are needed?
SET(IRELAY_SRCS
 RelayBoard.cpp RelayBoard.h
 iRelayMain.cpp  
)

# Add executable called iDepth
ADD_EXECUTABLE( iRelay ${IRELAY_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES( iRelay ${MOOSLIBS} ${PLATFORM_LIBS} )
