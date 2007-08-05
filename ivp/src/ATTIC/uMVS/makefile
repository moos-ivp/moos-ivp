#------------------------------------------------------------
# Makefile for uMVS
# Makefile by Mike Benjamin

LTARGET  = uMVS
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libMOOS.a libMOOSGen.a libnewmat.a
LIBS     = -lm -lpthread -lfltk -lX11 $(patsubst lib%.a,-l%,$(PLIBS))
PLDIRS   = ../MOOSLIB ../MOOSGenLib ../newmat10a 

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
