/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SIMAS_ExampleConfig.cpp                              */
/*    DATE: July 7th 2011                                        */
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
#include "SIMAS_ExampleConfig.h"
#include "ColorParse.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showExampleConfig

void showExampleConfig()
{
  blk("                                                                ");
  blk("=============================================================== ");
  blk("uSimActiveSonar Example MOOS Configuration                      ");
  blk("=============================================================== ");
  blu("Blue lines:     ","Default configuration                        ");
  mag("Magenta lines:  ","Non-default configuration                    ");
  blk("                                                                ");
  blk("ProcessConfig = uSimActiveSonar                                 ");
  blk("{                                                               ");
  blu("  AppTick   = 4                                                 ");
  blu("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // Configuring aspects of the vehicles in the sim             ");
  blu("  reach_distance = default = 100  // in meters or {nolimit}     ");
  blu("  reply_distance = default = 100  // in meters or {nolimit}     ");
  blu("  ping_wait      = default = 30   // in seconds                 ");
  blk("                                                                ");
  blk("  // Configuring manner of reporting                            ");
  blu("  report_vars    = short // or {long, both}                     ");
  blu("  ground_truth   = true  // or {false}                          ");
  blu("  verbose        = true  // or {false}                          ");
  blk("                                                                ");
  blk("  // Configuring visual artifacts                               ");
  blu("  ping_color     = white                                        ");
  blu("  reply_color    = chartreuse                                   ");
  blk("                                                                ");
  blk("  // Configuring Artificial Noise                               ");
  blu("  rn_algorithm   = uniform,pct=0                                ");
  blk("}                                                               ");
  blk("                                                                ");
}
