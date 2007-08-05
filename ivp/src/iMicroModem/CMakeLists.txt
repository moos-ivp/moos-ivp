PROJECT(iMicroModem)


#build a list of files to compile
SET(MICROMODEM_SRCS MicroModemInstrument.cpp iMicroModemMain.cpp MicroModemInstrument.h nmea.cpp nmea.h umodem.h umodem.cpp ccl.cpp ccl.h )

# Add executable
ADD_EXECUTABLE( iMicroModem ${MICROMODEM_SRCS})

# Link the executable
TARGET_LINK_LIBRARIES(iMicroModem ${MOOSLIBS} ${PLATFORM_LIBS} )
