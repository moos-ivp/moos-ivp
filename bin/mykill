#!/bin/bash

# We no longer use the "-q" option because it is not supported on
# the mac

moos_kill_list=$(ps -ef | egrep "MOOSDB|[pui][A-Z][a-z_A-Z]*" | grep '\.moos' | grep --invert-match xterm | cut -c 7-11)

if [[ ! -z $moos_kill_list ]]; then
    kill_list="$moos_kill_list"
fi

if [[ ! -z $kill_list ]]; then
    kill $@ ${kill_list} >& /dev/null
    echo "the moos is dead"
else
    echo "moos was not running, doing nothing."
fi

killall -9 iActuationRobotX iVelodyne pFeatureTracker       \
        pFeatureVision pGenWaypoint pManageAcousticVars     \
        pMarinePIDWAMV pObstacleMgr                         \
        uSimAcousticOutput uSimMarineWAMV pPoseKeep         \
        uTaskManager pBearingTracker pCBF pMergeArrayData   \
        iGPS_KFAC iActuationRobot pDeadManPost              \
        ddConsoleApp pTaskManager iM200 iActuationKFAC

killall -9 pLocalizePinger turnview

killall -9 MOOSDB iRemote MOOSRemote iGPS iGPSMonitor pNav  \
        pHelm pHelmIvP pMarineViewer iMarineSim uSimMarine  \
        pLogger iPWMController MOOSDump pAntler umm         \
        iINS iDepth pScheduler pThermoTrack pNaFCon pSearch \
        pHuxley pMessageSim pBeamform uTermCommand          \
        pBearingTrack uMissionTester pTestBug2

killall -9 pMOOSBridge  iVSA iCTD iCTDSBE49 pBearingsSim     \
        iPNICompass iMetaCompass iHeadingMonitor  pAEL       \
        iWifi p1BTracker p2BTracker pBearings uMS pTarget    \
        pRedirect pLoiterControl pTransponderAIS uMVS        \
        pMarinePID uTMS uMVS_Bluefin pEchoVar pDeployHandler \
        pTrafficControl pShipsideViewer uRepeater uXMS       \
        pViewerRelay uHelmScope pClusterPriority zLoggerNG

killall -9 iMicroModem pRouter pFramer pMessageSim             \
        pCSVLogger pBearings_VSA pEchoVar pTrackQuality        \
        p1HTracker pGPSSim uBathy uCtdSim2 pTargetOpportunity  \
        pBearings_DURIP uProcessWatch uPlayBack pSubIndex      \
        pBearings_GENERIC pTargetSim pMBTracker zLoggerNG      \
	iModemSim pFunctionSender uFunctionViewer uFunctionVis \
        uSimBeaconRange uSimCurrent pBasicContactMgr           \
        uSimActiveSonar bngview pConvexHull pObstacleMgr       \
        pObstacleManager

killall -9 pSensorSim pArtifactMapper pNoiseSim uNafconSplitter    \
        pBlast pSweepLines pNSWCPC pXRelay pXRelayTest alogview  \
        pAcommsHandler pCTDCodec pBTRCodec iDAS iOEX pGateway   \
        pAcommsPoller pArraySim pSimpleAcousticSim geoview      \
        pMultiTargetSim pMultiAcousticSim iOS5000 pGeneralCodec
        
        
killall -9 pCBF pBTracker pTrackMonitor uTimerScript pDTSP         \
        pTallyomatic pNodeReporter iCommStack pMissionCtl iDSP  \
        pNotDTSP pBoundingCircle pRIDManager uLogView           \
        uLogViewHelm uLogViewIPF pFooBar pFoobar                \
        pNotDTSP pBoundingCircle pRIDManager iKayakControlBoard

killall -9 pFleetControl uFldScope uFldContactRangeSensor         \
        uFldPathCheck uFldNodeComms pSearchGrid pHostInfo      \
        uFldShoreBroker uFldNodeBroker uFldBeaconRangeSensor   \
        uFldHazardSensor uFldMessageHandler pGenPath           \
        pPointAssign uFldHazardMetric pHandleSensorData        \
        uFldCTDSensor pFrontGridRender uPlotViewer uMAC        \
        pShare uFldGenericSensor uFldHazardMgr pGradeFrontEstimate pFrontEstimate

killall -9 pParticle uFldContactRangeSensorHP pUUVPath         \
        pWifiCheck uFldMessageSourcer pRangeSensorHandler      \
        pHunterPrey camera_example camera_example_server       \
        camera_example_viewer iHuxley uFldCollisionDetect iSay \
        uLoadWatch pPolePath sedit pEvalLoiter uMemWatch

killall -9 uSpeechRec uDialogManager

killall -9 pAntler pLogger 

echo Done - Killed all MOOS-Related Processes.....
