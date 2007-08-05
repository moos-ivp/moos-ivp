# Makefile definitions particular to the pHelmIvP app

LTARGET  = pMarinePID
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libMOOS.a libMOOSGen.a libgeometry.a libmbutil.a
LIBS     = -lm -lpthread $(patsubst lib%.a,-l%,$(PLIBS))

PLDIRS   = $(wildcard ../lib_*) ../MOOSLIB ../MOOSGenLib

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
