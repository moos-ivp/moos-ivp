/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PID_ExampleConfig.cpp                                */
/*    DATE: July 6th 2011                                        */
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
 
#include <iostream>
#include "PID_ExampleConfig.h"
#include "ColorParse.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showExampleConfig

void showExampleConfig()
{
  blk("                                                                ");
  blk("=============================================================== ");
  blk("pMarinePID Example MOOS Configuration                           ");
  blk("=============================================================== ");
  mag("Magenta lines:  ","Non-default configuration                    ");
  blu("Blue lines:     ","Default configuration, i.e., the below blue  ");
  blu("  lines, as configured, could be ommitted with no net effect.    ");
  blk("                                                                ");
  blk("ProcessConfig = pMarinePID                                      ");
  blk("{                                                               ");
  mag("  AppTick   = 20         "," // Default MOOS AppTick=4          ");
  mag("  CommsTick = 20         "," // Default MOOS CommsTick=4        ");
  blk("                                                                ");
  mag("  speed_factor    = 20   "," // Range [0,inf)                   ");
  blu("  sim_instability = 0    "," // Range [0,inf)                   ");
  blk("                                                                ");
  blu("  tardy_helm_threshold = 2.0                                    ");
  blu("  tardy_nav_threshold  = 2.0                                    ");
  blu("  active_start         = false   "," // or {true}               ");
  blu("  verbose              = terse   "," // or {verbose, quiet}     ");
  blk("                                                                ");
  blk("  // Yaw control configuration                                  ");
  mag("  yaw_pid_kp  = 0.5                                             ");
  mag("  yaw_pid_kd  = 0.1                                             ");
  mag("  yaw_pid_ki  = 0.012                                           ");
  mag("  yaw_pid_integral_limit = 0.07                                 ");
  mag("  yaw_pid_ki_limit       = 0.2                                  ");
  mag("  max_rudder  = 100                                             ");
  blk("                                                                ");
  blk("  // Speed control configuration                                ");
  mag("  speed_pid_kp = 0.8                                            ");
  mag("  speed_pid_kd = 0.1                                            ");
  mag("  speed_pid_ki = 0.11                                           ");
  mag("  speed_pid_ki_limit = 0.07                                     ");
  mag("  max_thrust   = 100                                            ");
  blk("                                                                ");
  blk("  // Depth control configuration                                ");
  mag("  depth_control      = true  "," // or {false}                  ");
  mag("  z_to_pitch_pid_kp  = 0.12                                     ");
  mag("  z_to_pitch_pid_kd  = 1.0                                      ");
  mag("  z_to_pitch_pid_ki  = 0.004                                    ");
  mag("  z_to_pitch_pid_integral_limit = 0.05                          ");
  mag("  max_pitch          = 15                                       ");
  blk("                                                                ");
  mag("  // Depth control configuration                                ");
  mag("  pitch_pid_kp       = 0.5                                      ");
  mag("  pitch_pid_kd       = 1.0                                      ");
  mag("  pitch_pid_ki       = 0                                        ");
  mag("  pitch_pid_integral_limit = 0                                  ");
  mag("  max_elevator       = 13                                       ");
  blk("}                                                               ");
  blk("                                                                ");
}
