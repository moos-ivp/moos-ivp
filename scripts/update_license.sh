#!/bin/bash

SRC="/Users/mikerb/Research/moos-ivp/branches/mikerb/moos-ivp-135lic/ivp/src/"

license  lgpl mike  -f $SRC/lib_ivpbuild/*.h     $SRC/lib_ivpbuild/*.cpp
license  lgpl mike  -f $SRC/lib_ivpcore/*.h      $SRC/lib_ivpcore/*.cpp
license  lgpl mike  -f $SRC/lib_behaviors/*.h    $SRC/lib_behaviors/*.cpp
license  lgpl mike  -f $SRC/lib_ivpsolve/*.h     $SRC/lib_ivpsolve/*.cpp
license  lgpl mike  -f $SRC/lib_mbutil/*.h       $SRC/lib_mbutil/*.cpp
license  lgpl mike  -f $SRC/lib_logic/*.h        $SRC/lib_logic/*.cpp
license  lgpl mike  -f $SRC/lib_geometry/*.h     $SRC/lib_geometry/*.cpp
license  lgpl mike  -f $SRC/lib_bhvutil/*.h      $SRC/lib_bhvutil/*.cpp
license  gpl  mike  -f $SRC/pHelmIvP/*.h         $SRC/pHelmIvP/*.cpp

license  gpl  mike  -f $SRC/app_alogcd/*.h       $SRC/app_alogcd/*.cpp
license  gpl  mike  -f $SRC/app_alogcheck/*.h    $SRC/app_alogcheck/*.cpp
license  gpl  mike  -f $SRC/app_alogclip/*.h     $SRC/app_alogclip/*.cpp

license  gpl  mike  -f $SRC/app_alogeplot/*.h    $SRC/app_alogeplot/*.cpp
license  gpl  mike  -f $SRC/app_aloggrep/*.h     $SRC/app_aloggrep/*.cpp
license  gpl  mike  -f $SRC/app_aloghelm/*.h     $SRC/app_aloghelm/*.cpp
license  gpl  mike  -f $SRC/app_alogiter/*.h     $SRC/app_alogiter/*.cpp
license  gpl  mike  -f $SRC/app_alogpare/*.h     $SRC/app_alogpare/*.cpp
license  gpl  mike  -f $SRC/app_alogrm/*.h       $SRC/app_alogrm/*.cpp
license  gpl  mike  -f $SRC/app_alogscan/*.h     $SRC/app_alogscan/*.cpp
license  gpl  mike  -f $SRC/app_alogsort/*.h     $SRC/app_alogsort/*.cpp
license  gpl  mike  -f $SRC/app_alogsplit/*.h    $SRC/app_alogsplit/*.cpp
license  gpl  mike  -f $SRC/app_alogview/*.h     $SRC/app_alogview/*.cpp

license  gpl  mike   -f $SRC/app_ffview/*.h      $SRC/app_ffview/*.cpp
license  gpl  mike   -f $SRC/app_gen_hazards/*.h $SRC/app_gen_hazards/*.cpp
license  gpl  mike   -f $SRC/app_geoview/*.h     $SRC/app_geoview/*.cpp
license  gpl  mike   -f $SRC/app_nsplug/*.h      $SRC/app_nsplug/*.cpp
license  gpl  mike   -f $SRC/app_zaic_hdg/*.h    $SRC/app_zaic_hdg/*.cpp
license  gpl  mike   -f $SRC/app_zaic_hleq/*.h   $SRC/app_zaic_hleq/*.cpp
license  gpl  mike   -f $SRC/app_zaic_peak/*.h   $SRC/app_zaic_peak/*.cpp
license  gpl  mike   -f $SRC/app_zaic_spd/*.h    $SRC/app_zaic_spd/*.cpp
license  gpl  mike   -f $SRC/app_zaic_vect/*.h   $SRC/app_zaic_hdg/*.cpp
license  gpl  mike   -f $SRC/iSay/*.h            $SRC/iSay/*.cpp


license  gpl  mike   -f $SRC/lib_apputil/*.h      $SRC/lib_apputil/*.cpp
license  gpl  mike   -f $SRC/lib_behaviors-colregs/*.h  $SRC/lib_behaviors-colregs/*.cpp
license  gpl  all    -f $SRC/lib_behaviors-marine/*.h  $SRC/lib_behaviors-marine/*.cpp
license  gpl  mike   -f $SRC/lib_contacts/*.h     $SRC/lib_contacts/*.cpp
license  gpl  mike   -f $SRC/lib_encounters/*.h   $SRC/lib_encounters/*.cpp
license  gpl  mike   -f $SRC/lib_genutil/*.h      $SRC/lib_genutil/*.cpp
license  gpl  mike   -f $SRC/lib_helmivp/*.h      $SRC/lib_helmivp/*.cpp

license  gpl  henrik -f $SRC/lib_henrik_anneal/*.h $SRC/lib_henrik_anneal/*.cpp
license  gpl  mike   -f $SRC/lib_ipfview/*.h      $SRC/lib_ipfview/*.cpp
license  gpl  mike   -f $SRC/lib_logutils/*.h     $SRC/lib_logutils/*.cpp
license  gpl  mike   -f $SRC/lib_marineview/*.h   $SRC/lib_marineview/*.cpp
license  gpl  mike   -f $SRC/lib_navplot/*.h      $SRC/lib_navplot/*.cpp
license  gpl  mike   -f $SRC/lib_ucommand/*.h     $SRC/lib_ucommand/*.cpp
license  gpl  mike   -f $SRC/lib_ufield/*.h       $SRC/lib_ufield/*.cpp
license  gpl  mike   -f $SRC/lib_ufld_hazards/*.h $SRC/lib_ufld_hazards/*.cpp
license  gpl  mike   -f $SRC/lib_zaicview/*.h     $SRC/lib_zaicview/*.cpp

license  gpl  mike   -f $SRC/pBasicContactMgr/*.h $SRC/pBasicContactMgr/*.cpp
license  gpl  mike   -f $SRC/pDeadManPost/*.h     $SRC/pDeadManPost/*.cpp
license  gpl  mike   -f $SRC/pEchoVar/*.h         $SRC/pEchoVar/*.cpp
license  gpl  henrik -f $SRC/pFrontEstimate/*.h   $SRC/pFrontEstimate/*.cpp
license  gpl  mike   -f $SRC/pHostInfo/*.h        $SRC/pHostInfo/*.cpp
license  gpl  mike   -f $SRC/pMarinePID/*.h       $SRC/pMarinePID/*.cpp
license  gpl  mike   -f $SRC/pMarineViewer/*.h    $SRC/pMarineViewer/*.cpp
license  gpl  mike   -f $SRC/pNodeReporter/*.h    $SRC/pNodeReporter/*.cpp
license  gpl  mike   -f $SRC/pSafetyRetreat/*.h   $SRC/pSafetyRetreat/*.cpp
license  gpl  mike   -f $SRC/pSearchGrid/*.h      $SRC/pSearchGrid/*.cpp
license  gpl  mike   -f $SRC/pXRelay/*.h          $SRC/pXRelay/*.cpp

license  gpl  mike   -f $SRC/uCommand/*.h          $SRC/uCommand/*.cpp
license  gpl  mike   -f $SRC/uFldBeaconRangeSensor/*.h   $SRC/uFldBeaconRangeSensor/*.cpp
license  gpl  mike   -f $SRC/uFldCollisionDetect/*.h     $SRC/uFldCollisionDetect/*.cpp
license  gpl  mike   -f $SRC/uFldContactRangeSensor/*.h  $SRC/uFldContactRangeSensor/*.cpp
license  gpl  mike   -f $SRC/uFldGenericSensor/*.h       $SRC/uFldGenericSensor/*.cpp
license  gpl  mike   -f $SRC/uFldHazardMetric/*.h        $SRC/uFldHazardMetric/*.cpp
license  gpl  mike   -f $SRC/uFldHazardMgr/*.h           $SRC/uFldHazardMgr/*.cpp
license  gpl  mike   -f $SRC/uFldHazardSensor/*.h        $SRC/uFldHazardSensor/*.cpp
license  gpl  mike   -f $SRC/uFldMessageHandler/*.h      $SRC/uFldMessageHandler/*.cpp
license  gpl  mike   -f $SRC/uFldNodeBroker/*.h          $SRC/uFldNodeBroker/*.cpp
license  gpl  mike   -f $SRC/uFldNodeComms/*.h           $SRC/uFldNodeComms/*.cpp
license  gpl  mike   -f $SRC/uFldPathCheck/*.h           $SRC/uFldPathCheck/*.cpp
license  gpl  mike   -f $SRC/uFldScope/*.h               $SRC/uFldScope/*.cpp
license  gpl  mike   -f $SRC/uFldShoreBroker/*.h         $SRC/uFldShoreBroker/*.cpp
license  gpl  kyle   -f $SRC/uFldWrapDetect/*.h          $SRC/uFldWrapDetect/*.cpp

license  gpl  mike   -f $SRC/uFunctionVis/*.h    $SRC/uFunctionVis/*.cpp
license  gpl  mike   -f $SRC/uHelmScope/*.h      $SRC/uHelmScope/*.cpp
license  gpl  mike   -f $SRC/uLoadWatch/*.h      $SRC/uLoadWatch/*.cpp
license  gpl  mike   -f $SRC/uMAC/*.h            $SRC/uMAC/*.cpp
license  gpl  mike   -f $SRC/uMACView/*.h        $SRC/uMACView/*.cpp
license  gpl  mike   -f $SRC/uPlotViewer/*.h     $SRC/uPlotViewer/*.cpp
license  gpl  mike   -f $SRC/uPokeDB/*.h         $SRC/uPokeDB/*.cpp
license  gpl  mike   -f $SRC/uProcessWatch/*.h   $SRC/uProcessWatch/*.cpp
license  gpl  mike   -f $SRC/uQueryDB/*.h        $SRC/uQueryDB/*.cpp
license  gpl  mike   -f $SRC/uSimCurrent/*.h     $SRC/uSimCurrent/*.cpp
license  gpl  mike   -f $SRC/uSimMarine/*.h      $SRC/uSimMarine/*.cpp
license  gpl  mike   -f $SRC/uTermCommand/*.h    $SRC/uTermCommand/*.cpp
license  gpl  mike   -f $SRC/uTimerScript/*.h    $SRC/uTimerScript/*.cpp
license  gpl  mike   -f $SRC/uXMS/*.h            $SRC/uXMS/*.cpp
 
