/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BRS_Info.cpp                                         */
/*    DATE: July 5th 2011                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#include <cstdlib>
#include <iostream>
#include "BRS_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  Typically run in a shoreside community. Configured with one or");
  blk("  more beacons with known beacon locations. Takes range requests");
  blk("  from a remote vehicle and returns a range report indicating   ");
  blk("  that vehicle's range to nearby beacons.  Range requests may   ");
  blk("  or may not be answered depending on range to beacon. Reports  ");
  blk("  may have noise added and may or may not include beacon ID.    ");
}

//----------------------------------------------------------------
// Procedure: showHelp

void showHelpAndExit()
{
  blk("                                                          ");
  blu("==========================================================");
  blu("Usage: uFldBeaconRangeSensor file.moos [OPTIONS]          ");
  blu("==========================================================");
  blk("                                                          ");
  showSynopsis();
  blk("                                                          ");
  blk("Options:                                                  ");
  mag("  --alias","=<ProcessName>                                ");
  blk("      Launch uFldBeaconRangeSensor with the given process ");
  blk("      name rather than uFldBeaconRangeSensor.             ");
  mag("  --example, -e                                           ");
  blk("      Display example MOOS configuration block.           ");
  mag("  --help, -h                                              ");
  blk("      Display this help message.                          ");
  mag("  --interface, -i                                         ");
  blk("      Display MOOS publications and subscriptions.        ");
  mag("  --version,-v                                            ");
  blk("      Display release version of uFldBeaconRangeSensor.   ");
  mag("  --verbose","=<setting>                                  ");
  blk("      Set verbosity. true or false (default)              ");
  mag("  --web,-w                                                ");
  blk("      Open browser to: https://oceanai.mit.edu/apps/uFldBeaconRangeSensor ");
  blk("                                                          ");
  blk("Note: If argv[2] does not otherwise match a known option, ");
  blk("      then it will be interpreted as a run alias. This is ");
  blk("      to support pAntler launching conventions.           ");
  blk("                                                          ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blu("=============================================================== ");
  blu("uFldBeaconRangeSensor Example MOOS Configuration                ");
  blu("=============================================================== ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // Configuring aspects of vehicles in the sim                 ");
  blk("  reach_distance = default = 200  "," // or {nolimit}           ");
  blk("  reach_distance = henry = 40     "," // meters                 ");
  blk("  ping_wait      = default = 30   "," // seconds                ");
  blk("  ping_wait      = henry   = 120                                ");
  blk("  ping_payments  = upon_response  "," // or {upon_receipt, upon_request}");
  blk("                                                                ");
  blk("  // Configuring manner of reporting                            ");
  blk("  report_vars    = short  ","// or {long, both}                 ");
  blk("  ground_truth   = true   ","// or {false}                      ");
  blk("  verbose        = true   ","// or {false}                      ");
  blk("                                                                ");
  blk("  // Configuring default beacon properties                      ");
  blk("  default_beacon_shape = circle    ","// or {square, diamond, etc.}  ");
  blk("  default beacon_color = orange    ","// or {red, green, etc.}       ");
  blk("  default_beacon_width = 4                                      ");
  blk("  default_beacon_report_range = 100                             ");
  blk("  default_beacon_freq  = never     ","// or [0,inf]             ");
  blk("  default_beacon_push_dist = 100                                ");
  blk("  default_beacon_pull_dist = 100                                ");
  blk("                                                                ");
  blk("  // Configuring Beacon properties                              ");
  blk("  beacon = x=200, y=435, label=01, report_range=45              ");
  blk("  beacon = x=690, y=205, label=02, freq=90                      ");
  blk("  beacon = x=350, y=705, label=03, width=8, color=blue          ");
  blk("                                                                ");
  blk("  // Configuring Artificial Noise                               ");
  blk("  rn_algorithm  =  uniform,pct=0  "," // pct may be in [0,1]    ");
  blk("                                                                ");
  blk("  app_logging = true  // {true or file} By default disabled     ");
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
  blu("uFldBeaconRangeSensor INTERFACE                                 ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  NODE_REPORT                                                   ");
  blk("  NODE_REPORT_LOCAL = NAME=alpha,TYPE=UUV,TIME=1252348077.59,   ");
  blk("                      X=51.71,Y=-35.50, LAT=43.824981,          ");
  blk("                      LON=-70.329755,SPD=2.0,HDG=118.8,         ");
  blk("                      YAW=118.8,DEPTH=4.6,LENGTH=3.8,           ");
  blk("                      MODE=MODE@ACTIVE:LOITERING                ");
  blk("  BRS_RANGE_REQUEST = name=archie                               ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  BRS_RANGE_REPORT    = vname=alpha,range=26.54,                ");
  blk("                        time=19656022406.44                     ");
  blk("  BRS_RANGE_REPORT_GT = vname=archie,range=126.54,              ");
  blk("                        time=19656022406.44                     ");
  blk("  BRS_DEBUG           = Invalid incoming node report            ");
  blk("  VIEW_MARKER         = x=400,y=-200,label=02,color=orange,     ");
  blk("                        type=circle,width=4                     ");
  blk("  VIEW_RANGE_PULSE    = x=4,y=15,radius=40,duration=15,label=04,");
  blk("                        fill=0.25,fill_color=green,edge_size=1, ");
  blk("                        edge_color=green,time=3892830128.5      ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldBeaconRangeSensor", "gpl");
  exit(0);
}










