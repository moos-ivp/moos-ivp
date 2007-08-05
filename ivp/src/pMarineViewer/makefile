# Makefile definitions particular to the pMarineViewer app

LTARGET  = pMarineViewer
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libmarineview.a libmbutil.a libgeometry.a 	\
	   libMOOS.a libMOOSGen.a  
LIBS     = $(patsubst lib%.a,-l%,$(PLIBS)) \
	   -lm -lpthread -lfltk -lfltk_gl -lX11  -lGL -ltiff
PLDIRS   = $(wildcard ../lib_*) ../MOOSLIB ../MOOSGenLib

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
