 .SILENT:

# Set variables to be passed down to subdirectories
include ./make_config.mk

LIB_BASE =  $(wildcard lib_ivpcore*)	$(wildcard lib_ivpbuild)   	   \
	    $(wildcard lib_mbutil*)	$(wildcard anrp/lib_anrp_util*)  	   \
	    $(wildcard lib_behaviors)	$(wildcard lib_behaviors-marine*)  \
	    $(wildcard lib_bhvutil*)	$(wildcard lib_behaviors-colregs*) \
	    $(wildcard lib_geometry*)	$(wildcard lib_helmivp*)    	   \
	    $(wildcard newmat10D*)      $(wildcard lib_genutil)		   \
	    $(wildcard lib_logic*)	$(wildcard eickstedt/lib_behaviors-don*) 	   \
	    $(wildcard mikerb/lib_behaviors-sandbox*) \


# Removed because we no longer put MOOS code directly into the IvP source tree...
# $(wildcard MOOSUtilityLib*) 
# $(wildcard MOOSLIB*) $(wildcard MOOSGenLib*)		   \


APP_BASE =  $(wildcard pHelmIvP*) 	$(wildcard pEchoVar*) 	   	\
	    $(wildcard pLoiterControl*) $(wildcard pMarinePID*)     	\
	    $(wildcard uXMS*)  	 	$(wildcard pTransponderAIS*)	\
	    $(wildcard uTMS*)		$(wildcard uTermCommand*)   	\
	    $(wildcard iMarineSim*)	$(wildcard iMetaCompass*)   	\
	    $(wildcard ajshafer/iPNICompass*)	$(wildcard iPWMController*) 	\
	    $(wildcard anrp/MOOSDump*)	$(wildcard uProcessWatch*)	\
	    $(wildcard app_splug*)	$(wildcard pDeployHandler*)	\
	    $(wildcard grund/pFramer*)	$(wildcard arjuna/pNaFCon*)		\
	    $(wildcard grund/iMicroModem*)	$(wildcard eickstedt/pScheduler*)		\
	    $(wildcard grund/pRouter*)	$(wildcard uMessageForward*)	\
	    $(wildcard iGPS*)           $(wildcard abahr/pUDPBroadcastClient*) \
	    $(wildcard pViewerRelay*)   $(wildcard anrp/iWinch*) 		\
            $(wildcard pSamplingControl*) $(wildcard anrp/iWinchSim*)	\
	    $(wildcard iMetaMemCompass*)  $(wildcard anrp/iOS5000*)          \
	    $(wildcard anrp/iCTDSBE49*)      $(wildcard anrp/iSerialNMEA*)	\
	    $(wildcard iWebsite*)       $(wildcard anrp/iWinchCTL*)          \
	    $(wildcard pVirtualTether*)	$(wildcard anrp/MOOSBlink*)		\
	    $(wildcard uRepeater*)    	    $(wildcard anrp/pCTDLogger*)     \
	    $(wildcard anrp/iBatteryMonitor*)



# Removed because we no longer put MOOS code directly into the IvP source tree...
# $(wildcard pAntler*)  $(wildcard pLogger*)
# $(wildcard pMOOSBridge*)
# $(wildcard iRemote*)

LIB_TOOLS = $(wildcard miker/lib_behaviors-sandbox*) \
	    $(wildcard lib_marineview*)	$(wildcard eickstedt/lib_behaviros-don*)	   \
	    $(wildcard lib_navplot*)	$(wildcard mikerb/lib_ivpbuild-extra*)	   \
	    $(wildcard eickstedt/lib_track*)	$(wildcard lib_ipfview*)

# $(wildcard lib_FLTKVW*)

APP_TOOLS = $(wildcard eickstedt/p1BTracker*)	$(wildcard iHeadingMonitor*)	\
	    $(wildcard eickstedt/p2BTracker*)	$(wildcard pMarineViewer*)	\
	    $(wildcard eickstedt/pBearingsSim*)	$(wildcard mikerb/pClusterManager*)	\
	    $(wildcard pACom*)		$(wildcard eickstedt/pExtTracker*)	\
	    $(wildcard iWifi*)	    	$(wildcard pFunctionSender*)	\
	    $(wildcard pMessageSim*)	$(wildcard ajshafer/pMineSim*)		\
	    $(wildcard eickstedt/pTarget*)	$(wildcard uFunctionVis*)	\
	    $(wildcard app_*)		$(wildcard pShipsideViewer*)	\
	    $(wildcard uPlayBack*)	$(wildcard iMicroModem*)	\
	    $(wildcard uMVS*)		$(wildcard pDrone*)		\
	    $(wildcard eickstedt/pBearings*)	

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

