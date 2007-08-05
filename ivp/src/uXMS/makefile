# Makefile definitions particular to the uTMS app

LTARGET  = uXMS
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libMOOS.a libMOOSGen.a libmbutil.a
LIBS     = -lm -lpthread $(patsubst lib%.a,-l%,$(PLIBS))
PLDIRS   = $(wildcard ../lib_*) ../MOOSLIB ../MOOSGenLib

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
