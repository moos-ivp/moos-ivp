# Makefile definitions particular to the iMicroModem app

LTARGET  = iMicroModem
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libMOOS.a libMOOSGen.a libanrp_util.a
LIBS     = -lm -lpthread $(patsubst lib%.a,-l%,$(PLIBS))
PLDIRS   = ../MOOSLIB ../MOOSGenLib ../lib_anrp_util

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
