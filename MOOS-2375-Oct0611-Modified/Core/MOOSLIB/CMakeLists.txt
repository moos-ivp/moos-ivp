PROJECT(MOOSLib)

#do we want to turn on the experimental clock skew detection?
SET(DETECT_CLOCK_DRIFT FALSE CACHE BOOL "MOOSCommClient<->MOOSDB Clock drift detection (experimental)")
IF (DETECT_CLOCK_DRIFT)
    ADD_DEFINITIONS(-DMOOS_DETECT_CLOCK_DRIFT)
ENDIF (DETECT_CLOCK_DRIFT)

# Removed the extra field that was added to the MOOS message protocol in October 2009.
OPTION(DISABLE_AUX_SOURCE "Disable the AUX_SRC field in MOOS messages.  Only change this if you know what it is!" OFF)
IF (DISABLE_AUX_SOURCE)
  ADD_DEFINITIONS(-DDISABLE_AUX_SOURCE)
ENDIF (DISABLE_AUX_SOURCE)
MARK_AS_ADVANCED(DISABLE_AUX_SOURCE)



SET(MOOSLIB_SRCS
MOOSVariable.cpp
MOOSCommClient.cpp
MOOSCommObject.cpp
MOOSCommPkt.cpp
MOOSCommServer.cpp
MOOSException.cpp
MOOSGlobalHelper.cpp
MOOSMsg.cpp
MOOSApp.cpp
MOOSInstrument.cpp
MOOSSkewFilter.cpp
XPCGetHostInfo.cpp
XPCGetProtocol.cpp
XPCSocket.cpp
XPCTcpSocket.cpp
XPCUdpSocket.cpp
MOOSVariable.h
MOOSCommClient.h
MOOSCommObject.h
MOOSCommPkt.h
MOOSCommServer.h
MOOSException.h
MOOSGlobalHelper.h
MOOSLib.h
MOOSMsg.h
MOOSApp.h
MOOSInstrument.h
MOOSSkewFilter.h
XPCEndian.h
XPCException.h
XPCGetHostInfo.h
XPCGetProtocol.h
XPCSocket.h
XPCTcpSocket.h
XPCUdpSocket.h
 )

ADD_LIBRARY(MOOS ${MOOSLIB_SRCS})

SET( MOOSLIB_HEADERS
MOOSVariable.h
MOOSCommClient.h
MOOSCommObject.h
MOOSCommPkt.h
MOOSCommServer.h
MOOSException.h
MOOSGlobalHelper.h
MOOSLib.h
MOOSMsg.h
MOOSApp.h
MOOSInstrument.h
XPCEndian.h
XPCException.h
XPCGetHostInfo.h
XPCGetProtocol.h
XPCSocket.h
XPCTcpSocket.h
XPCUdpSocket.h
)

# install headers
INSTALL( FILES ${MOOSLIB_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MOOSLIB )

# install libraries
INSTALL(TARGETS
MOOS
RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

