# Makefile definitions particular to the quadview app

LTARGET  = gridgen
LIFLAGS  = 
LCFLAGS  = 

PLIBS    =  libivpbuild.a libivpcore.a libgeometry.a libmbutil.a 
LIBS     = -lm $(patsubst lib%.a,-l%,$(PLIBS))
PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
