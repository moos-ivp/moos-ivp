# Makefile definitions particular to the logclip app

LTARGET  = logclip
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libmbutil.a 
LIBS     = -lm $(patsubst lib%.a,-l%,$(PLIBS))
PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
