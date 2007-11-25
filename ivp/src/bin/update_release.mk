.SILENT:

.PHONY : all

CORE_DIR = /home/mikerb/Research/moos-ivp/trunk/MOOS/Core/
ESSE_DIR = /home/mikerb/Research/moos-ivp/trunk/MOOS/Essentials/
MNAV_DIR = /home/mikerb/Research/moos-ivp/trunk/MOOS/NavigationAndControl/

IVP_DIR  = /home/mikerb/Research/moos-ivp/trunk/ivp/src/
REL_DIR  = /home/mikerb/Research/project-release/src/

IVP_MODS = lib_ivpcore 		\
	   lib_ivpbuild 	\
	   lib_mbutil 		\
           lib_geometry		\
	   lib_behaviors 	\
           lib_behaviors-marine \
	   lib_helmivp		\
	   lib_marineview 	\
	   lib_navplot 		\
	   lib_bhvutil 		\
	   lib_logic		\
	   pHelmIvP 		\
	   pMarineViewer	\
	   pTransponderAIS 	\
	   iMarineSim 		\
	   pMarinePID 		\
	   pEchoVar		\
	   uXMS			\
	   app_polyview 	\
	   app_logview 		

CORE_MODS = MOOSDB 		\
	    MOOSGenLib 		\
	    MOOSLIB 		

ESSE_MODS = pLogger 		\
	    pMOOSBridge 	\
	    MOOSUtilityLib	

MNAV_MODS = iRemote	


all: $(IVP_MODS) $(CORE_MODS) $(ESSE_MODS) $(MNAV_MODS) missions
	@echo Finished with update of project-release

$(IVP_MODS)::
	@echo Handling Directory $@
	rm -f $(REL_DIR)/$@/*.h
	rm -f $(REL_DIR)/$@/*.cpp
	cp $(IVP_DIR)/$@/*.h       $(REL_DIR)/$@/
	cp $(IVP_DIR)/$@/*.cpp     $(REL_DIR)/$@/

$(CORE_MODS)::
	@echo Handling Directory $@
	rm -f $(REL_DIR)/$@/*.h
	rm -f $(REL_DIR)/$@/*.cpp
	cp $(CORE_DIR)/$@/*.h       $(REL_DIR)/$@/
	cp $(CORE_DIR)/$@/*.cpp     $(REL_DIR)/$@/

$(ESSE_MODS)::
	@echo Handling Directory $@
	rm -f $(REL_DIR)/$@/*.h
	rm -f $(REL_DIR)/$@/*.cpp
	cp $(ESSE_DIR)/$@/*.h       $(REL_DIR)/$@/
	cp $(ESSE_DIR)/$@/*.cpp     $(REL_DIR)/$@/

$(MNAV_MODS)::
	@echo Handling Directory $@
	rm -f $(REL_DIR)/$@/*.h
	rm -f $(REL_DIR)/$@/*.cpp
	cp $(MNAV_DIR)/$@/*.h       $(REL_DIR)/$@/
	cp $(MNAV_DIR)/$@/*.cpp     $(REL_DIR)/$@/

missions:
	@echo Updating missions directory
	@echo Removing old version of missions
	rm -f $(REL_DIR)/../mission/*.moos
	rm -f $(REL_DIR)/../mission/*.bhv
	@echo Installing new version of missions
	cp $(IVP_DIR)/../missions/missions_manual/*.moos $(REL_DIR)../missions
	cp $(IVP_DIR)/../missions/missions_manual/*.bhv  $(REL_DIR)../missions

