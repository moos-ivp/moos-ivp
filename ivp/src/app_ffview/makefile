# Makefile definitions particular to the fview app

LTARGET  = ffview
LIFLAGS  = 
LCFLAGS  = 

# Eliminated private libraries
# PLIBS    = libbehaviors-don.a		\
# 	   libbehaviors-sandbox.a 	\
# 	   libbehaviors-marine.a	\
# 	   libbhvutil.a			\
# 	   libipfview.a			\
# 	   libbehaviors.a		\
# 	   libgeometry.a 		\
# 	   libivpbuild.a    		\
#            libivpcore.a libmbutil.a 

PLIBS    = 	   libbhvutil.a			\
	   libipfview.a			\
	   libbehaviors.a		\
	   libgeometry.a 		\
	   libivpbuild.a    		\
           libivpcore.a libmbutil.a 

LIBS     = -lm -lfltk -lfltk_gl -lX11 -lGL 		\
           $(patsubst lib%.a,-l%,$(PLIBS))

PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE
