/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CRS_ExampleConfig.cpp                                */
/*    DATE: July 7th 2011                                        */
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
#include "ColorParse.h"
#include "CRS_Info_HP.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  Typically run in a shoreside community. Takes reports from    ");
  blk("  remote vehicles, notes their position. Takes a range request  ");
  blk("  from a remote vehicle and returns a range report indicating   ");
  blk("  that vehicle's range to nearby vehicles. Range requests may   ");
  blk("  or may not be answered dependent on inter-vehicle range.      ");
  blk("  Reports may also have noise added to their range values.      ");
}

//----------------------------------------------------------------
// Procedure: showHelp

void showHelpAndExit()
{
  blk("                                                          ");
  blu("==========================================================");
  blu("Usage: uFldContactRangeSensor file.moos [OPTIONS]         ");
  blu("==========================================================");
  blk("                                                          ");
  showSynopsis();
  blk("                                                          ");
  blk("Options:                                                  ");
  mag("  --alias","=<ProcessName>                                ");
  blk("      Launch uFldContactRangeSensor with the given        ");
  blk("      process name rather than uFldContactRangeSensor.    ");
  mag("  --example, -e                                           ");
  blk("      Display example MOOS configuration block.           ");
  mag("  --help, -h                                              ");
  blk("      Display this help message.                          ");
  mag("  --interface, -i                                         ");
  blk("      Display MOOS publications and subscriptions.        ");
  mag("  --version,-v                                            ");
  blk("      Display release version of uFldContactRangeSensor.  ");
  mag("  --verbose","=<setting>                                  ");
  blk("      Set verbosity. true or false (default)              ");
  mag("  --web,-w                                                ");
  blk("      Open browser to: https://oceanai.mit.edu/apps/uFldContactRangeSensor ");
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
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uFldContactRangeSensor Example MOOS Configuration               ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uContactRangeSensor                             ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // Configuring aspects of the vehicles in the sim             ");
  blk("  push_dist  = default = 100  // in meters or {nolimit}         ");
  blk("  pull_dist  = default = 100  // in meters or {nolimit}         ");
  blk("  ping_wait  = default = 30   // in seconds                     ");
  blk("                                                                ");
  blk("  allow_echo_types = uuv                                        ");
  blk("                                                                ");
  blk("  // Configuring manner of reporting                            ");
  blk("  report_vars    = short // or {long, both}                     ");
  blk("  ground_truth   = true  // or {false}                          ");
  blk("  verbose        = true  // or {false}                          ");
  blk("  display_pulses = true  // or {false}                          ");
  blk("                                                                ");
  blk("  // Configuring visual artifacts                               ");
  blk("  ping_color     = white                                        ");
  blk("  echo_color     = chartreuse                                   ");
  blk("                                                                ");
  blk("  // Configuring Artificial Noise                               ");
  blk("  rn_uniform_pct    = 0.5                                        ");
  blk("  rn_gaussian_sigma = 1.5                                       ");
  blk("                                                                ");
  blk("  // Configuring Sensor Arcs                                    ");
  blk("  sensor_arc = 45:135,225:315 // or {315:45} for front          ");
  blk("  // sensor_arc = 315:45      // just for front                 ");
  blk("  // sensor_arc = 360         // default, sets full circle      ");
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
  blu("uFldContactRangeSensor INTERFACE                                ");
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
  blk("  CRS_RANGE_REQUEST = name=archie                               ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  CRS_RANGE_REPORT    = vname=archie,range=126.54,              ");
  blk("                        target=jackal,time=19656022406.44       ");
  blk("  CRS_RANGE_REPORT_GT = vname=archie,range=126.54,              ");
  blk("                        target=jackal,time=19656022406.44       ");
  blk("  VIEW_RANGE_PULSE    = x=-40,y=-150,radius=40,duration=15,     ");
  blk("                        fill=0.25,fill_color=green,label=04,    ");
  blk("                        edge_color=green,time=3892830128.5,     ");
  blk("                        edge_size=1                             ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldContactRangeSensor", "gpl");
  exit(0);
}









