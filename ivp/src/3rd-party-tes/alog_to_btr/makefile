# Makefile definitions particular to this app

LTARGET  = alog_to_btr
LIFLAGS  = 
LCFLAGS  = 

REQUIRED_MOOS_LIBS = 


PLIBS   = 

LIBS     =  \
	$(patsubst lib%.a,-l%,$(PLIBS)) \
	$(patsubst lib%.a,-l%,$(REQUIRED_MOOS_LIBS)) \

PLDIRS   = $(wildcard ../lib_*)  $(wildcard ../../lib_*)


# Make configuration parameters
include ../../make_config.mk
# Generic makefile structure for all application modules
include ../../make_3rd_party_app.mk
# DO NOT DELETE