/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NodeReporter_Info.cpp                                */
/*    DATE: July 6th 2011                                        */
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
#include "ReleaseInfo.h"
#include "NodeReporter_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  A tool for collecting node information such as present vehicle");
  blk("  position, trajectory and type, and posting it in a single     ");
  blk("  report for sharing between vehicles or sending to a shoreside ");
  blk("  display.                                                      ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: pNodeReporter file.moos [OPTIONS]                        ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pNodeReporter with the given process               ");
  blk("      name rather than pNodeReporter.                           ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pNodeReporter.             ");
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
  blu("pNodeReporter Example MOOS Configuration                        ");
  blu("=============================================================== ");
  blu("Blue lines:     ","Default configuration                        ");
  blk("                                                                ");
  blk("ProcessConfig = pNodeReporter                                   ");
  blk("{                                                               ");
  blu("  AppTick   = 4                                                 ");
  blu("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // Configure key aspects of the node                          ");
  blk("  platform_type      = glider  "," // or {uuv,auv,ship,kayak}   ");
  blk("  platform_length    = 8       "," // meters.  Range [0,inf)    ");
  blk("  platform_color     = green   "," // or any legal color        ");
  blk("                                                                ");
  blk("  // Configure optional blackout functionality                  ");
  blu("  blackout_interval  = 0       "," // seconds. Range [0,inf)    ");
  blk("                                                                ");
  blk("   // Configure the optional platform report summary            ");
  blk("  plat_report_input  = COMPASS_HEADING, gap=1                   ");
  blk("  plat_report_input  = GPS_SAT, gap=5                           ");
  blk("  plat_report_input  = WIFI_QUALITY, gap=1                      ");
  blu("  plat_report_output = PLATFORM_REPORT_LOCAL                    ");
  blk("                                                                ");
  blk("  // Configure the MOOS variable containg the node report       ");
  blu("  node_report_output = NODE_REPORT_LOCAL                        ");
  blk("                                                                ");
  blk("  // Threshold for conveying an absense of the helm             ");
  blu("  nohelm_threshold   = 5       "," // seconds                   ");
  blk("                                                                ");
  blk("  // Policy for filling in missing lat/lon from x/y or v.versa  ");
  blu("  crossfill_policy   = literal "," // or {fill-empty,use-latest}");
  blk("                                                                ");
  blk("  // Configure monitor/reporting of dual nav solution           ");
  blk("  alt_nav_prefix     = NAV_GT                                   ");
  blk("  alt_nav_name       = _GT                                      ");
  blk("                                                                ");
  blk("  // Declare the vehicle to be part of group tango.             ");
  blk("  group = tango                                                 ");
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
  blu("pNodeReporter INTERFACE                                         ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  NAV_X           = 112.3                                       ");
  blk("  NAV_Y           = -99.1                                       ");
  blk("  NAV_LAT         = 43.824981                                   ");
  blk("  NAV_LONG        = -70.329755                                  ");
  blk("  NAV_HEADING     = 118.8                                       ");
  blk("  NAV_SPEED       = 2.0                                         ");
  blk("  NAV_YAW         = 118.8                                       ");
  blk("  NAV_DEPTH       = 4.6                                         ");
  blk("  IVPHELM_SUMMARY =                                             ");
  blk("     iter=1,ofnum=1,warnings=0,utc_time=23839589908.76,         ");
  blk("     solve_time=0.00,create_time=0.00,loop_time=0.00,           ");
  blk("     var=course:195.0,var=speed:1.2,halted=false,               ");
  blk("     running_bhvs=none,modes=MODE@ACTIVE:LOITERING,             ");
  blk("     active_bhvs=loiter$23839589908.8$100$9$0.00$0/0$1$1,       ");
  blk("     idle_bhvs=waypt_return$0.0$0/0:station-keep$0.0$n/a,       ");
  blk("     completed_bhvs=none                                        ");
  blk("  IVPHELM_ENGAGED = ENGAGED                                     ");
  blk("  IVPHELM_ALLSTOP = clear                                       ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  NODE_REPORT                                                   ");
  blk("  NODE_REPORT_LOCAL = NAME=alpha,TYPE=UUV,TIME=1252348077.59,   ");
  blk("                      COLOR=red,X=51.71,Y=-35.50, LAT=43.824981,");
  blk("                      LON=-70.329755,SPD=2.0,HDG=118.8,         ");
  blk("                      YAW=118.8,DEPTH=4.6,LENGTH=3.8,           ");
  blk("                      MODE=MODE@ACTIVE:LOITERING,               ");
  blk("                      THRUST_MODE_REVERSE=true                  ");
  blk("                                                                ");
  blk("  NODE_REPORT_LOCAL_FIRST = posted just once at startup, a copy ");
  blk("                            of NODE_REPORT_LOCAL in case one    ");
  blk("                            wants to not log the regular reports");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pNodeReporter", "gpl");
  exit(0);
}









