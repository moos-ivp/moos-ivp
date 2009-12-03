#--------------------------------------------------------
# The CMakeLists.txt for:                       lib_tiff
# Author(s):                                Chris Gagner
#--------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(TIFF_DIR ./tiff-3.8.2)

SET(SRC_DIR ${TIFF_DIR}/libtiff)

INCLUDE_DIRECTORIES( AFTER ${SRC_DIR} )

# For Windows use the preconfigure headers provided by libtiff
CONFIGURE_FILE(${SRC_DIR}/tif_config.h.vc ${SRC_DIR}/tif_config.h COPYONLY)
CONFIGURE_FILE(${SRC_DIR}/tiffconf.h.vc ${SRC_DIR}/tiffconf.h COPYONLY)

SET(SYS_FILES ${SRC_DIR}/tif_win32.c)

SET(HDR
        ${SRC_DIR}/t4.h
        ${SRC_DIR}/tiff.h
        ${SRC_DIR}/tiffconf.h
        ${SRC_DIR}/tiffio.h
        ${SRC_DIR}/tiffiop.h
        ${SRC_DIR}/tiffvers.h
        ${SRC_DIR}/tif_config.h
        ${SRC_DIR}/tif_dir.h
        ${SRC_DIR}/tif_fax3.h
        ${SRC_DIR}/tif_predict.h
        ${SRC_DIR}/uvcode.h
	)

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

ADD_LIBRARY(tiff STATIC ${HDR} ${SRC})

