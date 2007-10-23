# Makefile definitions particular to the pTarget app

LTARGET  = pTarget
LIFLAGS  = 
LCFLAGS  = 


REQUIRED_MOOS_LIBS = \
	libMOOS.a 			\
	libMOOSGen.a 		

PLIBS    = libgeometry.a libmbutil.a

LIBS     = -lm -lpthread \
	$(patsubst lib%.a,-l%,$(PLIBS)) \
	$(patsubst lib%.a,-l%,$(REQUIRED_MOOS_LIBS)) 

PLDIRS   = $(wildcard ../lib_*) $(wildcard ../../lib_*)

# Make configuration parameters
include ../../make_config.mk
# Generic makefile structure for all application modules
include ../../make_3rd_party_app.mk
# DO NOT DELETE
