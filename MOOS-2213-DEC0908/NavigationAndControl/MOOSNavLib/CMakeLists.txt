PROJECT(MOOSNavLib)

SET(MOOSNAVLIB_SRCS
CMOOSNavLibGlobalHelper.cpp
LBLMaths.cpp
MOOSLinearLeastMedianFilter.cpp
MOOSNavBase.cpp
MOOSNavBBCMLEngine.cpp
MOOSNavBeacon.cpp
MOOSNavBroker.cpp
MOOSNavEKFEngine.cpp
MOOSNavEngine.cpp
MOOSNavEntity.cpp
MOOSNavLogger.cpp
MOOSNavLSQEngine.cpp
MOOSNavObsStore.cpp
MOOSNavSensor.cpp
MOOSNavTopDownCalEngine.cpp
MOOSNavVehicle.cpp
MOOSObservation.cpp
MOOSSensorChannel.cpp
MOOSNavLibGlobalHelper.h
LBLMaths.h
MOOSLinearLeastMedianFilter.h
MOOSNavBase.h
MOOSNavBBCMLEngine.h
MOOSNavBeacon.h
MOOSNavBroker.h
MOOSNavEKFEngine.h
MOOSNavEngine.h
MOOSNavEntity.h
MOOSNavLogger.h
MOOSNavLSQEngine.h
MOOSNavObsStore.h
MOOSNavSensor.h
MOOSNavTopDownCalEngine.h
MOOSNavVehicle.h
MOOSObservation.h
MOOSSensorChannel.h
)


INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/newmat ${THIRDPARTY_DIR})

ADD_LIBRARY(MOOSNav ${MOOSNAVLIB_SRCS})
