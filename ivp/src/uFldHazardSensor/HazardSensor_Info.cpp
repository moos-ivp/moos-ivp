/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardSensor_Info.cpp                                */
/*    DATE: Jan 28th 2012                                        */
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
#include "HazardSensor_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                        ");
  blk("------------------------------------                             ");
  blk("  Typically run in a shoreside community. Configured with a set  ");
  blk("  objects with a given x,y location and classification (hazard   ");
  blk("  or benign). The sensor simulator receives a series of requests ");
  blk("  from a remote vehicle. When sensor determines that an object   ");
  blk("  is within the sensor field of a requesting vehicle, it may or  ");
  blk("  may not return a sensor detection report for the object, and   ");
  blk("  perhaps also a proper classification. The odds of receiving a  ");
  blk("  detection and proper classification depend on the sensor       ");
  blk("  configuration and the user's preference for P_D/P_FA on the    ");
  blk("  prevailing ROC curve.                                          ");
}

//----------------------------------------------------------------
// Procedure: showHelp

void showHelpAndExit()
{
  blk("                                                          ");
  blu("==========================================================");
  blu("Usage: uFldHazardSensor file.moos [OPTIONS]               ");
  blu("==========================================================");
  blk("                                                          ");
  showSynopsis();
  blk("                                                          ");
  blk("Options:                                                  ");
  mag("  --alias","=<ProcessName>                                ");
  blk("      Launch uFldHazardSensor with the given process      ");
  blk("      name rather than uFldHazardSensor.                  ");
  mag("  --example, -e                                           ");
  blk("      Display example MOOS configuration block.           ");
  mag("  --help, -h                                              ");
  blk("      Display this help message.                          ");
  mag("  --interface, -i                                         ");
  blk("      Display MOOS publications and subscriptions.        ");
  mag("  --version,-v                                            ");
  blk("      Display release version of uFldHazardSensor.        ");
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
  blu("uFldHazardSensor Example MOOS Configuration                     ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldHazardSensor                                ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // Common to all appcasting MOOSApps                          ");
  blk("  term_report_interval = 0.4                      // default    ");
  blk("  max_appcast_events   = 8                        // default    ");
  blk("  max_appcast_run_warnings = 10                   // default    ");
  blk("                                                                ");
  blk("  // Configuring visual preferences                             ");
  blk("  default_hazard_shape = triangle                 // default    ");
  blk("  default_hazard_color = green                    // default    ");
  blk("  default_hazard_width = 8                        // default    ");
  blk("                                                                ");
  blk("  default_benign_shape = triangle                 // default    ");
  blk("  default_benign_color = light_blue               // default    ");
  blk("  default_benign_width = 8                        // default    ");
  blk("  swath_transparency   = 0.2                      // default    ");
  blk("                                                                ");
  blk("  sensor_config = width=25, exp=4, class=0.80                   ");
  blk("  sensor_config = width=50, exp=2, class=0.60                   ");
  blk("  sensor_config = width=10, exp=6, class=0.93,max=1             ");
  blk("  hazard_file   = hazards.txt                                   ");
  blk("  swath_length  = 5                               // default    ");
  blk("  seed_random   = false                           // default    ");
  blk("                                                                ");
  blk("  show_hazards  = true  // default                // default    ");
  blk("  show_swath    = true  // default                // default    ");
  blk("  show_detections = 60  // seconds (unlimited if unspecified)   ");
  blk("  show_pd       = true  // pd shown with swaths   // default    ");
  blk("  show_pfa      = true  // pfa shown with swaths  // default    ");
  blk("                                                                ");
  blk("  min_reset_interval       = 300  // default                    ");
  blk("  min_classify_interval    = 30   // default                    ");
  blk("  options_summary_interval = 10   // default                    ");
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
  blu("uFldHazardSensor INTERFACE                                      ");
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
  blk("  UHZ_CLASSIFY_REQUEST = vname=archie,label=07,priority=50      ");
  blk("  UHZ_CONFIG_REQUEST   = vname=archie,width=50,pd=0.9           ");
  blk("  UHZ_SENSOR_REQUEST   = vname=archie                           ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  UHZ_DETECTION_REPORT = vname=betty,x=51,y=11.3,label=12       ");
  blk("  UHZ_DETECTION_REPORT_<V> = x=51,y=11.3,label=12               ");
  blk("  UHZ_HAZARD_REPORT   = vname=betty,x=51,y=11.3,type=hazard,    ");
  blk("                        label=12                                ");
  blk("  UHZ_HAZARD_REPORT_<V> = x=51,y=11.3,type=hazard,label=12      ");
  blk("  UHZ_CONFIG_ACK      = vname=archie,width=20,pd=0.9,pfa=0.53,  ");
  blk("                        pclass=0.91                             ");
  blk("                                                                ");
  blk("  UHZ_OPTIONS_SUMMARY = width=10,exp=6,class=0.9:width=25,exp=4,");
  blk("                        class=0.85:width=50,exp=2,class=0.78    ");
  blk("                                                                ");
  blk("  VIEW_CIRCLE         = x=-150.3,y=-117.5,radius=10,edge_size=1 ");
  blk("  (detection)           edge_color=white,fill_color=white,      ");
  blk("                        vertex_size=0,fill_transparency=0.3     ");
  blk("                        type=circle,width=4                     ");
  blk("                                                                ");
  blk("  VIEW_MARKER         = x=217.3,y=-16.7,width=8,label=6,        ");
  blk("  (hazards)             primary_color=green,type=triangle       ");
  blk("                                                                ");
  blk("  VIEW_POLYGON        = pts={-156.7,-314.6:-160.2,-305.2:-113.3,");
  blk("  (sensor swath)        -287.7:-109.8,-297.1},msg=_null_,       ");
  blk("                        label=sensor_swath_archie,vertex_size=0,");
  blk("                        fill_color=white,edge_size=0,           ");
  blk("                        fill_transparency=0.25                  ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldHazardSensor", "gpl");
  exit(0);
}









