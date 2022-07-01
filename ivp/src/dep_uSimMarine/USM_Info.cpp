/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: USM_Info.cpp                                         */
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
#include "USM_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;
//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The uSimMarine application is a simple 3D vehicle simulator   ");
  blk("  that updates vehicle state, position and trajectory, based on ");
  blk("  the present actuator values and prior vehicle state. Typical  ");
  blk("  usage scenario has a single instance of uSimMarine associated ");
  blk("  with each simulated vehicle.                                  ");
}


//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: uSimMarine file.moos [OPTIONS]                           ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uSimMarine with the given process name rather      ");
  blk("      than uSimMarine.                                          ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uSimMarine.                ");
  mag("  --web,-w                                                      ");
  blk("      Open browser to: https://oceanai.mit.edu/ivpman/apps/uSimMarine");
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
  blu("uSimMarine Example MOOS Configuration                           ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uSimMarine                                      ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  start_x       = 0                                             ");
  blk("  start_y       = 0                                             ");
  blk("  start_heading = 0                                             ");
  blk("  start_speed   = 0                                             ");
  blk("  start_depth   = 0                                             ");
  blk("  start_pos     = x=0, y=0, speed=0, heading=0, depth=0         ");
  blk("                                                                ");
  blk("  drift_x       = 0                                             ");
  blk("  drift_y       = 0                                             ");
  blk("  rotate_speed  = 0                                             ");
  blk("  drift_vector  = 0,0     "," // heading, magnitude             ");
  blk("                                                                ");  
  blk("  buoyancy_rate        = 0.025 ","// meters/sec                 ");
  blk("  max_acceleration     = 0     ","// meters/sec^2               ");
  blk("  max_deceleration     = 0.5   ","// meters/sec^2               ");
  blk("  max_depth_rate       = 0.5   ","// meters/sec                 ");
  blk("  max_depth_rate_speed = 2.0   ","// meters/sec                 ");
  blk("                                                                ");
  blk("  sim_pause            = false ","// or {true}                  ");
  blk("  dual_state           = false ","// or {true}                  ");
  blk("  thrust_reflect       = false ","// or {true}                  ");
  blk("  thrust_factor        = 20    ","// range [0,inf)              ");
  blk("  turn_rate            = 70    ","// range [0,100]              ");
  blk("  thrust_map           = 0:0, 20:1, 40:2, 60:3, 80:5, 100:5     ");
  blk("                                                                ");
  blk("  prefix               = NAV_  ","// default is USM_            ");
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
  blu("uSimMarine INTERFACE                                            ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  DESIRED_THRUST     = [-100,100]                               ");
  blk("  DESIRED_RUDDER     = [-100,100]                               ");
  blk("  DESIRED_ELEVATOR   = [-100,100]                               ");
  blk("                                                                ");
  blk("  DESIRED_THRUST_L   = [-100,100]                               ");
  blk("  DESIRED_THRUST_R   = [-100,100]                               ");
  blk("                                                                ");
  blk("  THRUST_MODE_REVERSE      = [true/false]                       ");
  blk("  THRUST_MODE_DIFFERENTIAL = [true/false]                       ");
  blk("                                                                ");
  blk("  BUOYANCY_CONTROL   = [-inf,+inf]                              ");
  blk("  BUOYANCY_RATE      = [-inf,+inf] m/s                          ");
  blk("  CURRENT_FIELD      = [true/false]                             ");
  blk("  DRIFT_X/CURRENT_X  = [-inf,+inf] m/s                          ");
  blk("  DRIFT_Y/CURRENT_Y  = [-inf,+inf] m/s                          ");
  blk("  DRIFT_VECTOR       = [0,360),[0,+inf]                         ");
  blk("  DRIFT_VECTOR_ADD   = [-inf,+inf]                              ");
  blk("  DRIFT_VECTOR_MULT  = [-inf,+inf]                              ");
  blk("  ROTATE_SPEED       = [0,inf] m/s                              ");
  blk("  TRIM_CONTROL       = [-inf,+inf]                              ");
  blk("  WATER_DEPTH        = [0,+inf]                                 ");
  blk("                                                                ");
  blk("  USM_RESET            (value not read)                         ");
  blk("  USM_SIM_PAUSED     = [true/false]                             ");
  blk("  USM_ENABLED        = [true/false]                             ");
  blk("  USM_TURN_RATE      = [0,100]                                  ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  BUOYANCY_REPORT         = status=2,error=0,buoyancy=0.0       ");
  blk("  TRIM_CONTROL            = status=2,error=0,trim_pitch=0.0,    ");
  blk("                            trim_roll=0.0                       ");
  blk("  USM_ALTITUDE            = 100                                 ");
  blk("  USM_DEPTH               = 45                                  ");
  blk("  USM_DRIFT_SUMMARY       = ang=90, mag=1.5, xmag=90, ymag=0    ");
  blk("  USM_HEADING             = 197                                 ");
  blk("  USM_HEADING_OVER_GROUND = 192                                 ");
  blk("  USM_LAT                 = 42.1293844                          ");
  blk("  USM_LONG                = -73.2398311                         ");
  blk("  USM_SPEED               = 1.33                                ");
  blk("  USM_SPEED_OVER_GROUND   = 2.09                                ");
  blk("  USM_X                   = 34.9                                ");
  blk("  USM_Y                   = 442.5                               ");
  blk("  USM_YAW                 = 197                                 ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uSimMarine   ", "gpl");
  exit(0);
}









