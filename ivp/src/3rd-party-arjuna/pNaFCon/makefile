# Makefile definitions particular to the pNaFCon app

LTARGET  = pNaFCon
LIFLAGS  = 
LCFLAGS  = 

EXTRA_IFLAGS = -I ../../3rd-party-anrp/lib_anrp_util

REQUIRED_MOOS_LIBS = 			\
	libMOOS.a 			\
	libMOOSUtility.a		\
	libMOOSGen.a 		

PLIBS    = libmbutil.a libanrp_util.a

LIBS     = -lm -lpthread -ggdb3 \
	$(patsubst lib%.a,-l%,$(PLIBS)) \
	$(patsubst lib%.a,-l%,$(REQUIRED_MOOS_LIBS)) 

PLDIRS   = $(wildcard ../../lib_*) ../../3rd-party-anrp/lib_anrp_util

# Generic makefile structure for all application modules
include ../../make_3rd_party_app.mk
# DO NOT DELETE
