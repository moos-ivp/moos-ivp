# Makefile definitions particular to the tupelize app

LTARGET  = tupelize
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libmbutil.a libivpbuild.a libivpcore.a
LIBS     = -lm $(patsubst lib%.a,-l%,$(PLIBS))
PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
