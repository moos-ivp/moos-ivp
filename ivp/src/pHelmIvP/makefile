# Makefile definitions particular to the pHelmIvP app

LTARGET  = pHelmIvP
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libMOOS.a 			\
	   libMOOSGen.a 		\
	   libhelmivp.a    		\
	   libbehaviors-marine.a 	\
	   libbehaviors-sandbox.a 	\
	   libbhvutil.a			\
	   libbehaviors.a  		\
	   libmbutil.a 			\
	   libivpbuild.a 		\
	   libivpcore.a 		\
	   libgeometry.a 		\
	   liblogic.a

#	   libbehaviours-oxford.a 	\


LIBS     = -lm -lpthread $(patsubst lib%.a,-l%,$(PLIBS))

PLDIRS   = $(wildcard ../lib_*) ../MOOSLIB ../MOOSGenLib

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
