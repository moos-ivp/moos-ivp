# Makefile definitions particular to the pHelmIvP app

LTARGET  = uFunctionViewer
LIFLAGS  = 
LCFLAGS  = 

REQUIRED_MOOS_LIBS = 			\
	libMOOS.a 			\
	libMOOSGen.a 		

# Removed linkage to libbehaviors-sandbox.a because it's a private library. -CJC


PLIBS    = libmbutil.a 			\
	   libgeometry.a 		\
	   libivpbuild.a 		\
	   libgenutil.a 		\
	   libivpcore.a 

LIBS     = -lm -lfltk -lfltk_gl -lX11 -lGL -lpthread \
	$(patsubst lib%.a,-l%,$(PLIBS)) 		\
	$(patsubst lib%.a,-l%,$(REQUIRED_MOOS_LIBS)) 

PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE

