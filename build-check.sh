#!/bin/bash

TERSE="false"

OUT_MOOS_LIBS_ONLY="no"
OUT_MOOS_APPS_ONLY="no"
OUT_IVP_LIBS_ONLY="no"
OUT_IVP_APPS_ONLY="no"

MISSING_MOOS_LIBS=""
MISSING_MOOS_APPS=""
MISSING_IVP_LIBS=""
MISSING_IVP_APPS=""

MIN_ROBOT="no"


for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
        echo "build-check.sh [SWITCHES]                      "
        echo "  --help, -h                                   "
        echo "  --terse,-t   Output terse report             "
        echo "  --mooslib    Only show missing MOOS libs     "
        echo "  --moosapp    Only show missing MOOS apps     "
        echo "  --ivplib     Only show missing IvP libs      "
        echo "  --ivpapp     Only show missing IvP apps      "
        echo "  --minrobot   Only check for robot libs/apps  "
        echo "                                               "
        echo "Examples:                                      "
        echo "  build-check.sh                               "
        echo "  build-check.sh --terse                       "
        echo "  build-check.sh --mooslib                     "
        echo "  build-check.sh --moosapp                     "
        echo "  build-check.sh --ivplib                      "
        echo "  build-check.sh --ivpapp                      "
        echo "  build-check.sh --ivpapp --minrobot           "
        exit 0;
    elif [ "${ARGI}" = "--minrobot" ] ; then
        MIN_ROBOT="yes"
    elif [ "${ARGI}" = "--mooslib" ] ; then
        OUT_MOOS_LIBS_ONLY="yes"
    elif [ "${ARGI}" = "--moosapp" ] ; then
        OUT_MOOS_APPS_ONLY="yes"
    elif [ "${ARGI}" = "--ivplib" ] ; then
        OUT_IVP_LIBS_ONLY="yes"
    elif [ "${ARGI}" = "--ivpapp" ] ; then
        OUT_IVP_APPS_ONLY="yes"
    elif [ "${ARGI}" = "--terse" -o "${ARGI}" = "-t" ] ; then
        TERSE="true"
    fi
done

INVOC_ABS_DIR="$(pwd)"
SCRIPT_ABS_DIR="$(cd $(dirname "$0") && pwd -P)"

cd "${SCRIPT_ABS_DIR}"

#================================================================
# Part: Check existence of all MOOS Libraries
#================================================================
MISSING=""

if [ "`uname`" == "Darwin" ] ; then
    if [ ! -e build/MOOS/MOOSGeodesy/lib/libMOOSGeodesy.dylib ]; then
	MISSING=$MISSING"MOOSGeodesy,";
    fi
    if [ ! -e build/MOOS/MOOSToolsUI/lib/libfltkvw.dylib ]; then
	if [ "${MIN_ROBOT}" = "no" ] ; then
	    MISSING=$MISSING"fltkvw,";
	fi
    fi
else
    if [ ! -e build/MOOS/MOOSGeodesy/lib/libMOOSGeodesy.so ]; then
	MISSING=$MISSING"MOOSGeodesy,";
    fi
    if [ ! -e build/MOOS/MOOSToolsUI/lib/libfltkvw.so ]; then
	if [ "${MIN_ROBOT}" = "no" ] ; then
	    MISSING=$MISSING"fltkvw,";
	fi
    fi
fi

if [ ! -e build/MOOS/MOOSCore/lib/libMOOS.a ]; then
    MISSING=$MISSING"MOOS,";
fi
if [ ! -e build/MOOS/proj-5.2.0/lib/libproj.a ]; then
    MISSING=$MISSING"proj,";
fi

if [[ "$MISSING" == *, ]]; then
    MISSING_ALL=$MISSING_ALL$MISSING
    MISSING_MOOS_LIBS="${MISSING%?}";
fi

#=================================================================
# Part 2: Check health of MOOS Apps
#=================================================================
MISSING=""

if [ ! -e bin/umm ];         then  MISSING=$MISSING"umm,"; fi
if [ ! -e bin/ktm ];         then  MISSING=$MISSING"ktm,"; fi
if [ ! -e bin/mtm ];         then  MISSING=$MISSING"mtm,"; fi
if [ ! -e bin/mqos ];        then  MISSING=$MISSING"mqos,"; fi
if [ ! -e bin/pMOOSBridge ]; then  MISSING=$MISSING"pMOOSBridge,"; fi
if [ ! -e bin/pAntler ];     then  MISSING=$MISSING"pAntler,"; fi
if [ ! -e bin/pLogger ];     then  MISSING=$MISSING"pLogger,"; fi
if [ ! -e bin/pScheduler ];  then  MISSING=$MISSING"pScheduler,"; fi
if [ ! -e bin/pShare ];      then  MISSING=$MISSING"pShare,"; fi
if [ ! -e bin/iRemoteLite ]; then  MISSING=$MISSING"iRemoteLite,"; fi
if [ ! -e bin/uPoke ];       then  MISSING=$MISSING"uPoke,"; fi

if [ "${MIN_ROBOT}" = "no" ] ; then
    if [ ! -e bin/uMS ];         then  MISSING=$MISSING"uMS,"; fi
    if [ ! -e bin/uPlayback ];   then  MISSING=$MISSING"uPlayback,"; fi
fi

if [[ "$MISSING" == *, ]]; then
    MISSING_ALL=$MISSING_ALL$MISSING
    MISSING_MOOS_APPS="${MISSING%?}";
fi

#=================================================================
# Part 3: Check health of IvP Libraries
#=================================================================
MISSING=""

if [ ! -e lib/libapputil.a ];       then  MISSING=$MISSING"apputil,"; fi
if [ ! -e lib/libbehaviors.a ];     then  MISSING=$MISSING"behaviors,"; fi
if [ ! -e lib/libbehaviors-marine.a ];  then  MISSING=$MISSING"behaviors-marine,"; fi
if [ ! -e lib/libbehaviors-colregs.a ]; then  MISSING=$MISSING"behaviors-colregs,"; fi
if [ ! -e lib/libbhvutil.a ];       then  MISSING=$MISSING"bhvutil,"; fi
if [ ! -e lib/libcontacts.a ];      then  MISSING=$MISSING"contacts,"; fi
if [ ! -e lib/libencounters.a ];    then  MISSING=$MISSING"encounters,"; fi
if [ ! -e lib/libgenutil.a ];       then  MISSING=$MISSING"genutil,"; fi
if [ ! -e lib/libgeometry.a ];      then  MISSING=$MISSING"geometry,"; fi

if [ ! -e lib/libhelmivp.a ];       then  MISSING=$MISSING"helmivp,"; fi
if [ ! -e lib/libivpbuild.a ];      then  MISSING=$MISSING"ivpbuild,"; fi
if [ ! -e lib/libivpcore.a ];       then  MISSING=$MISSING"ivpcore,"; fi

if [ ! -e lib/libivpsolve.a ];      then  MISSING=$MISSING"ivpsolve,"; fi
if [ ! -e lib/liblogic.a ];         then  MISSING=$MISSING"logic,"; fi
if [ ! -e lib/liblogutils.a ];      then  MISSING=$MISSING"logutils,"; fi
if [ ! -e lib/libmanifest.a ];      then  MISSING=$MISSING"manifest,"; fi
if [ ! -e lib/libmbutil.a ];        then  MISSING=$MISSING"mbutil,"; fi

if [ ! -e lib/libufield.a ];       then  MISSING=$MISSING"ufield,"; fi
if [ ! -e lib/libufld_hazards.a ]; then  MISSING=$MISSING"ufld_hazards,"; fi

if [ "${MIN_ROBOT}" = "no" ] ; then
    if [ ! -e lib/libucommand.a ];   then  MISSING=$MISSING"ucommand,"; fi
    if [ ! -e lib/libipfview.a ];    then  MISSING=$MISSING"ipfview,"; fi
    if [ ! -e lib/libzaicview.a ];   then  MISSING=$MISSING"zaic_view,"; fi
    if [ ! -e lib/libmarineview.a ]; then  MISSING=$MISSING"marineview,"; fi
fi

# if missing is non-empty, augment missing_all, remove trailing common from
# missing_ivp_libs as this is the final value for that variable
if [[ "$MISSING" == *, ]]; then
    MISSING_ALL=$MISSING_ALL$MISSING
    MISSING_IVP_LIBS="${MISSING%?}";
fi

#=================================================================
# Part 4: Check health of IvP Apps
#=================================================================
MISSING=""

if [ ! -e bin/aloggrep ];    then  MISSING+="aloggrep,"; fi
if [ ! -e bin/alogscan ];    then  MISSING+="alogscan,"; fi
if [ ! -e bin/alogcd ];      then  MISSING+="alogcd,"; fi
if [ ! -e bin/alogpare ];    then  MISSING+="alogpare,"; fi
if [ ! -e bin/alogeplot ];   then  MISSING+="alogeplot,"; fi
if [ ! -e bin/alogrm ];      then  MISSING+="alogrm,"; fi
if [ ! -e bin/alogiter ];    then  MISSING+="alogiter,"; fi
if [ ! -e bin/alogcat ];     then  MISSING+="alogcat,"; fi
if [ ! -e bin/alogclip ];    then  MISSING+="alogclip,"; fi
if [ ! -e bin/aloghelm ];    then  MISSING+="aloghelm,"; fi
if [ ! -e bin/nsplug ];      then  MISSING+="nsplug,"; fi
if [ ! -e bin/pickpos ];     then  MISSING+="pickpos,"; fi
if [ ! -e bin/manifest_test ]; then  MISSING+="manifest_test,"; fi
if [ ! -e bin/manifest_wiki ]; then  MISSING+="manifest_wiki,"; fi
if [ ! -e bin/manifest_loc ];  then  MISSING+="manifest_loc,"; fi
if [ ! -e bin/pEchoVar ];      then  MISSING+="pEchoVar,"; fi
if [ ! -e bin/pEvalLoiter ];   then  MISSING+="pEvalLoiter,"; fi
if [ ! -e bin/pHelmIvP ];      then  MISSING+="pHelmIvP,"; fi
if [ ! -e bin/pMarinePID ];    then  MISSING+="pMarinePID,"; fi
if [ ! -e bin/pBasicContactMgr ]; then  MISSING+="pBasicContactMgr,"; fi
if [ ! -e bin/pDeadManPost ];  then  MISSING+="pDeadManPost,"; fi
if [ ! -e bin/pNodeReporter ]; then  MISSING+="pNodeReporter,"; fi
if [ ! -e bin/pObstacleMgr ];  then  MISSING+="pObstacleMgr,"; fi
if [ ! -e bin/pFrontEstimate ]; then  MISSING+="pFrontEstimate,"; fi
if [ ! -e bin/uFldMessageHandler ];  then  MISSING+="uFldMessageHandler,"; fi
if [ ! -e bin/uFldNodeBroker ];      then  MISSING+="uFldNodeBroker,"; fi
if [ ! -e bin/uHelmScope ];    then  MISSING+="uHelmScope,"; fi
if [ ! -e bin/uTimerScript ];  then  MISSING+="uTimerScript,"; fi
if [ ! -e bin/uProcessWatch ]; then  MISSING+="uProcessWatch,"; fi
if [ ! -e bin/uMemWatch ]; then  MISSING+="uMemWatch,"; fi
if [ ! -e bin/uLoadWatch ];    then  MISSING+="uLoadWatch,"; fi
if [ ! -e bin/uTermCommand ];  then  MISSING+="uTermCommand,"; fi
if [ ! -e bin/uXMS ];          then  MISSING+="uXMS,"; fi
if [ ! -e bin/uMAC ];          then  MISSING+="uMAC,"; fi
if [ ! -e bin/uPokeDB ];       then  MISSING+="uPokeDB,"; fi
if [ ! -e bin/uQueryDB ];      then  MISSING+="uQueryDB,"; fi
if [ ! -e bin/pHostInfo ];     then  MISSING+="pHostInfo,"; fi
if [ ! -e bin/iSay ];          then  MISSING+="iSay,"; fi


if [ "${MIN_ROBOT}" = "no" ] ; then
    if [ ! -e bin/alogsplit ];   then  MISSING+="alogsplit,"; fi
    if [ ! -e bin/alogsort ];    then  MISSING+="alogsort,"; fi
    if [ ! -e bin/alogcheck ];   then  MISSING+="alogcheck,"; fi
    if [ ! -e bin/gen_hazards ];   then  MISSING+="gen_hazards,"; fi
    if [ ! -e bin/gen_obstacles ]; then  MISSING+="gen_obstacles,"; fi
    if [ ! -e bin/bhv2graphviz ];  then  MISSING+="bhv2graphviz,"; fi
    if [ ! -e bin/pXRelay ];       then  MISSING+="pXRelay,"; fi
    if [ ! -e bin/uFldCollisionDetect ]; then  MISSING+="uFldCollisionDetect,"; fi
    if [ ! -e bin/uFldLoiterAssign ];    then  MISSING+="uFldLoiterAssign,"; fi
    if [ ! -e bin/uFldPathCheck ];       then  MISSING+="uFldPathCheck,"; fi
    if [ ! -e bin/uFldShoreBroker ];     then  MISSING+="uFldShoreBroker,"; fi
    if [ ! -e bin/uFldScope ];           then  MISSING+="uFldScope,"; fi
    if [ ! -e bin/uFldNodeComms ];       then  MISSING+="uFldNodeComms,"; fi
    if [ ! -e bin/uFldBeaconRangeSensor ]; then  MISSING+="uFldBeaconRangeSensor,"; fi
    if [ ! -e bin/uFldContactRangeSensor ]; then  MISSING+="uFldContactRangeSensor,"; fi
    if [ ! -e bin/uFldObstacleSim ];     then  MISSING+="uFldObstacleSim,"; fi
    if [ ! -e bin/uFldHazardSensor ];    then  MISSING+="uFldHazardSensor,"; fi
    if [ ! -e bin/uFldHazardMgr ];       then  MISSING+="uFldHazardMgr,"; fi
    if [ ! -e bin/uFldHazardMetric ];    then  MISSING+="uFldHazardMetric,"; fi
    if [ ! -e bin/uFldGenericSensor ];   then  MISSING+="uFldGenericSensor,"; fi
    #if [ ! -e bin/pWrapDetect ];   then  MISSING+="pWrapDetect,"; fi
    if [ ! -e bin/pSearchGrid ];   then  MISSING+="pSearchGrid,"; fi
    if [ ! -e bin/uSimMarine ];    then  MISSING+="uSimMarine,"; fi
    if [ ! -e bin/pFrontGridRender ];   then  MISSING+="pFrontGridRender,"; fi
    if [ ! -e bin/pGradeFrontEstimate ]; then  MISSING+="pGradeFrontEstimate,"; fi
    if [ ! -e bin/uFldCTDSensor ];       then  MISSING+="uFldCTDSensor,"; fi
    #if [ ! -e bin/uSimCurrent ];   then  MISSING+="uSimCurrent,"; fi
fi

if [ "${MIN_ROBOT}" = "no" ] ; then
    if [ ! -e bin/ffview ];      then  MISSING+="ffview,"; fi
    if [ ! -e bin/alogview ];    then  MISSING+="alogview,"; fi
    if [ ! -e bin/geoview ];     then  MISSING+="geoview,"; fi
    if [ ! -e bin/zaic_hleq ];   then  MISSING+="zaic_hleq,"; fi
    if [ ! -e bin/zaic_vect ];   then  MISSING+="zaic_vect,"; fi
    if [ ! -e bin/zaic_hdg ];    then  MISSING+="zaic_hdg,"; fi
    if [ ! -e bin/zaic_spd ];    then  MISSING+="zaic_spd,"; fi
    if [ ! -e bin/zaic_peak ];   then  MISSING+="zaic_peak,"; fi
    if [ ! -e bin/pMarineViewer ]; then  MISSING+="pMarineViewer,"; fi
    if [ ! -e bin/uFunctionVis ];  then  MISSING+="uFunctionVis,"; fi
    if [ ! -e bin/uMACView ];    then  MISSING+="uMACView,"; fi
    if [ ! -e bin/uPlotViewer ]; then  MISSING+="uPlotViewer,"; fi
    if [ ! -e bin/uCommand ];    then  MISSING+="uCommand,"; fi
fi

if [[ "$MISSING" == *, ]]; then
    MISSING_ALL=$MISSING_ALL$MISSING
    MISSING_IVP_APPS="${MISSING%?}";
fi


#===============================================================
# Part 5: Handle when specific output is requested.
#===============================================================

if [ "${OUT_MOOS_LIBS_ONLY}" == "yes" ]; then 
    if [ "$MISSING_MOOS_LIBS" == "" ]; then 
	echo "none"
	exit 0
    else
	echo $MISSING_MOOS_LIBS
	exit 1
    fi
fi

if [ "${OUT_MOOS_APPS_ONLY}" == "yes" ]; then 
    if [ "$MISSING_MOOS_APPS" == "" ]; then 
	echo "none"
	exit 0
    else
	echo $MISSING_MOOS_APPS
	exit 1
    fi
fi

if [ "${OUT_IVP_LIBS_ONLY}" == "yes" ]; then 
    if [ "$MISSING_IVP_LIBS" == "" ]; then 
	echo "none"
	exit 0
    else
	echo $MISSING_IVP_LIBS
	exit 1
    fi
fi

if [ "${OUT_IVP_APPS_ONLY}" == "yes" ]; then 
    if [ "$MISSING_IVP_APPS" == "" ]; then 
	echo "none"
	exit 0
    else
	echo $MISSING_IVP_APPS
	exit 1
    fi
fi


#===============================================================
# Part 6: Handle when collective output is requested.
#===============================================================
if [ "$TERSE" == "false" ]; then
    echo "     Missing MOOS Libs: " $MISSING_MOOS_LIBS
    echo "     Missing MOOS Apps: " $MISSING_MOOS_APPS
    echo "      Missing IvP Libs: " $MISSING_IVP_LIBS
    echo "      Missing IvP Apps: " $MISSING_IVP_APPS
fi

RESULT=0
if [ "$MISSING_ALL" == "" ]; then
    echo "PASS";
else
    if [[ "$MISSING_ALL" == *, ]]; then
	MISSING_ALL="${MISSING_ALL%?}";
    fi
    echo "FAIL{"$MISSING_ALL"}";
    RESULT=1
fi

cd "${INVOC_ABS_DIR}"

exit $RESULT

