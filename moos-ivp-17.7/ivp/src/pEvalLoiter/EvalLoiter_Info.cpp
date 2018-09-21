/**************************************************************/
/*    NAME: Michael Benjamin                                  */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA  */
/*    FILE: EvalLoiter_Info.h                                 */
/*    DATE: January 6th, 2016                                 */
/**************************************************************/

#include <cstdlib>
#include <iostream>
#include "EvalLoiter_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The pEvalLoiter application is used for evaluating vehicle    ");
  blk("  efficiency in both (a) traversing to a loiter polygon, and    ");
  blk("  (b) traversing the polygon once there.                        ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: pEvalLoiter file.moos [OPTIONS]                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pEvalLoiter with the given process name            ");
  blk("      rather than pEvalLoiter.                                  ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pEvalLoiter.               ");
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
  blu("pEvalLoiter Example MOOS Configuration                          ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pEvalLoiter                                     ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  eval_window      = 30     // (default is 30, in seconds)      ");
  blk("  nav_stale_thresh = 10     // (default is 10, in seconds)      ");
  blk("  mode_thresh      = 10     // (default is 10, in meters)       ");
  blk("  max_nav_age      = 15     // (default is 15, in seconds)      ");
  blk("  range_max_ineff  = 50     // (default is 50, in meters)       ");
  blk("                                                                ");
  blk("  notable_cpa_thresh = 10  (meters)                             ");
  blk("  notable_eff_thresh = 50  (percent)                            ");
  blk("  notable_cpa_var    = NOTABLE_CPA                              ");
  blk("  notable_eff_var    = NOTABLE_EFF                              ");
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
  blu("pEvalLoiter INTERFACE                                           ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  NODE_REPORT_LOCAL = NAME=alpha,TYPE=UUV,TIME=1252348077.59,   ");
  blk("                      X=51.71,Y=-35.50, LAT=43.824981,          ");
  blk("                      LON=-70.329755,SPD=2.0,HDG=118.8,         ");
  blk("                      YAW=118.8,DEPTH=4.6,LENGTH=3.8,           ");
  blk("                      MODE=MODE@ACTIVE:LOITERING                ");
  blk("  EVAL_LOITER_REQ =   id=129, cpa=23.19                         ");
  blk("  BHV_SETTINGS    =                                             ");
  blk("                                                                ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  ENCOUNTER_SUMMVAL = 87.66   (every encounter)                 ");
  blk("                                                                ");
  blk("  ENCOUNTER_SUMMARY = id=4,cpa=11.832,v2=deb,eff=87.31,v1=abe,  ");
  blk("                      type=eval_loiter                          ");
  blk("                                                                ");
  blk("  NOTABLE_CPA = 8.43    (some encounters)                       ");
  blk("  NOTABLE_EFF = 49.1    (some encounters)                       ");
  blk("                                                                ");
  blk("  EVAL_LOITER_PT_EVAL = 98.23 (every app iteration)             ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pEvalLoiter", "gpl");
  exit(0);
}

