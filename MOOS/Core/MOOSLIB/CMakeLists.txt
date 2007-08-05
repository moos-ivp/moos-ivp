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
XPCEndian.h
XPCException.h
XPCGetHostInfo.h
XPCGetProtocol.h
XPCSocket.h
XPCTcpSocket.h )

ADD_LIBRARY(MOOS ${MOOSLIB_SRCS})

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

INSTALL(
   TARGETS MOOS
   DESTINATION lib/moos-${MOOS_VERSION}
   COMPONENT Moos_Core_Dev)

# Note: If there are any header files that are truly unneeded by users of this
# library, it should be OK to remove them from the PUBLIC_HEADERS variable,
# which in turn will prevent them from being installed. -CJC
SET(PUBLIC_HEADERS 
   MOOSApp.h
   MOOSCommClient.h
   MOOSCommObject.h
   MOOSCommPkt.h
   MOOSCommServer.h
   MOOSException.h
   MOOSGlobalHelper.h
   MOOSInstrument.h
   MOOSLib.h
   MOOSMsg.h
   MOOSVariable.h
   XPCEndian.h
   XPCException.h
   XPCGetHostInfo.h
   XPCGetProtocol.h
   XPCSocket.h
   XPCTcpSocket.h
)

INSTALL(
   FILES ${PUBLIC_HEADERS}
   DESTINATION include/moos-${MOOS_VERSION}/Core/MOOSLib
   COMPONENT Moos_Core_Dev)

#===============================================================================
