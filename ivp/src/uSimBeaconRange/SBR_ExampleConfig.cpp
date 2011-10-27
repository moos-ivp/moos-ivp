/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SBR_ExampleConfig.cpp                                */
/*    DATE: July 5th 2011                                        */
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
#include "SBR_ExampleConfig.h"
#include "ColorParse.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showExampleConfig

void showExampleConfig()
{
  blk("=============================================================== ");
  blk("uSimBeaconRange Example MOOS Configuration                      ");
  blk("=============================================================== ");
  blu("Blue lines:     ","Default configuration                        ");
  mag("Magenta lines:  ","Non-default configuration                    ");
  blk("                                                                ");
  blk("{                                                               ");
  blu("  AppTick   = 4                                                 ");
  blu("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // Configuring aspects of vehicles in the sim                 ");
  blu("  reach_distance = default = 200  "," // or {nolimit}           ");
  mag("  reach_distance = henry = 40     "," // meters                 ");
  blu("  ping_wait      = default = 30   "," // seconds                ");
  mag("  ping_wait      = henry   = 120                                ");
  blu("  ping_payments  = upon_response  "," // or {upon_receipt, upon_request}");
  blk("                                                                ");
  blk("  // Configuring manner of reporting                            ");
  blu("  report_vars    = short  ","// or {long, both}                 ");
  blu("  ground_truth   = true   ","// or {false}                      ");
  blu("  verbose        = true   ","// or {false}                      ");
  blk("                                                                ");
  blk("  // Configuring default beacon properties                      ");
  blu("  default_beacon_shape = circle    ","// or {square, diamond, etc.}  ");
  blu("  default beacon_color = orange    ","// or {red, green, etc.}       ");
  blu("  default_beacon_width = 4                                      ");
  blu("  default_beacon_report_range = 100                             ");
  blu("  default_beacon_freq  = never     ","// or [0,inf]             ");
  blk("                                                                ");
  blk("  // Configuring Beacon properties                              ");
  mag("  beacon = x=200, y=435, label=01, report_range=45              ");
  mag("  beacon = x=690, y=205, label=02, freq=90                      ");
  mag("  beacon = x=350, y=705, label=03, width=8, color=blue          ");
  blk("                                                                ");
  blk("  // Configuring Artificial Noise                               ");
  blu("  rn_algorithm  =  uniform,pct=0  "," // pct may be in [0,1]    ");
  blk("}                                                               ");
  blk("                                                                ");
}

