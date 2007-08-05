PROJECT(MOOSGenLib)

SET(MOOSGENLIB_SRCS
   MOOSLock.cpp
   MOOSFileReader.cpp
   MOOSGenLibGlobalHelper.cpp
   ProcessConfigReader.cpp
   MOOSLinuxSerialPort.cpp
   MOOSSerialPort.cpp
   MOOSLock.h
   MOOSFileReader.h
   MOOSGenLibGlobalHelper.h
   ProcessConfigReader.h
   MOOSLinuxSerialPort.h
   MOOSSerialPort.h
   )


IF(WIN32)
    SET(MOOSGENLIB_SRCS ${MOOSGENLIB_SRCS} NTSerial.cpp MOOSNTSerialPort.cpp NTSerial.h MOOSNTSerialPort.h )
ENDIF(WIN32)

SET(ALLOW_RELEASE_ASSERTS FALSE CACHE BOOL "If TRUE then MOOSAssert is active in Release builds")

SET(MOOS_ASSERT_LEVEL "SOFT" CACHE STRING "OFF, SOFT (default) or HARD.  OFF: No asserts tested, SOFT: Prints error message but attempts to continue, HARD: Prints error message and exits.")
IF (MOOS_ASSERT_LEVEL MATCHES "OFF")
	ADD_DEFINITIONS(-DMOOS_ASSERT_LEVEL=0)
ELSEIF (MOOS_ASSERT_LEVEL MATCHES "SOFT")
	ADD_DEFINITIONS(-DMOOS_ASSERT_LEVEL=1)
ELSEIF (MOOS_ASSERT_LEVEL MATCHES "HARD")
	ADD_DEFINITIONS(-DMOOS_ASSERT_LEVEL=2)
ELSE (MOOS_ASSERT_LEVEL MATCHES "OFF")
	# Default
	ADD_DEFINITIONS(-DMOOS_ASSERT_LEVEL=1)
ENDIF (MOOS_ASSERT_LEVEL MATCHES "OFF")



ADD_LIBRARY(MOOSGen ${MOOSGENLIB_SRCS})

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

INSTALL(
   TARGETS MOOSGen
   DESTINATION lib/moos-${MOOS_VERSION}
   COMPONENT Moos_Core_Dev)

# Note: If there are any header files that are truly unneeded by users of this
# library, it should be OK to remove them from the PUBLIC_HEADERS variable,
# which in turn will prevent them from being installed. -CJC
IF(WIN32)
   SET(PUBLIC_HEADERS 
      MOOSAssert.h
      MOOSFileReader.h
      MOOSGenLibGlobalHelper.h
      MOOSGenLib.h
#       MOOSLinuxSerialPort.h   This isn't necessary for a Windows build, is it?
      MOOSLock.h
      MOOSNTSerialPort.h
      MOOSSerialPort.h
      NTSerial.h
      ProcessConfigReader.h
   )
ELSE(WIN32)
   SET(PUBLIC_HEADERS 
      MOOSAssert.h
      MOOSFileReader.h
      MOOSGenLibGlobalHelper.h
      MOOSGenLib.h
      MOOSLinuxSerialPort.h
      MOOSLock.h
      MOOSSerialPort.h
      ProcessConfigReader.h
   )
ENDIF(WIN32)

INSTALL(
   FILES ${PUBLIC_HEADERS}
   DESTINATION include/moos-${MOOS_VERSION}/Core/MOOSGenLib
   COMPONENT Moos_Core_Dev)

#===============================================================================
