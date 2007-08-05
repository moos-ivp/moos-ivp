.SILENT:

.PHONY : all

SRC_DIR = /home/mikerb/Research/project-marine/src/
REL_DIR = /home/mikerb/Research/project-release/src/

MODULES = lib_ivpcore lib_ivpbuild lib_mbutil lib_geometry	\
	  lib_behaviors lib_behaviors-marine lib_helmivp	\
	  lib_marineview lib_navplot lib_bhvutil lib_logic	\
	  pHelmIvP pMarineViewer	\
	  pTransponderAIS iMarineSim pMarinePID pEchoVar	\
	  app_polyview app_logview MOOSDB MOOSGenLib iRemote	\
	  pLogger pMOOSBridge uTMS


all: $(MODULES) missions
	@echo Finished with update of project-code-release

$(MODULES)::
	@echo Handling Directory $@
	rm -f $(REL_DIR)/$@/*.h
	rm -f $(REL_DIR)/$@/*.cpp
	rm -f $(REL_DIR)/$@/makefile
	cp $(SRC_DIR)/$@/*.h       $(REL_DIR)/$@/
	cp $(SRC_DIR)/$@/*.cpp     $(REL_DIR)/$@/
	cp $(SRC_DIR)/$@/makefile  $(REL_DIR)/$@/

missions:
	@echo Updating missions directory
	rm -f $(REL_DIR)/../mission/*.moos
	rm -f $(REL_DIR)/../mission/*.bhv
	cp $(SRC_DIR)/missions/example_missions/*.moos $(REL_DIR)../missions
	cp $(SRC_DIR)/missions/example_missions/*.bhv  $(REL_DIR)../missions

