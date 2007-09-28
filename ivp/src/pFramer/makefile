# Makefile definitions particular to the pFramer app

LTARGET  = pFramer
LIFLAGS  = 
LCFLAGS  = 

REQUIRED_MOOS_LIBS =			\
	libMOOS.a 			\
	libMOOSGen.a 		

LIBS     = -lm -lpthread \
	$(patsubst lib%.a,-l%,$(PLIBS)) \
	$(patsubst lib%.a,-l%,$(REQUIRED_MOOS_LIBS)) 


# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
