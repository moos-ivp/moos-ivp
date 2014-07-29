#!/bin/bash

SRC="/Users/mikerb/Research/moos-ivp/branches/mikerb/moos-ivp-135lic/ivp/src/"

license  lgpl all   -f $SRC/lib_ivpbuild/*.h     $SRC/lib_ivpbuild/*.cpp
license  lgpl mike  -f $SRC/lib_ivpcore/*.h      $SRC/lib_ivpcore/*.cpp
license  lgpl all   -f $SRC/lib_behaviors/*.h    $SRC/lib_behaviors/*.cpp
license  nuwc all   -f $SRC/lib_ivpsolve/*.h     $SRC/lib_ivpsolve/*.cpp
license  nuwc all   -f $SRC/pHelmIvP/*.h         $SRC/pHelmIvP/*.cpp

license  gpl  all   -f $SRC/lib_helmivp/*.h      $SRC/lib_helmivp/*.cpp
license  gpl  all   -f $SRC/app_alogcheck/*.h    $SRC/app_alogcheck/*.cpp
license  gpl  all   -f $SRC/app_alogclip/*.h     $SRC/app_alogclip/*.cpp
license  gpl  kyle  -f $SRC/app_alogeff/*.h      $SRC/app_alogeff/*.cpp
license  gpl  all   -f $SRC/app_aloggrep/*.h     $SRC/app_aloggrep/*.cpp
license  gpl  all   -f $SRC/app_aloghelm/*.h     $SRC/app_aloghelm/*.cpp
license  gpl  all   -f $SRC/app_alogiter/*.h     $SRC/app_alogiter/*.cpp
license  gpl  all   -f $SRC/app_alogrm/*.h       $SRC/app_alogrm/*.cpp
license  gpl  all   -f $SRC/app_alogscan/*.h     $SRC/app_alogscan/*.cpp
license  gpl  all   -f $SRC/app_alogsort/*.h     $SRC/app_alogsort/*.cpp
license  gpl  all   -f $SRC/app_alogview/*.h     $SRC/app_alogview/*.cpp
license  gpl  all   -f $SRC/app_currview/*.h     $SRC/app_currview/*.cpp
license  gpl  all   -f $SRC/app_ffview/*.h       $SRC/app_ffview/*.cpp
license  gpl  all   -f $SRC/app_gen_hazards/*.h  $SRC/app_gen_hazards/*.cpp
license  gpl  all   -f $SRC/app_geoview/*.h      $SRC/app_geoview/*.cpp
license  gpl  all   -f $SRC/app_gzaicview/*.h    $SRC/app_gzaicview/*.cpp
license  gpl  all   -f $SRC/app_ivpcheck/*.h     $SRC/app_ivpcheck/*.cpp
license  gpl  all   -f $SRC/app_nsplug/*.h       $SRC/app_nsplug/*.cpp
license  gpl  all   -f $SRC/app_vzaicview/*.h    $SRC/app_vzaicview/*.cpp
license  gpl  all   -f $SRC/lib_apputil/*.h      $SRC/lib_apputil/*.cpp
license  gpl  all   -f $SRC/lib_behaviors-marine/*.h  $SRC/lib_behaviors-marine/*.cpp
license  gpl  all   -f $SRC/lib_bhvutil/*.h      $SRC/lib_bhvutil/*.cpp
license  gpl  all   -f $SRC/lib_contacts/*.h     $SRC/lib_contacts/*.cpp
license  gpl  all   -f $SRC/lib_genutil/*.h      $SRC/lib_genutil/*.cpp
license  gpl  all   -f $SRC/lib_geometry/*.h     $SRC/lib_geometry/*.cpp
license  gpl  all   -f $SRC/lib_ipfview/*.h      $SRC/lib_ipfview/*.cpp
license  gpl  all   -f $SRC/lib_logic/*.h        $SRC/lib_logic/*.cpp
license  gpl  all   -f $SRC/lib_logutils/*.h     $SRC/lib_logutils/*.cpp
license  gpl  all   -f $SRC/lib_marineview/*.h   $SRC/lib_marineview/*.cpp
license  gpl  all   -f $SRC/lib_mbutil/*.h       $SRC/lib_mbutil/*.cpp
license  gpl  all   -f $SRC/lib_navplot/*.h      $SRC/lib_navplot/*.cpp
license  gpl  all   -f $SRC/lib_ufield/*.h       $SRC/lib_ufield/*.cpp
license  gpl  all   -f $SRC/lib_ufld_hazards/*.h $SRC/lib_ufld_hazards/*.cpp
license  gpl  all   -f $SRC/lib_ulogview/*.h     $SRC/lib_ulogview/*.cpp
license  gpl  all   -f $SRC/pBasicContactMgr/*.h $SRC/pBasicContactMgr/*.cpp
license  gpl  all   -f $SRC/pEchoVar/*.h         $SRC/pEchoVar/*.cpp
license  gpl  all   -f $SRC/pHostInfo/*.h        $SRC/pHostInfo/*.cpp
license  gpl  all   -f $SRC/pMarinePID/*.h       $SRC/pMarinePID/*.cpp
license  gpl  all   -f $SRC/pMarineViewer/*.h    $SRC/pMarineViewer/*.cpp
license  gpl  all   -f $SRC/pNodeReporter/*.h    $SRC/pNodeReporter/*.cpp
license  gpl  all   -f $SRC/pSafetyRetreat/*.h   $SRC/pSafetyRetreat/*.cpp
license  gpl  all   -f $SRC/pSearchGrid/*.h      $SRC/pSearchGrid/*.cpp
license  gpl  all   -f $SRC/pXRelay/*.h          $SRC/pXRelay/*.cpp
license  gpl  all   -f $SRC/iSay/*.h             $SRC/iSay/*.cpp

license  gpl  all   -f $SRC/uFldBeaconRangeSensor/*.h   $SRC/uFldBeaconRangeSensor/*.cpp
license  gpl  kyle  -f $SRC/uFldCollisionDetect/*.h     $SRC/uFldCollisionDetect/*.cpp
license  gpl  all   -f $SRC/uFldContactRangeSensor/*.h  $SRC/uFldContactRangeSensor/*.cpp
license  gpl  all   -f $SRC/uFldGenericSensor/*.h       $SRC/uFldGenericSensor/*.cpp
license  gpl  all   -f $SRC/uFldHazardMetric/*.h        $SRC/uFldHazardMetric/*.cpp
license  gpl  all   -f $SRC/uFldHazardMgr/*.h           $SRC/uFldHazardMgr/*.cpp
license  gpl  all   -f $SRC/uFldHazardSensor/*.h        $SRC/uFldHazardSensor/*.cpp
license  gpl  all   -f $SRC/uFldMessageHandler/*.h      $SRC/uFldMessageHandler/*.cpp
license  gpl  all   -f $SRC/uFldNodeBroker/*.h          $SRC/uFldNodeBroker/*.cpp
license  gpl  all   -f $SRC/uFldNodeComms/*.h           $SRC/uFldNodeComms/*.cpp
license  gpl  all   -f $SRC/uFldPathCheck/*.h           $SRC/uFldPathCheck/*.cpp
license  gpl  all   -f $SRC/uFldScope/*.h               $SRC/uFldScope/*.cpp
license  gpl  all   -f $SRC/uFldShoreBroker/*.h         $SRC/uFldShoreBroker/*.cpp
license  gpl  kyle  -f $SRC/uFldWrapDetect/*.h          $SRC/uFldWrapDetect/*.cpp

license  gpl  all   -f $SRC/uFunctionVis/*.h    $SRC/uFunctionVis/*.cpp
license  gpl  all   -f $SRC/uHelmScope/*.h      $SRC/uHelmScope/*.cpp
license  gpl  all   -f $SRC/uLoadWatch/*.h      $SRC/uLoadWatch/*.cpp
license  gpl  all   -f $SRC/uLogView/*.h        $SRC/uLogView/*.cpp
license  gpl  all   -f $SRC/uLogViewHelm/*.h    $SRC/uLogViewHelm/*.cpp
license  gpl  all   -f $SRC/uLogViewIPF/*.h     $SRC/uLogViewIPF/*.cpp
license  gpl  all   -f $SRC/uMAC/*.h            $SRC/uMAC/*.cpp
license  gpl  all   -f $SRC/uMACView/*.h        $SRC/uMACView/*.cpp
license  gpl  all   -f $SRC/uPlotViewer/*.h     $SRC/uPlotViewer/*.cpp
license  gpl  all   -f $SRC/uPokeDB/*.h         $SRC/uPokeDB/*.cpp
license  gpl  all   -f $SRC/uProcessWatch/*.h   $SRC/uProcessWatch/*.cpp
license  gpl  all   -f $SRC/uSimCurrent/*.h     $SRC/uSimCurrent/*.cpp
license  gpl  all   -f $SRC/uSimMarine/*.h      $SRC/uSimMarine/*.cpp
license  gpl  all   -f $SRC/uTermCommand/*.h    $SRC/uTermCommand/*.cpp
license  gpl  all   -f $SRC/uTimerScript/*.h    $SRC/uTimerScript/*.cpp
license  gpl  all   -f $SRC/uXMS/*.h            $SRC/uXMS/*.cpp
 
