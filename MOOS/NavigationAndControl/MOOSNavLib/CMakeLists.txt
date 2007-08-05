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

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

INSTALL(
   TARGETS MOOSNav
   DESTINATION lib/moos-${MOOS_VERSION}
   COMPONENT Moos_NavigationAndControl_Dev)

# Note: If there are any header files that are truly unneeded by users of this
# library, it should be OK to remove them from the PUBLIC_HEADERS variable,
# which in turn will prevent them from being installed. -CJC
SET(PUBLIC_HEADERS 
   LBLMaths.h
   MOOSLinearLeastMedianFilter.h
   MOOSNavBase.h
   MOOSNavBBCMLEngine.h
   MOOSNavBeacon.h
   MOOSNavBroker.h
   MOOSNavEKFEngine.h
   MOOSNavEngine.h
   MOOSNavEntity.h
   MOOSNavLibDefs.h
   MOOSNavLibGlobalHelper.h
   MOOSNavLib.h
   MOOSNavLogger.h
   MOOSNavLSQEngine.h
   MOOSNavObsStore.h
   MOOSNavSensor.h
   MOOSNavTopDownCalEngine.h
   MOOSNavVehicle.h
   MOOSObservation.h
   MOOSSensorChannel.h
)

INSTALL(
   FILES ${PUBLIC_HEADERS}
   DESTINATION include/moos-${MOOS_VERSION}/NavigationAndControl/MOOSNavLib
   COMPONENT Moos_NavigationAndControl_Dev)

#===============================================================================
