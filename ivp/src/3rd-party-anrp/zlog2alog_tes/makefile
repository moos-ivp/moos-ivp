# Makefile definitions particular to the zlog2alog app

LTARGET  = zlog2alog_tes
LIFLAGS  = 
LCFLAGS  = 


PLIBS    = ../../3rd-party-anrp/lib_anrp_util/libanrp_util.a 

LIBS     = -lm -lpthread -lz \
	$(patsubst lib%.a,-l%,$(PLIBS)) \

PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../../make_config.mk
# Generic makefile structure for all application modules
include ../../make_3rd_party_app.mk
# DO NOT DELETE
