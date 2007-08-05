# Makefile definitions particular to the logview app

LTARGET  = glogview
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libnavplot.a libipfview.a libmarineview.a libgeometry.a \
	   libmbutil.a libivpbuild.a libivpcore.a
LIBS     = $(patsubst lib%.a,-l%,$(PLIBS)) \
	   -lm -lfltk -lfltk_gl -lX11 -lGL -ltiff
PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
