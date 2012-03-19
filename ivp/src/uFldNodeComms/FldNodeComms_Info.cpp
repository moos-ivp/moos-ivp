/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: FldNodeComms_Info.cpp                                */
/*   DATE: Dec 4th 2011                                         */
/****************************************************************/

#include <cstdlib>
#include <iostream>
#include "ColorParse.h"
#include "ReleaseInfo.h"
#include "FldNodeComms_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  A tool for managing comms between vehicles. Based on incoming ");
  blk("  and outgoing node reports. Comms determined on range and/or   ");
  blk("  vehicle team affiliation.                                     ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blk("Usage: uFldNodeComms file.moos [OPTIONS]                        ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uFldNodeComms with the given process               ");
  blk("      name rather than uFldNodeComms.                           ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uFldNodeComms.             ");
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
  blu("uFldNodeComms Example MOOS Configuration                        ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldNodeComms                                   ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  COMMS_RANGE    = 100      // default (in meters)              ");
  blk("  CRITICAL_RANGE = 30       // default (in meters)              ");
  blk("  STALE_TIME     = 5        // default (in seconds)             ");
  blk("                                                                ");
  blk("  MIN_MSG_INTERVAL = 30     // default (in seconds)             ");
  blk("  MAX_MSG_LENGTH   = 1000   // default (# of characters)        ");
  blk("                                                                ");
  blk("  VIEW_NODE_RPT_PULSES = true // default                        ");
  blk("                                                                ");
  blk("  VERBOSE  = true           // default                          ");
  blk("                                                                ");
  blk("  STEALTH  = vname=alpha, stealth=0.8                           ");
  blk("  EARANGE  = vname=alpha, earange=4.5                           ");
  blk("                                                                ");
  blk("  GROUPS   = true                                               ");
  blk("}                                                               ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uFldNodeComms INTERFACE                                         ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  NODE_MESSAGE      = src_node=henry,dest_node=ike,var_name=FOO ");
  blk("                      string_val=bar                            ");
  blk("  NODE_REPORT                                                   ");
  blk("  NODE_REPORT_LOCAL = NAME=alpha,TYPE=UUV,TIME=1252348077.59,   ");
  blk("                      X=51.71,Y=-35.50, LAT=43.824981,          ");
  blk("                      LON=-70.329755,SPD=2.0,HDG=118.8,         ");
  blk("                      YAW=118.8,DEPTH=4.6,LENGTH=3.8,           ");
  blk("                      MODE=MODE@ACTIVE:LOITERING                ");
  blk("                                                                ");
  blk("  UNC_VIEW_NODE_RPT_PULSES = false                              ");
  blk("  UNC_STEALTH = vname=alpha,stealth=0.4                         ");
  blk("  UNC_EARANGE = vname=alpha,earange=0.5                         ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  NODE_MESSAGE_<VNAME> = src_node=henry,dest_node=ike,          ");
  blk("                         var_name=FOO,string_val=bar            ");
  blk("  NODE_REPORT_<VNAME>  = NAME=alpha,TYPE=UUV,TIME=1252348077.5, ");
  blk("                         X=51.71,Y=-35.50, LAT=43.824981,       ");
  blk("                         LON=-70.329755,SPD=2.0,HDG=118.8,      ");
  blk("                         YAW=118.8,DEPTH=4.6,LENGTH=3.8,        ");
  blk("                         MODE=MODE@ACTIVE:LOITERING             ");
  blk("  VIEW_COMMS_PULSE     = label=one,sx=4,sy=2,tx=44,ty=55,       ");
  blk("                         beam_width=10,duration=5,fill=0.3,     ");
  blk("                         fill_color=yellow,edge_color=green     ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldNodeComms", "gpl");
  exit(0);
}

