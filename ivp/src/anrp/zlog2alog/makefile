# Makefile definitions particular to the zlog2alog app

LTARGET  = zlog2alog
LIFLAGS  = 
LCFLAGS  = 


PLIBS    = libanrp_util.a 

LIBS     = -lm -lpthread -lz \
	$(patsubst lib%.a,-l%,$(PLIBS)) \

PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
