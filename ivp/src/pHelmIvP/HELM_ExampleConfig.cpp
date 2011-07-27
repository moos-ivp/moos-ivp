/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HELM_ExampleConfig.cpp                               */
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
#include "HELM_ExampleConfig.h"
#include "ColorParse.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showExampleConfig

void showExampleConfig()
{
  blk("                                                                ");
  blk("=============================================================== ");
  blk("pHelmIvP Example MOOS Configuration                             ");
  blk("=============================================================== ");
  blu("Blue lines:     ","Default configuration                        ");
  mag("Magenta lines:  ","Additional configuration                     ");
  blk("                                                                ");
  blk("ProcessConfig = pHelmIvP                                        ");
  blk("{                                                               ");
  blu("  AppTick   = 4                                                 ");
  blu("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // Configuring behavior file and decision domain              ");
  mag("  behaviors   =  filename.bhv                                   ");
  mag("  domain      =  course:0:359:360                               ");
  mag("  domain      =  speed:0:2:21                                   ");
  mag("  domain      =  depth:0:1000:1001                              ");
  blk("                                                                ");
  blk("  // Configure Helm Engagement policies                         ");
  blu("  start_engaged        = false     // or {true}                 ");
  blu("  allow_disengaged     = true      // or {false}                ");
  blu("  disengage_on_allstop = false     // or {true}                 ");
  blk("                                                                ");
  blk("  // Provide alternative to MOOS_MANUAL_OVERRIDE directive      ");
  mag("  other_override_var   = AUTONOMY_OVERRIDE                      ");
  blk("                                                                ");
  blk("  // Handle possible time skews from external communities       ");
  mag("  node_skew            = charlie,45  ","// vehicle_name, secs   ");
  blu("  ok_skew              = 60          ","// or {any}             ");
  blk("                                                                ");
  blk("  // Configure the verbosity of terminal output.                ");
  blu("  verbose              = terse  "," // or {true,false,quiet}    ");
  blk("}                                                               ");
  blk("                                                                ");
}
