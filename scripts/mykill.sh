#!/bin/bash
#--------------------------------------------------------------
#    Script: mykill.sh
#    Author: Michael Benjamin
#      Date: July 2025
#--------------------------------------------------------------
#  Part 1: Set global vars
#--------------------------------------------------------------
VERBOSE="yes"

#--------------------------------------------------------------
#  Part 2: Handle Command Line args
#--------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
        echo "info_date.sh [OPTIONS]                     "
        echo "  --help, -h       Show this help message  "
	echo "  --quiet, -q      Quiet output            "
        exit 0
    elif [ "${ARGI}" = "--quiet" -o "${ARGI}" = "-q" ]; then
        VERBOSE=""
    else
        echo "mykill.sh: Bad Arg: "$ARGI
        exit 1
    fi
done

# current apps and legacy apps
apps=()
lapps=()

# moos-ivp
apps+=("pAntler"      "MOOSDB"         "alogview"       )
apps+=("iSay"         "ffview"         "uFldMessageHandler")
apps+=("uXMS"         "pContactMgrV20" "pDeadManPost"   "uFldCollObDetect")
apps+=("pHelmIvP"     "pHostInfo"      "pLogger"        "pOdometry")
apps+=("pMapMarkers"  "pMarinePID"     "pMarinePIDV22"  "pMarineViewer")
apps+=("pMissionEval" "pMissionHash"   "pNodeReporter"  "pObstacleMgr")
apps+=("pRealm"       "pScheduler"     "pSearchGrid"    "uFldCollisionDetect")
apps+=("pSpoofNode"   "pXRelay"        "uCommand"       "uFldBeaconRangeSensor")
apps+=("pEchoVar"     "pShare"         "uFldDelve"      "uFldContactRangeSensor")
apps+=("uMAC"         "pAutoPoke"      "uFldNodeBroker" "uFldNodeComms")
apps+=("uMS"          "uFldPathCheck"  "uFldScope"      "uFldShoreBroker")
apps+=("uFunctionVis" "uHelmScope"     "uLoadWatch"     "uFldGenericSensor")
apps+=("uMACView"     "uMayFinish"     "uMemWatch"      "uFldObstacleSim")
apps+=("uPlayback"    "uPlotViewer"    "uProcessWatch"  "zaic_vect")
apps+=("uPokeDB"      "uQueryDB"       "uSimMarine"     "uSimMarineV22")
apps+=("zaic_vect"    "uTermCommand"   "uTimerScript"   "pBasicContactMgr")
apps+=("zaic_hdg"     "zaic_hleq"      "zaic_peak"      "zaic_spd")
lapps+=("pMOOSBridge" "iRemoteLite"    "iBlinkStick"    "uPoke")


# moos-ivp-ncurses
apps+=("uFldUnison" "uFldLoiterMgr" "uJobHandler" "uFldKaziZoneMgr")
lapps+=("dpath"      "uFldJoustMgr"  "uFldLoiterAssign")

# moos-ivp-swarm
apps+=("alogmtask"   "pMediator"    "rollsort")
apps+=("alognpos"    "uFldJoustMgr" "uFldVoronoi")
apps+=("pHostInfoV2" "pTaskManager" "uFldTaskMonitor")

# moos-ivp-newco
apps+=("bezview"   "gridview"      "pTowManager"   "uSimTowBody")
apps+=("bngview"   "obview"        "pairview"      "voitest")
apps+=("pEncircle" "platview"      "voitest_gp")
apps+=("freview"   "pProxonoi"     "uSimMarineTow" "voiview")

# moos-ivp-2680
apps+=("pPointAssignEval" "uFldHazardMetric" "uFldHazardSensor")
apps+=("swimview"         "uFldHazardMgr"    "uFldRescueMgr")

# moos-ivp-2680
apps+=("iM300" "iM300Health")

# Legacy
lapps+=("iVelodyne"  "pFeatureTracker" "pObstacleMgr" "uFldRescueMgr"    )
lapps+=("uSimHeron"  "uFldConvoyEval"  "uSockComms"   "pGeoSelect"       )
lapps+=("pPoseKeep"  "pProxonoi2"      "uFldAnemo"                       )
lapps+=("pGenRescue" "uFldFlagManager" "uFldTagManager"                  )
lapps+=("umm"        "pSearch"         "iPWMController" "uMissionTester")
lapps+=("iINS"       "pScheduler"      "pThermoTrack"   "pNaFCon "     )
lapps+=("pHuxley"    "pMessageSim"     "pBeamform"      "pBearingTrack ")
lapps+=("iDepth"     "uFldLegRunMgr"   "uFldLegRunEval" "pFrontEstimate")
lapps+=("iVSA"       "iCTDSBE49"       "pBearingsSim"   "uMVS_Bluefin"  )
lapps+=("iCTD"       "iPNICompass"     "iMetaCompass"   "iHeadingMonitor")
lapps+=("iWifi"      "p1BTracker"      "p2BTracker"     "pBearings"     )
lapps+=("pTarget"    "pRedirect"       "pLoiterControl" "pTransponderAIS")
lapps+=("uMVS"       "pTrafficControl" "uRepeater"      "pShipsideViewer")
lapps+=("pAEL"       "pViewerRelay"    "zLoggerNG"      "pClusterPriority")
lapps+=("uTMS"       "iMicroModem"     "pMessageSim"    "uDialogManager")
lapps+=("pCSVLogger" "pBearings_VSA"   "pFooBar"        "pTrackQuality" )
lapps+=("p1HTracker" "pGPSSim" "uBathy" "uCtdSim2"      "pTargetOpportunity")
lapps+=("pSubIndex"  "pBearings_DURIP" "pMBTracker"     "uSimCurrent"  )
lapps+=("pFramer"    "zLoggerNG"       "pTargetSim"     "pBearings_GENERIC")
lapps+=("iModemSim"  "pFunctionSender" "uPlayBack"      "uSimBeaconRange")
lapps+=("pRouter"    "uSimActiveSonar" "pConvexHull"    "uFldWrapDetect")
lapps+=("pSensorSim" "pArtifactMapper" "pNoiseSim"      "uNafconSplitter")
lapps+=("pBlast"     "pSweepLines"     "pNSWCPC"        "pXRelayTest")
lapps+=("pGateway"   "pAcommsHandler"  "pCTDCodec"      "pBTRCodec")
lapps+=("iDAS"       "pAcommsPoller"   "pArraySim"      "pSimpleAcousticSim")
lapps+=("iOEX"       "pMultiTargetSim" "uLogView"       "pMultiAcousticSim")
lapps+=("pCBF"       "pBTracker"       "pTrackMonitor"  "iKayakControlBoard")
lapps+=("pDTSP"      "iCommStack"      "pMissionCtl"    "pFrontGridRender")
lapps+=("iDSP"       "pBoundingCircle" "pRIDManager"    "uFldGenericSensor")
lapps+=("pFoobar"    "uLogViewHelm"    "uLogViewIPF"    "pGradeFrontEstimate")
lapps+=("uFldScope"  "pFleetControl"   "uFldPathCheck"  "pSearchGrid")
lapps+=("pGenPath"   "pGenPath2"       "pPointAssign"   "pHandleSensorData")
lapps+=("pNotDTSP"   "uFldCTDSensor"   "iOS5000"        "pRangeSensorHandler")
lapps+=("pWifiCheck" "pEvalLoiter"     "uSpeechRec"     "uFldMessageSourcer")
lapps+=("sedit"      "uSimHeron"       "camera_example" "camera_example_server")
lapps+=("uSockComms" "pMapMarkers"     "pGeneralCodec"  "camera_example_viewer")

all_apps=""
if [ "${VERBOSE}" = "" ]; then 
    for line in "${apps[@]}"
    do
	all_apps+=" $line"
    done
    for line in "${lapps[@]}"
    do
	all_apps+=" $line"
    done
    killall -9 -v $all_apps >& /dev/null
else
    echo "Checking for apps to kill..."
    for line in "${apps[@]}"
    do
	killall -9 $line >& /dev/null
	if [ $? = 0 ]; then
	    echo "killed: $line"
	fi	
    done
    for line in "${lapps[@]}"
    do
	all_apps+=" $line"
    done
    killall -9 -v $all_apps >& /dev/null
fi
exit 0

echo Done - Killed all MOOS-Related Processes.....
