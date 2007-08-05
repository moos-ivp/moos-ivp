# Makefile definitions particular to the mvsim app

LTARGET  = mvsim
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libhelmivp.a libmarinesim.a				\
	   libmarineview.a libbehaviors-search.a		\
	   libbehaviors-colregs.a libbehaviors-sandbox.a	\
	   libbehaviors-marine.a libbehaviors-don.a		\
	   libbehaviors.a libmbutil.a libivpbuild.a 		\
	   libivpcore.a libMOOSGen.a libnewmat.a		\
	   libbehaviors-nicop.a
LIBS     = $(patsubst lib%.a,-l%,$(PLIBS)) \
	   -lm -lpthread -lfltk -lfltk_gl -lX11 -lGL -ltiff
PLDIRS   = $(wildcard ../lib_*) ../MOOSLIB ../MOOSGenLib 	\
	   ../newmat10a $(LIB_FLTK)

all:
	@echo "skipping application " $(LTARGET)

depend:

clean:
	@echo Finished cleaning $(LTARGET)


# Make configuration parameters
#include ../make_config.mk
# Generic makefile structure for all application modules
#include ../make_app.mk
# DO NOT DELETE
