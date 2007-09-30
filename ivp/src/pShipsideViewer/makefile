# Makefile definitions particular to the pMarineViewer app

LTARGET  = pShipsideViewer
LIFLAGS  = 
LCFLAGS  = 

REQUIRED_MOOS_LIBS = 		\
	libMOOS.a 		\
	libMOOSGen.a 		\
	libMOOSUtility.a 		

PLIBS    = libmarineview.a libmbutil.a libgeometry.a libgenutil.a

LIBS     = -lm -lpthread -lfltk -lfltk_gl -lX11  -lGL -ltiff \
	$(patsubst lib%.a,-l%,$(PLIBS)) \
	$(patsubst lib%.a,-l%,$(REQUIRED_MOOS_LIBS)) 

PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
