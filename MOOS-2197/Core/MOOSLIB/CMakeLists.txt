PROJECT(MOOSLib)

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
XPCTcpSocket.h )

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
XPCTcpSocket.h )

# install headers
INSTALL( FILES ${MOOSLIB_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MOOSLIB )

# install libraries
INSTALL(TARGETS
MOOS
RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

