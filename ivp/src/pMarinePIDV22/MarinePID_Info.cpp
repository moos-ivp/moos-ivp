/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: MarinePID_Info.cpp                                   */
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
#include "MarinePID_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  Provides a basic PID controller interface to control heading  ");
  blk("  speed and depth based on incoming desired heading, speed and  ");
  blk("  depth objectives and output on rudder, thrust and elevator    ");
  blk("  fed through three PID controllers.                            ");
}


//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                           ");
  blu("========================================================== ");
  blu("Usage: pMarinePIDV22 file.moos [OPTIONS]                  ");
  blu("========================================================== ");
  blk("                                                           ");
  showSynopsis();
  blk("                                                           ");
  blk("Options:                                                   ");
  mag("  --alias","=<ProcessName>                                 ");
  blk("      Launch pMarinePIDV22 with the given process name     ");
  blk("      rather than pMarinePIDV22    .                       ");
  mag("  --example, -e                                            ");
  blk("      Display example MOOS configuration block.            ");
  mag("  --help, -h                                               ");
  blk("      Display this help message.                           ");
  mag("  --interface, -i                                          ");
  blk("      Display MOOS publications and subscriptions.         ");
  mag("  --version,-v                                             ");
  blk("      Display release version of pMarinePIDV22.            ");
  blk("                                                           ");
  blk("Note: If argv[2] does not otherwise match a known option,  ");
  blk("      then it will be interpreted as a run alias. This is  ");
  blk("      to support pAntler launching conventions.            ");
  blk("                                                           ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("pMarinePIDV22 Example MOOS Configuration                        ");
  blu("=============================================================== ");
  blu("Blue lines:     ","Default configuration, i.e., the below blue  ");
  blu("  lines, as configured, could be ommitted with no net effect.   ");
  blk("                                                                ");
  blk("ProcessConfig = pMarinePIDV22                                   ");
  blk("{                                                               ");
  blk("  AppTick   = 20         "," // Default MOOS AppTick=4          ");
  blk("  CommsTick = 20         "," // Default MOOS CommsTick=4        ");
  blk("                                                                ");
  blk("  speed_factor    = 20   "," // Range [0,inf)                   ");
  blu("  sim_instability = 0    "," // Range [0,inf)                   ");
  blk("                                                                ");
  blu("  max_sat_hdg_debug = false                                     ");
  blu("  max_sat_spd_debug = false                                     ");
  blu("  max_sat_dep_debug = false                                     ");
  blk("                                                                ");
  blu("  tardy_helm_threshold = 2.0                                    ");
  blu("  tardy_nav_threshold  = 2.0                                    ");
  blu("  verbose              = terse   "," // or {verbose, quiet}     ");
  blk("                                                                ");
  blk("  // Yaw control configuration                                  ");
  blk("  yaw_pid_kp  = 0.5                                             ");
  blk("  yaw_pid_kd  = 0.1                                             ");
  blk("  yaw_pid_ki  = 0.012                                           ");
  blk("  yaw_pid_integral_limit = 0.07                                 ");
  blk("  yaw_pid_ki_limit       = 0.2                                  ");
  blk("  maxrudder   = 100                                             ");
  blk("                                                                ");
  blk("  // Speed control configuration                                ");
  blk("  speed_pid_kp = 0.8                                            ");
  blk("  speed_pid_kd = 0.1                                            ");
  blk("  speed_pid_ki = 0.11                                           ");
  blk("  speed_pid_ki_limit = 0.07                                     ");
  blk("  maxthrust    = 100                                            ");
  blk("                                                                ");
  blk("  // Depth control configuration                                ");
  blk("  depth_control      = true  "," // or {false}                  ");
  blk("  z_to_pitch_pid_kp  = 0.12                                     ");
  blk("  z_to_pitch_pid_kd  = 1.0                                      ");
  blk("  z_to_pitch_pid_ki  = 0.004                                    ");
  blk("  z_to_pitch_pid_integral_limit = 0.05                          ");
  blk("  maxpitch           = 15                                       ");
  blk("                                                                ");
  blk("  // Depth control configuration                                ");
  blk("  pitch_pid_kp       = 0.5                                      ");
  blk("  pitch_pid_kd       = 1.0                                      ");
  blk("  pitch_pid_ki       = 0                                        ");
  blk("  pitch_pid_integral_limit = 0                                  ");
  blk("  maxelevator        = 13                                       ");
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
  blu("pMarinePIDV22 INTERFACE                                         ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  DESIRED_HEADING = 189                                         ");
  blk("  DESIRED_SPEED   = 3.2                                         ");
  blk("  DESIRED_DEPTH   = 15                                          ");
  blk("  NAV_HEADING     = 182                                         ");
  blk("  NAV_YAW         = 0.12                                        ");
  blk("  NAV_SPEED       = 3.4                                         ");
  blk("  NAV_DEPTH       = 22                                          ");
  blk("  NAV_PITCH       = -1.9                                        ");
  blk("  SPEED_FACTOR    = 20                                          ");
  blk("  PID_VERBOSE     = verbose/quiet/terse                         ");
  blk("  MOOS_MANUAL_OVERIDE  = false                                  ");
  blk("  MOOS_MANUAL_OVERRIDE = false                                  ");
  blk("                                                                ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  DESIRED_RUDDER   = -22                                        ");
  blk("  DESIRED_THRUST   = 60                                         ");
  blk("  DESIRED_ELEVATOR = -1.2                                       ");
  blk("                                                                ");
  blk("  PID_MAX_SAT_HDG = true                                        ");
  blk("  PID_MAX_SAT_SPD = true                                        ");
  blk("  PID_MAX_SAT_DEP = true                                        ");
  blk("                                                                ");
  blk("  PID_MAX_SAT_HDG_DEBUG = dfeIn=0.00000,                        ");
  blk("        dfErrorTime=18677697.28731, m_dfDT=0.05311,             ");
  blk("        dfDiffNow=0.00000, DiffHistSize=9, mdfeDiff(1)=0.00000, ");
  blk("        mdfeDiff(2)=0.00000, m_dfOut(1)0                        ");
  blk("  PID_MAX_SAT_SPD_DEBUG = dfeIn=0.00000, ...                    ");
  blk("  PID_MAX_SAT_DEP_DEBUG = dfeIn=0.00000, ...                    ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pMarinePIDV22", "gpl");
  exit(0);
}















