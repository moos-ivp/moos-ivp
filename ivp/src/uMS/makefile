# Makefile definitions particular to the uMS app

LTARGET  = uMS
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libMOOS.a libMOOSGen.a libfltkvw.a
LIBS     = -lm -lpthread -lfltk -lX11 $(patsubst lib%.a,-l%,$(PLIBS))
PLDIRS   = ../MOOSLIB ../MOOSGenLib ../lib_FLTKVW $(LIB_FLTK)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
