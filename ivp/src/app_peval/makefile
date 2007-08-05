# Makefile definitions particular to the peval app

LTARGET  = peval
LIFLAGS  = 
LCFLAGS  = 

PLIBS    =  libnavplot.a libmbutil.a libgeometry.a
LIBS     = -lm $(patsubst lib%.a,-l%,$(PLIBS))
PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
