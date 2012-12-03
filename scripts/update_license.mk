.SILENT:

.PHONY : all

#SRC_DIR = /home/mikerb/Research/moos-ivp/trunk/ivp/src/
SRC_DIR = /Users/mikerb/Research/moos-ivp/branches/mikerb/moos-ivp-122gou/ivp/src

IVP_CORE  = lib_ivpcore

IVP_HELM  = lib_helmivp lib_behaviors

GPL_RELS  = app_alogcheck       app_alogclip           \
            app_aloggrep        app_aloghelm           \
            app_alogrm          app_alogscan           \
            app_alogview        app_gen_hazards        \
            app_geoview         app_gzaicview          \
            app_ivpcheck        app_nsplug             \
            lib_apputil         lib_behaviors-marine   \
	    lib_bhvutil		lib_contacts           \
	    lib_geometry 	lib_helmivp            \
	    lib_ipview          lib_logic              \
	    lib_logutils        lib_marineview         \
            lib_mbutil          lib_navplot            \
            lib_ufield          lib_ufld_hazards       \
            lib_ulogview                               \
            pBasicContactMgr    pEchoVar               \
            pHelmIvP            pHostInfo              \
            pMarinePID          pMarineViewer          \
            pNodeReporter       pSafetyRetreat         \
            pSearchGrid         pXRelay                \
            uFldHazardMetric    uFldBeaconRangeSensor  \
            uFldHazardMgr       uFldContactRangeSensor \
            uFldGenericSensor   uFldHazardSensor       \
            uFldMessageHandler  uFldNodeBroker         \
            uFldNodeComms       uFldPathCheck          \
            uFldScope           uFldShoreBroker        \
            uFunctionVis        uHelmScope             \
            uLogView            uLogViewHelm           \
            uLogViewIPF         uMAC                   \
            uMACView            uPlotViewer            \
            uPokeDB             uProcessWatch          \
            uSimCurrent         uSimMarine             \
            uTermCommand        uTimerScript           \
            uXMS

MRB_BETA  = app_cpaview         app_ffview             \
            app_testfp          app_vzaicview

license: $(IVP_CORE) $(IVP_HELM) $(GPL_RELS) $(MRB_BETA)

$(IVP_CORE)::
	license ivpcore nuwc mike $(SRC_DIR)/$@/*.h
	license ivpcore nuwc mike $(SRC_DIR)/$@/*.cpp

$(IVP_HELM)::
	license ivphelm nuwc mike $(SRC_DIR)/$@/*.h
	license ivphelm nuwc mike $(SRC_DIR)/$@/*.cpp

$(GPL_RELS)::
	license none gpl all $(SRC_DIR)/$@/*.h
	license none gpl all $(SRC_DIR)/$@/*.cpp

$(MRB_BETA)::
	license none limit all $(SRC_DIR)/$@/*.h
	license none limit all $(SRC_DIR)/$@/*.cpp


