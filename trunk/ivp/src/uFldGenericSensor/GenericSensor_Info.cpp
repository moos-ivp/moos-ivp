/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GenericSensor_Info.cpp                               */
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
#include "GenericSensor_Info.h"
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
  blk("  source objects with a given x,y location and label.            ");
  blk("  The sensor simulator receives a series of requests from a      ");
  blk("  remote vehicle. When the sensor determines that an object is   ");
  blk("  is within the sensor field of a requesting vehicle, it may or  ");
  blk("  may not return a sensor detection report for the object.       ");
  blk("  The report consists of a point in the x-y space within a given ");
  blk("  range from the source object. The vehicle is configured with   ");
  blk("  one of several possible \"detection ranges\". The sensor will  ");
  blk("  not produce a value unless the source point is within that     ");
  blk("  range of the vehicle, and the reported point produced by the   ");
  blk("  sensor is also within that range from the source point.        ");
}

//----------------------------------------------------------------
// Procedure: showHelp

void showHelpAndExit()
{
  blk("                                                          ");
  blu("==========================================================");
  blu("Usage: uFldGenericSensor file.moos [OPTIONS]              ");
  blu("==========================================================");
  blk("                                                          ");
  showSynopsis();
  blk("                                                          ");
  blk("Options:                                                  ");
  mag("  --alias","=<ProcessName>                                ");
  blk("      Launch uFldGenericSensor with the given process     ");
  blk("      name rather than uFldGenericSensor.                 ");
  mag("  --example, -e                                           ");
  blk("      Display example MOOS configuration block.           ");
  mag("  --help, -h                                              ");
  blk("      Display this help message.                          ");
  mag("  --interface, -i                                         ");
  blk("      Display MOOS publications and subscriptions.        ");
  mag("  --version,-v                                            ");
  blk("      Display release version of uFldGenericSensor.       ");
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
  blu("uFldGenericSensor Example MOOS Configuration                    ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldGenericSensor                               ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // Configuring visual preferences                             ");
  blk("  show_source_pts     = true                    // default      ");
  blk("  scope_transparency  = 0.25                    // default      ");
  blk("  color_source_pts    = green                   // default      ");
  blk("  show_source_pts     = true                    // default      ");
  blk("                                                                ");
  blk("  // Configure the Sensed Object Laydown                        ");
  blk("  source_point        = x=-20,y=30,label=07                     ");
  blk("  source_point        = x=45,y=-93,label=12                     ");
  blk("                                                                ");
  blk("  // Configure the Sensor Properties                            ");
  blk("  min_reset_interval  = 300                     // default      ");
  blk("  min_sensor_interval = 30                      // default      ");
  blk("  sensor_config       = range = 30                              ");
  blk("  sensor_config       = range = 100                             ");
  blk("  sensor_config       = range = 60                              ");
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
  blu("uFldGenericSensor INTERFACE                                      ");
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
  blk("  UGS_SENSOR_REQUEST = vname=archie                             ");
  blk("  UGS_CONFIG_REQUEST = vname=archie,range=25                    ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  UGS_HAZARD_REPORT   = vname=betty,x=51,y=11.3,hazard=true,    ");
  blk("                        label=12                                ");
  blk("  UGS_HAZARD_REPORT_<V> = x=51,y=11.3,hazard=true,label=12      ");
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
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldGenericSensor", "gpl");
  exit(0);
}









