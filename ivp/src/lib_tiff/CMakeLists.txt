#--------------------------------------------------------
# The CMakeLists.txt for:                       lib_tiff
# Author(s):                                Chris Gagner
#--------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(TIFF_DIR ./tiff-3.8.2)

SET(SRC_DIR ${TIFF_DIR}/libtiff)

IF(${WIN32})
   # For Windows use the preconfigure headers provided by libtiff
      	CONFIGURE_FILE(${SRC_DIR}/tif_config.h.vc ${SRC_DIR}/tif_config.h COPYONLY)
   	CONFIGURE_FILE(${SRC_DIR}/tiffconf.h.vc ${SRC_DIR}/tiffconf.h COPYONLY)
	MESSAGE("You are running windows!!!")
ELSE(${WIN32})
   # For Apple and Unix/Linux use the autoconf provided by libtiff to 
   # generate the configuration headers.
	EXECUTE_PROCESS(
		WORKING_DIRECTORY ${TIFF_DIR}
		COMMAND ./configure
		)
ENDIF(${WIN32})


IF(${WIN32})
	SET(SYS_FILES ${SRC_DIR}/tif_win32.c)
ELSEIF(${UNIX})
	SET(SYS_FILES ${SRC_DIR}/tif_unix.c)
ELSEIF(${APPLE})
	SET(SYS_FILES ${SRC_DIR}/tif_apple.c)
ENDIF(${WIN32})

SET(SRC
	${SYS_FILES}
        ${SRC_DIR}/tif_aux.c
        ${SRC_DIR}/tif_close.c
        ${SRC_DIR}/tif_codec.c
        ${SRC_DIR}/tif_color.c
        ${SRC_DIR}/tif_compress.c
        ${SRC_DIR}/tif_dir.c
        ${SRC_DIR}/tif_dirinfo.c
        ${SRC_DIR}/tif_dirread.c
        ${SRC_DIR}/tif_dirwrite.c
        ${SRC_DIR}/tif_dumpmode.c
        ${SRC_DIR}/tif_error.c
        ${SRC_DIR}/tif_extension.c
        ${SRC_DIR}/tif_fax3.c
        ${SRC_DIR}/tif_fax3sm.c
        ${SRC_DIR}/tif_getimage.c
        ${SRC_DIR}/tif_jpeg.c
        ${SRC_DIR}/tif_ojpeg.c
        ${SRC_DIR}/tif_flush.c
        ${SRC_DIR}/tif_luv.c
        ${SRC_DIR}/tif_lzw.c
        ${SRC_DIR}/tif_next.c
        ${SRC_DIR}/tif_open.c
        ${SRC_DIR}/tif_packbits.c
        ${SRC_DIR}/tif_pixarlog.c
        ${SRC_DIR}/tif_predict.c
        ${SRC_DIR}/tif_print.c
        ${SRC_DIR}/tif_read.c
        ${SRC_DIR}/tif_stream.cxx
        ${SRC_DIR}/tif_swab.c
        ${SRC_DIR}/tif_strip.c
        ${SRC_DIR}/tif_thunder.c
        ${SRC_DIR}/tif_tile.c
        ${SRC_DIR}/tif_version.c
        ${SRC_DIR}/tif_warning.c
        ${SRC_DIR}/tif_write.c
        ${SRC_DIR}/tif_zip.c	
)

ADD_LIBRARY(tiff STATIC ${SRC_DIR}/tif_config.h ${SRC_DIR}/tiffconf.h ${SRC})

ADD_LIBRARY(libtiff SHARED 
	${SRC_DIR}/tif_config.h 
	${SRC_DIR}/tiffconf.h 
	${SRC_DIR}/libtiff.def 
	${SRC}
	)

IF(${UNIX})
        # Find JPEG Library and ZLIB Libraries and link them to the
        # TIFF libraries
        FIND_LIBRARY(JPEG jpeg)
        FIND_LIBRARY(ZLIB z zlib)
        SET(LIBS ${JPEG} ${ZLIB})
        TARGET_LINK_LIBRARIES(tiff ${LIBS})
        TARGET_LINK_LIBRARIES(libtiff ${LIBS})
ENDIF(${UNIX})

