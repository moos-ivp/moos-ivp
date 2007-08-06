# Makefile definitions particular to the uTMS app

LTARGET  = uFunctionVis
LIFLAGS  = 
LCFLAGS  = 

REQUIRED_MOOS_LIBS = \
	libMOOS.a 			\
	libMOOSGen.a 		

PLIBS    = libipfview.a libgeometry.a   \
           libmbutil.a libivpbuild.a libivpcore.a

LIBS     = -lm -lfltk -lfltk_gl -lX11 -lGL -lpthread \
	$(patsubst lib%.a,-l%,$(PLIBS)) \
	$(patsubst lib%.a,-l%,$(REQUIRED_MOOS_LIBS)) 

PLDIRS   = $(wildcard ../lib_*)


# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
