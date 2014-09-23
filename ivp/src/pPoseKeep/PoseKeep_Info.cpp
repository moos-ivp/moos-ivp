/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: PoseKeep_Info.cpp                                    */
/*   DATE: Sep 15th 2014                                        */
/****************************************************************/

#include <cstdlib>
#include <iostream>
#include "PoseKeep_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The pPoseKeep application is used for                         ");
  blk("                                                                ");
  blk("                                                                ");
  blk("                                                                ");
  blk("                                                                ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: pPoseKeep file.moos [OPTIONS]                            ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pPoseKeep with the given process name              ");
  blk("      rather than pPoseKeep.                                    ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pPoseKeep.                 ");
  blk("                                                                ");
  blk("Note: If argv[2] does not otherwise match a known option,       ");
  blk("      then it will be interpreted as a run alias. This is       ");
  blk("      to support pAntler launching conventions.                 ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("pPoseKeep Example MOOS Configuration                            ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pPoseKeep                                       ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  activeflag   = THRUST_MODE_DIFFERENTIAL=true                  ");
  blk("  inactiveflag = THRUST_MODE_DIFFERENTIAL=false                 ");
  blk("  endflag      = POSE_KEEP = done                               ");
  blk("                                                                ");
  blk("	 hold_duration  = 60  // seconds, default is -1, no limit      ");
  blk("  hold_tolerance = 2   // degrees, default is 3                 ");
  blk("  hold_heading   = 270  // degrees, no default                  ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("pPoseKeep INTERFACE                                             ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  APPCAST_REQ                                                   ");
  blk("  DEPLOY                                                        ");
  blk("  NAV_X                                                         ");
  blk("  NAV_Y                                                         ");
  blk("  NAV_HEADING                                                   ");
  blk("                                                                ");
  blk("  HOLD_POINT                                                    ");
  blk("  MVIEWER_LCLICK                                                ");
  blk("  HOLD_TOLERANCE                                                ");
  blk("  HOLD_DURATION                                                 ");
  blk("  HOLD_HEADING                                                  ");
  blk("                                                                ");
  blk("  HOLD_ENDFLAG                                                  ");
  blk("  HOLD_ENDFLAG_RESET                                            ");
  blk("  HOLD_ACTIVEFLAG                                               ");
  blk("  HOLD_ACTIVEFLAG_RESET                                         ");
  blk("  HOLD_INACTIVEFLAG                                             ");
  blk("  HOLD_INACTIVEFLAG_RESET                                       ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  APPCAST                                                       ");
  blk("  DESIRED_THRUST_L                                              ");
  blk("  DESIRED_THRUST_R                                              ");
  blk("                                                                ");
  blk("  <endflags>                                                    ");
  blk("  <activeflags>                                                 ");
  blk("  <inactiveflags>                                               ");
  blk("                                                                ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pPoseKeep", "gpl");
  exit(0);
}

