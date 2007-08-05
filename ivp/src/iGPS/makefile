# Makefile definitions particular to the iGPS app

LTARGET  = iGPS
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libMOOS.a libMOOSGen.a libMOOSUtilityLib.a libanrp_util.a
LIBS     = -lm -lpthread $(patsubst lib%.a,-l%,$(PLIBS))
PLDIRS   = ../MOOSLIB ../MOOSGenLib ../MOOSUtilityLib ../lib_anrp_util

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
