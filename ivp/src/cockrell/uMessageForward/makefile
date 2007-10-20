# Makefile definitions 

LTARGET  = uMessageForward
LIFLAGS  = 
LCFLAGS  = 

REQUIRED_MOOS_LIBS = 		\
	libMOOS.a 		\
	libMOOSGen.a 		

PLIBS    = 

LIBS     = -lm -lpthread \
	$(patsubst lib%.a,-l%,$(PLIBS)) \
	$(patsubst lib%.a,-l%,$(REQUIRED_MOOS_LIBS)) 

PLDIRS   = $(wildcard ../lib_*)


# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
