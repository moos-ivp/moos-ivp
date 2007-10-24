 .SILENT:

# Set variables to be passed down to subdirectories
include ./make_config.mk

LIB_BASE =  $(wildcard lib_ivpcore*)	$(wildcard lib_ivpbuild)   	   \
	    $(wildcard lib_mbutil*)	$(wildcard 3rd-party-anrp/lib_anrp_util*)  	   \
	    $(wildcard lib_behaviors)	$(wildcard lib_behaviors-marine*)  \
	    $(wildcard lib_bhvutil*)	$(wildcard lib_behaviors-colregs*) \
	    $(wildcard lib_geometry*)	$(wildcard lib_helmivp*)    	   \
	    $(wildcard newmat10D*)      $(wildcard lib_genutil)		   \
	    $(wildcard lib_logic*)	$(wildcard 3rd-party-eickstedt/lib_behaviors-don*) 	   \
	    $(wildcard 3rd-party-mikerb/lib_behaviors-sandbox*) \


# Removed because we no longer put MOOS code directly into the IvP source tree...
# $(wildcard MOOSUtilityLib*) 
# $(wildcard MOOSLIB*) $(wildcard MOOSGenLib*)		   \


APP_BASE =  $(wildcard pHelmIvP*) 	$(wildcard pEchoVar*) 	   	\
	    $(wildcard pLoiterControl*) $(wildcard pMarinePID*)     	\
	    $(wildcard uXMS*)  	 	$(wildcard pTransponderAIS*)	\
	    $(wildcard uTMS*)		$(wildcard uTermCommand*)   	\
	    $(wildcard iMarineSim*)	$(wildcard iMetaCompass*)   	\
	    $(wildcard anrp/Compass*)	$(wildcard iPWMController*) 	\
	    $(wildcard anrp/MOOSDump*)	$(wildcard uProcessWatch*)	\
	    $(wildcard app_splug*)	$(wildcard pDeployHandler*)	\
	    $(wildcard 3rd-party-grund/pFramer*)	$(wildcard 3rd-party-arjuna/pNaFCon*)		\
	    $(wildcard 3rd-party-grund/iMicroModem*)	$(wildcard 3rd-party-eickstedt/pScheduler*)		\
	    $(wildcard 3rd-party-grund/pRouter*)	$(wildcard uMessageForward*)	\
	    $(wildcard 3rd-party-anrp/iGPS*)           $(wildcard 3rd-party-cockrell/pUDPBroadcastClient*) \
	    $(wildcard pViewerRelay*)   $(wildcard 3rd-party-anrp/iWinch*) 		\
            $(wildcard pSamplingControl*) $(wildcard 3rd-party-anrp/iWinchSim*)	\
	    $(wildcard iMetaMemCompass*)  $(wildcard 3rd-party-anrp/iOS5000*)          \
	    $(wildcard 3rd-party-anrp/iCTDSBE49*)      $(wildcard 3rd-party-anrp/iSerialNMEA*)	\
	    $(wildcard iWebsite*)       $(wildcard 3rd-party-anrp/iWinchCTL*)          \
	    $(wildcard pVirtualTether*)	$(wildcard 3rd-party-anrp/MOOSBlink*)		\
	    $(wildcard uRepeater*)    	    $(wildcard 3rd-party-anrp/pCTDLogger*)     \
	    $(wildcard 3rd-party-anrp/iBatteryMonitor*)



# Removed because we no longer put MOOS code directly into the IvP source tree...
# $(wildcard pAntler*)  $(wildcard pLogger*)
# $(wildcard pMOOSBridge*)
# $(wildcard iRemote*)

LIB_TOOLS = $(wildcard 3rd-party-mikerb/lib_behaviors-sandbox*) \
	    $(wildcard lib_marineview*)	$(wildcard 3rd-party-eickstedt/lib_behaviros-don*)	   \
	    $(wildcard lib_navplot*)	$(wildcard 3rd-party-mikerb/lib_ivpbuild-extra*)	   \
	    $(wildcard 3rd-party-eickstedt/lib_track*)	$(wildcard lib_ipfview*)

# $(wildcard lib_FLTKVW*)

APP_TOOLS = $(wildcard 3rd-party-eickstedt/p1BTracker*)	$(wildcard iHeadingMonitor*)	\
	    $(wildcard 3rd-party-eickstedt/p2BTracker*)	$(wildcard pMarineViewer*)	\
	    $(wildcard 3rd-party-eickstedt/pBearingsSim*)	$(wildcard 3rd-party-mikerb/pClusterManager*)	\
	    $(wildcard pACom*)		$(wildcard 3rd-party-eickstedt/pExtTracker*)	\
	    $(wildcard iWifi*)	    	$(wildcard pFunctionSender*)	\
	    $(wildcard pMessageSim*)	$(wildcard 3rd-party-ajshafer/pMineSim*)		\
	    $(wildcard 3rd-party-eickstedt/pTarget*)	$(wildcard uFunctionVis*)	\
	    $(wildcard app_*)		$(wildcard pShipsideViewer*)	\
	    $(wildcard uPlayBack*)	$(wildcard iMicroModem*)	\
	    $(wildcard uMVS*)		$(wildcard pDrone*)		\
	    $(wildcard 3rd-party-eickstedt/pBearings*)	

# $(wildcard uMS*)	
# 

LIBS = $(LIB_BASE) $(LIB_TOOLS)

APPS = $(APP_BASE) $(APP_TOOLS)

# Provide the ability to BUILD the codebase
all:   $(LIBS) $(APPS)
	@echo "project-marine ueber make finished at "; date;
	@echo "CXX:" $(CXX) "  DEBUG_LEVEL:" $(DEBUG_LEVEL); echo

base:   $(LIB_BASE) $(APP_BASE)
	@echo "project-marine ueber make finished at "; date;
	@echo "CXX:" $(CXX) "  DEBUG_LEVEL:" $(DEBUG_LEVEL); echo

tools:  $(LIB_TOOLS) $(APP_TOOLS)
	@echo "project-marine ueber make finished at "; date;
	@echo "CXX:" $(CXX) "  DEBUG_LEVEL:" $(DEBUG_LEVEL); echo

$(LIBS):: 
	@echo Entering directory $@
	cd $@; make depend
	cd $@; make 

$(APPS):: 
	@echo Entering directory $@
	cd $@; make depend
	cd $@; make 

# Provide the ability to CLEAN the codebase
CLN_LIBS = $(patsubst %, cln_%, $(LIBS))
CLN_APPS = $(patsubst %, cln_%, $(APPS))

clean: $(CLN_LIBS) $(CLN_APPS)

$(CLN_LIBS):: 
	@echo Entering directory $@
	cd $(patsubst cln_%, %, $@); make clean LIB_DIR=$(LIB_DIR)

$(CLN_APPS):: 
	@echo Entering directory $@
	cd $(patsubst cln_%, %, $@); make clean BIN_DIR=$(BIN_DIR)

