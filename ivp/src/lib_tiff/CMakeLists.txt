#--------------------------------------------------------
# The CMakeLists.txt for:                       lib_tiff
# Author(s):                                Chris Gagner
#--------------------------------------------------------

SET(SRC
	tif_win32.c
        tif_aux.c
        tif_close.c
        tif_codec.c
        tif_color.c
        tif_compress.c
        tif_dir.c
        tif_dirinfo.c
        tif_dirread.c
        tif_dirwrite.c
        tif_dumpmode.c
        tif_error.c
        tif_extension.c
        tif_fax3.c
        tif_fax3sm.c
        tif_getimage.c
        tif_jpeg.c
        tif_ojpeg.c
        tif_flush.c
        tif_luv.c
        tif_lzw.c
        tif_next.c
        tif_open.c
        tif_packbits.c
        tif_pixarlog.c
        tif_predict.c
        tif_print.c
        tif_read.c
        tif_stream.cxx
        tif_swab.c
        tif_strip.c
        tif_thunder.c
        tif_tile.c
        tif_version.c
        tif_warning.c
        tif_write.c
        tif_zip.c	
)

ADD_LIBRARY(tiff STATIC tif_config.h tiffconf.h ${SRC})

ADD_LIBRARY(libtiff SHARED tif_config.h tiffconf.h libtiff.def ${SRC})
