.SILENT:

.PHONY : all

SRC_DIR = /home/mikerb/Research/moos-ivp/trunk/ivp/src/

IVP_CORE  = lib_ivpcore

IVP_HELM  = lib_helmivp lib_behaviors

GPL_RELS  = lib_ivpbuild 	lib_behaviors-marine 	\
	    lib_helmivp		lib_mbutil 		\
	    lib_geometry 	lib_marineview 		\
	    lib_navplot	 	lib_bhvutil		\
	    lib_logic					\
	    pHelmIvP 					\
	    pMarineViewer 	pTransponderAIS 	\
	    iMarineSim		pMarinePID 		\
	    pEchoVar 		app_polyview 		\
	    app_logview 	uTMS			\
	    uXMS     		uProcessWatch		\
	    uRepeater		uFunctionVis	

MRB_BETA  = lib_behaviors-colregs lib_behaviors-sandbox     	\
	    lib_behaviors-atttic  app_fview app_hexaview 	\
	    app_logclip app_logparse app_peval app_quadview 	\
	    app_solveIPF app_splug app_zaicview iHeadingMonitor \
	    iMetaCompass lib_xgui pLoiterControl		\
	    uFunctionViewer uFunctionSender 

license: $(IVP_CORE) $(IVP_HELM) $(GPL_RELS) $(MRB_BETA)

$(IVP_CORE)::
	license ivpcore nuwc nuwc $(SRC_DIR)/$@/*.h
	license ivpcore nuwc nuwc $(SRC_DIR)/$@/*.cpp

$(IVP_HELM)::
	license ivphelm nuwc nuwc $(SRC_DIR)/$@/*.h
	license ivphelm nuwc nuwc $(SRC_DIR)/$@/*.cpp

$(GPL_RELS)::
	license none gpl nuwcmit $(SRC_DIR)/$@/*.h
	license none gpl nuwcmit $(SRC_DIR)/$@/*.cpp

$(MRB_BETA)::
	license none limit nuwc $(SRC_DIR)/$@/*.h
	license none limit nuwc $(SRC_DIR)/$@/*.cpp


