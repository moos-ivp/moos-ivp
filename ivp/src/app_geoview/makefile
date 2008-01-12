# Makefile definitions particular to the polyview app

LTARGET  = geoview
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libmarineview.a libgeometry.a libmbutil.a 
LIBS     =  $(patsubst lib%.a,-l%,$(PLIBS)) \
	    -lm -lfltk -lfltk_gl -ltiff
PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
