SET(EXEC uPoke)

#what files are needed?
SET(SRCS 	
	main.cpp)

FIND_PACKAGE(MOOS REQUIRED)

#where to look for help
INCLUDE_DIRECTORIES( {MOOS_INCLUDE_DIRS} ${MOOS_DEPEND_INCLUDE_DIRS} )

# Add executable called pLogger
ADD_EXECUTABLE( ${EXEC} ${SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(${EXEC}   ${MOOS_LIBRARIES} ${MOOS_DEPEND_LIBRARIES}  )

#install directive
INSTALL(TARGETS ${EXEC} RUNTIME DESTINATION bin )

