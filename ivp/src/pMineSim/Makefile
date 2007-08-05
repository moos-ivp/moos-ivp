# Makefile definitions particular to the pMineSim app

LTARGET  = pMineSim
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libMOOS.a libMOOSGen.a libgeometry.a libmbutil.a
LIBS     = -lm -lpthread $(patsubst lib%.a,-l%,$(PLIBS))
PLDIRS   = ../MOOSLIB ../MOOSGenLib ../lib_geometry ../lib_mbutil

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
