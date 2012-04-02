/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CTDSensor_Info.cpp                                */
/*    DATE: Jan 28th 2012                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/
 
#include <cstdlib>
#include <iostream>
#include "CTDSensor_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                        ");
  blk("------------------------------------                             ");
  blk("  Typically run in a shoreside community. Simulates a CTD        ");
  blk("  sensor on a vehicle. Currently only provides temperature.      ");
  blk("  Configured for a specific temepraturte field, currently        ");
  blk("  a propagating ocean front                                      ");
}

//----------------------------------------------------------------
// Procedure: showHelp

void showHelpAndExit()
{
  blk("                                                          ");
  blu("==========================================================");
  blu("Usage: uFldCTDSensor file.moos [OPTIONS]                  ");
  blu("==========================================================");
  blk("                                                          ");
  showSynopsis();
  blk("                                                          ");
  blk("Options:                                                  ");
  mag("  --alias","=<ProcessName>                                ");
  blk("      Launch uFldCTDSensor with the given process         ");
  blk("      name rather than uFldCTDSensor.                     ");
  mag("  --example, -e                                           ");
  blk("      Display example MOOS configuration block.           ");
  mag("  --help, -h                                              ");
  blk("      Display this help message.                          ");
  mag("  --interface, -i                                         ");
  blk("      Display MOOS publications and subscriptions.        ");
  mag("  --version,-v                                            ");
  blk("      Display release version of uFldCTDSensor.           ");
  mag("  --verbose","=<setting>                                  ");
  blk("      Set verbosity. true or false (default)              ");
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
  blu("uFldCTDSensor Example MOOS Configuration                     ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldCTDSensor                                ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // Configuring visual preferences                             ");
  blk("  show_source_pts      = true                     // default    ");
  blk("  scope_transparency   = 0.25                     // default    ");
  blk("                                                                ");
  blk("  source_point         = x=20,y=30,label=12                     ");
  blk("  default_benign_color = light_blue               // default    ");
  blk("  default_benign_width = 8                        // default    ");
  blk("  swath_transparency   = 0.2                      // default    ");
  blk("                                                                ");
  blk("  sensor_config = width=25, exp=4, class=0.80                   ");
  blk("  sensor_config = width=50, exp=2, class=0.60                   ");
  blk("  sensor_config = width=10, exp=6, class=0.93                   ");
  blk("  hazard_file   = hazards.txt                                   ");
  blk("  swath_length  = 5                               // default    ");
  blk("  seed_random   = false                           // default    ");
  blk("  verbose       = true                            // default    ");
  blk("                                                                ");
  blk("  show_hazards  = true  // default                // default    ");
  blk("  show_swath    = true  // default                // default    ");
  blk("  show_reports  = 60    // seconds (unlimited if unspecified)   ");
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
  blu("uFldCTDSensor INTERFACE                                      ");
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
  blk("  UCTD_SENSOR_REQUEST = vname=archie                             ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  UCTD_MSMNT_REPORT   = vname=betty,temperature=22.54           ");
  blk("  UGS_HAZARD_REPORT_<V> = temperature=22.54                     ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldCTDSensor", "gpl");
  exit(0);
}

