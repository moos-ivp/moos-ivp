# Makefile definitions particular to the pMarineViewer app

LTARGET  = wireframe_viewer
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = 

LIBS     = -lm -lpthread -lfltk -lfltk_gl  -lGL \
	$(patsubst lib%.a,-l%,$(PLIBS)) \

PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
