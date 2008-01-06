# Makefile definitions particular to the pAIS2SQL app

LTARGET  = ptsur2sql
LIFLAGS  = 
LCFLAGS  = 

REQUIRED_MOOS_LIBS = 			

PLIBS   =

LIBS     = -lpthread  -lmysqlclient \
	$(patsubst lib%.a,-l%,$(PLIBS)) \
	$(patsubst lib%.a,-l%,$(REQUIRED_MOOS_LIBS)) \

PLDIRS   = $(wildcard ../lib_*)  $(wildcard ../../lib_*)

EXTRA_IFLAGS = -I /usr/include/mysql

EXTRA_LFLAGS = -L /usr/lib/mysql


# Make configuration parameters
include ../../make_config.mk
# Generic makefile structure for all application modules
include ../../make_3rd_party_app.mk
# DO NOT DELETE