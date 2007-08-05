# Makefile definitions particular to the atest app

LTARGET  = stest
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libivpbuild.a libmbutil.a libgeometry.a libivpcore.a liblogic.a
LIBS     = -lm $(patsubst lib%.a,-l%,$(PLIBS))

PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
