.SILENT:

# Set variables to be passed down to subdirectories
include ./make_config.mk

LIB_BASE =  $(wildcard lib_ivpcore*)	$(wildcard lib_ivpbuild)   	   \
	    $(wildcard lib_mbutil*)	$(wildcard lib_anrp_util*)  	   \
	    $(wildcard lib_behaviors)	$(wildcard lib_behaviors-marine*)  \
	    $(wildcard lib_bhvutil*)	$(wildcard lib_behaviors-colregs*) \
	    $(wildcard MOOSUtilityLib*) $(wildcard lib_behaviors-don*) 	   \
	    $(wildcard lib_behaviors-sandbox*)				   \
	    $(wildcard lib_geometry*)	$(wildcard lib_helmivp*)    	   \
	    $(wildcard lib_logic*)	$(wildcard MOOSGenLib*)		   \
	    $(wildcard MOOSLIB*)	$(wildcard newmat10D*)

APP_BASE =  $(wildcard pHelmIvP*) 	$(wildcard pAntler*)	   	\
	    $(wildcard pEchoVar*) 	$(wildcard iRemote*)	   	\
	    $(wildcard pLogger*)	$(wildcard pLoiterControl*) 	\
	    $(wildcard pMOOSBridge*)	$(wildcard pMarinePID*)     	\
	    $(wildcard uXMS*)  	 	$(wildcard pTransponderAIS*)	\
	    $(wildcard uTMS*)		$(wildcard uTermCommand*)   	\
	    $(wildcard MOOSDB*)		$(wildcard iGPS*) 	   	\
	    $(wildcard iMarineSim*)	$(wildcard iMetaCompass*)   	\
	    $(wildcard iPNICompass*)	$(wildcard iPWMController*) 	\
	    $(wildcard MOOSDump*)	$(wildcard uProcessWatch*)	\
	    $(wildcard app_splug*)

LIB_TOOLS = $(wildcard lib_FLTKVW*)	$(wildcard lib_behaviors-sandbox*) \
	    $(wildcard lib_marineview*)	$(wildcard lib_behaviros-don*)	   \
	    $(wildcard lib_navplot*)	$(wildcard lib_ivpbuild-extra*)	   \
	    $(wildcard lib_track*)	$(wildcard lib_ipfview*)	

APP_TOOLS = $(wildcard p1BTracker*)	$(wildcard iHeadingMonitor*)	\
	    $(wildcard p2BTracker*)	$(wildcard pMarineViewer*)	\
	    $(wildcard pBearingsSim*)	$(wildcard pClusterManager*)	\
	    $(wildcard pACom*)		$(wildcard pExtTracker*)	\
	    $(wildcard iWifi*)	    	$(wildcard pFunctionSender*)	\
	    $(wildcard pMessageSim*)	$(wildcard pMineSim*)		\
	    $(wildcard pTarget*)	$(wildcard uFunctionVis*)	\
	    $(wildcard uMS*)		$(wildcard app_*)		\
	    $(wildcard uPlayBack*)	$(wildcard iMicroModem*)	\
	    $(wildcard uMVS*)		$(wildcard pDrone*)		\
	    $(wildcard pBearings*)	

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
	@cd $@; make depend
	@cd $@; make 

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

