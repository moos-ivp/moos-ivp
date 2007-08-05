# Makefile definitions particular to the uTMS app

LTARGET  = uFunctionViewer
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libMOOS.a libMOOSGen.a libgeometry.a libmbutil.a	\
	   libivpbuild.a libivpcore.a
LIBS     = -lm -lfltk -lfltk_gl -lX11 -lGL -lpthread 	\
	    $(patsubst lib%.a,-l%,$(PLIBS))
PLDIRS   = $(wildcard ../lib_*) ../MOOSLIB ../MOOSGenLib

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
