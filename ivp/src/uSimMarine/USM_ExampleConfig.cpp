/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: USM_ExampleConfig.cpp                                */
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
#include "USM_ExampleConfig.h"
#include "ColorParse.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showExampleConfig

void showExampleConfig()
{
  blk("                                                                ");
  blk("=============================================================== ");
  blk("uSimMarine Example MOOS Configuration                           ");
  blk("=============================================================== ");
  blu("Blue lines:     ","Default configuration                        ");
  mag("Magenta lines:  ","Non-default configuration                    ");
  blk("                                                                ");
  blk("ProcessConfig = uSimMarine                                      ");
  blk("{                                                               ");
  blu("  AppTick   = 4                                                 ");
  blu("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blu("  start_x       = 0                                             ");
  blu("  start_y       = 0                                             ");
  blu("  start_heading = 0                                             ");
  blu("  start_speed   = 0                                             ");
  blu("  start_depth   = 0                                             ");
  mag("  start_pos     = x=0, y=0, speed=0, heading=0, depth=0         ");
  blk("                                                                ");
  blu("  force_x       = 0                                             ");
  blu("  force_y       = 0                                             ");
  blu("  force_theta   = 0                                             ");
  mag("  force_vector  = 0,0     "," // heading, magnitude             ");
  blk("                                                                ");  
  blu("  buoyancy_rate        = 0.025 ","// meters/sec                 ");
  blu("  max_acceleration     = 0     ","// meters/sec^2               ");
  blu("  max_deceleration     = 0.5   ","// meters/sec^2               ");
  blu("  max_depth_rate       = 0.5   ","// meters/sec                 ");
  blu("  max_depth_rate_speed = 2.0   ","// meters/sec                 ");
  blk("                                                                ");
  blu("  sim_pause            = false ","// or {true}                  ");
  blu("  dual_state           = false ","// or {true}                  ");
  blu("  thrust_reflect       = false ","// or {true}                  ");
  blu("  thrust_factor        = 20    ","// range [0,inf)              ");
  blu("  turn_rate            = 70    ","// range [0,100]              ");
  mag("  thrust_map           = 0:0, 20:1, 40:2, 60:3, 80:5, 100:5     ");
  blk("}                                                               ");
  blk("                                                                ");

}
