# Makefile for artfieldgenerator

LTARGET  = artfieldgenerator
LIFLAGS  =
LCFLAGS  =

# Libraries that are found in MOOS/MOOSBin...
REQUIRED_MOOS_LIBS = \
	libMOOS.a 			\
	libMOOSGen.a

# Libraries that are found somewhere in ivp/src/lib_*/ ...
PLIBS    = libipfview.a libgeometry.a   \
           libmbutil.a libivpbuild.a libivpcore.a libgenutil.a

# Libraries that are found elsewhere...
LIBS     = -lm -lfltk -lfltk_gl -lX11 -lGL -lpthread \
	$(patsubst lib%.a,-l%,$(PLIBS)) \
	$(patsubst lib%.a,-l%,$(REQUIRED_MOOS_LIBS)) 

PLDIRS   = $(wildcard ../lib_*)


# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
