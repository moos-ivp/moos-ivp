PROJECT(uMVS)

# next few lines are simply to insert icons into
# the console application on windows
IF(WIN32)
    SET(RESOURCES icon1.ico uMVS.rc)
ENDIF(WIN32)

#what files are needed?
SET(UMVS_SRCS 	
	AcousticBeacon.cpp 		AcousticBeacon.h
	AcousticIntersection.cpp 	AcousticIntersection.h 
	AcousticNode.cpp 		AcousticNode.h
	AcousticSignal.cpp 		AcousticSignal.h
	AcousticTransceiver.cpp         AcousticTransceiver.h
	AcousticResponder.cpp 		AcousticResponder.h 
	SimEntity.cpp 			SimEntity.h
	SimEnvironment.cpp 		SimEnvironment.h
	MVSim.cpp 			MVSim.h 
	SixDOFAUV.cpp 			SixDOFAUV.h
	SimBase.cpp 			SimBase.h 
	SimVehicle.cpp 			SimVehicle.h
	SimLoggable.cpp 		SimLoggable.h
	SimParams.cpp 			SimParams.h
	uMVSMain.cpp 			
	${RESOURCES})


SET( BUILD_NEWMAT ON CACHE BOOL "Build Newmat" FORCE)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR} ${THIRDPARTY_DIR}/newmat)

# Add executable called uMVS
ADD_EXECUTABLE( uMVS ${UMVS_SRCS})

# indicate how to link
TARGET_LINK_LIBRARIES(uMVS ${MOOSLIBS} ${PLATFORM_LIBS} newmat )
