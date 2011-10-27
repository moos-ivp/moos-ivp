.SILENT:

.PHONY : all

MAR_DIR = /home/mikerb/Research/project-marine/src
PLS_DIR = /home/mikerb/Research/project-plus/src

MODULES  = iMarineSim iRemote lib_FLTKVW lib_behaviors 		\
	   lib_behaviors-colregs lib_behaviors-marine		\
	   lib_bhvutil lib_geometry lib_helmivp lib_ipfview	\
	   lib_ivpbuild lib_ivpcore lib_logic lib_marineview	\
	   lib_mbutil lib_navplot pEchoVar pHelmIvP pLogger	\
	   pMOOSBridge pMarinePID pMarineViewer pTransponderAIS \
	   uFunctionVis uProcessWatch uTMS uTermCommand uXMS

# iPNICompass
# iPWMController
# lib_anrp_util
# lib_behaviors-don

all: $(MODULES)

$(MODULES)::
	echo Cleaning $(PLS_DIR)/$@
	rm -f $(PLS_DIR)/$@/*.h
	rm -f $(PLS_DIR)/$@/*.cpp
	rm -f $(PLS_DIR)/$@/makefile*
	echo Populating $(PLS_DIR)/$@
	@cp $(MAR_DIR)/$@/*.h   $(PLS_DIR)/$@/
	@cp $(MAR_DIR)/$@/*.cpp $(PLS_DIR)/$@/
	@cp $(MAR_DIR)/$@/makefile*  $(PLS_DIR)/$@/

