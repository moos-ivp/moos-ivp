# Makefile definitions particular to the pHelmIvP app

LTARGET  = pHelmIvP
LIFLAGS  = 
LCFLAGS  = 

REQUIRED_MOOS_LIBS = \
	libMOOS.a 			\
	libMOOSGen.a 		
	   
PLIBS    = libhelmivp.a    		\
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


LIBS     = -lm -lpthread \
	$(patsubst lib%.a,-l%,$(PLIBS)) \
	$(patsubst lib%.a,-l%,$(REQUIRED_MOOS_LIBS)) 

PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
