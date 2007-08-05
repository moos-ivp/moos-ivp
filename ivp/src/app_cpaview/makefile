# Makefile definitions particular to the cpaview app

LTARGET  = cpaview
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libmarineview.a 	\
	   libipfview.a 	\
	   libgeometry.a  	\
	   libivpbuild.a 	\
	   libmbutil.a 		\
	   libivpcore.a
LIBS     = $(patsubst lib%.a,-l%,$(PLIBS)) \
	   -lm -lfltk -lfltk_gl -lX11 -lGL -ltiff
PLDIRS   = $(wildcard ../lib_*)


# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
