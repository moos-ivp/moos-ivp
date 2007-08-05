# Makefile definitions particular to the ltest app

LTARGET  = ltest
LIFLAGS  = 
LCFLAGS  = 

PLIBS    =  liblogic.a libmbutil.a
LIBS     = -lm $(patsubst lib%.a,-l%,$(PLIBS))

PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
