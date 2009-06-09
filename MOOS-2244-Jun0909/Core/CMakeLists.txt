SET(CORE_DIRS  MOOSGenLib MOOSLIB MOOSDB )

IF(UNIX)
	#this is a new addition allowing added for gcc to build
	#shared libraries against the MOOS libs..
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
ENDIF(UNIX)

SUBDIRS(${CORE_DIRS})