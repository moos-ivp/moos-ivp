/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: TS_ExampleConfig.cpp                                 */
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
#include "TS_ExampleConfig.h"
#include "ColorParse.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showExampleConfig

void showExampleConfig()
{
  blk("                                                                ");
  blk("=============================================================== ");
  blk("uTimerScript Example MOOS Configuration                           ");
  blk("=============================================================== ");
  blu("Blue lines:     ","Default configuration                        ");
  mag("Magenta lines:  ","Non-default configuration                    ");
  blk("                                                                ");
  blk("ProcessConfig = uTimerScript                                    ");
  blk("{                                                               ");
  blu("  AppTick   = 4                                                 ");
  blu("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // Logic condition that must be met for script to be unpaused ");
  mag("  condition      = WIND_GUSTS = true                            ");
  blk("  // Seconds added to each event time, on each script pass      ");
  blu("  delay_reset    = 0                                            ");
  blk("  // Seconds added to each event time, on first pass only       ");
  blu("  delay_start    = 0                                            ");
  blk("  // Event(s) are the key components of the script              ");
  mag("  event          = var=SBR_RANGE_REQUEST, val=\"name=archie\", time=25:35  ");
  blk("  // A MOOS variable for taking cues to forward time            ");
  blu("  forward_var    = UTS_FORWARD  "," // or other MOOS variable   ");
  blk("  // If true script is paused upon launch                       ");
  blu("  paused         = false   "," // or {true}                     ");
  blk("  // A MOOS variable for receiving pause state cues             ");
  blu("  pause_var      = UTS_PAUSE    "," // or other MOOS variable   ");
  blk("  // Declaration of random var macro expanded in event values   ");
  mag("  randvar        = varname=ANG, min=0, max=359, key=at_reset    ");
  blk("  // Maximum number of resets allowed                           ");
  blu("  reset_max      = nolimit "," // or in range [0,inf)           ");
  blk("  // A point when the script is reset                           ");
  blu("  reset_time     = none    "," // or {all-posted} or range (0,inf) ");
  blk("  // A MOOS variable for receiving reset cues                   ");
  blu("  reset_var      = UTS_RESET    "," // or other MOOS variable   ");
  blk("  // If true script will complete if conditions suddenly fail   ");
  blu("  script_atomic  = false   "," // or {true}                     ");
  blk("  // A hopefully unique name given to the script                ");
  blu("  script_name    = unnamed                                      ");
  blk("  // If true timestamps are recalculated on each script reset   ");
  blu("  shuffle        = true                                         ");
  blk("  // If true progress is generated to the console               ");
  blu("  verbose        = true    "," // or {false}                    ");
  blk("  // Reset or restart script upon conditions being met after failure ");
  blu("  upon_awake     = n/a     "," // or {reset,resstart}           ");
  blk("  // A MOOS variable for posting the status summary             ");
  blu("  status_var     = UTS_STATUS   "," // or other MOOS variable   ");
  blk("  // Rate at which time is accelerated in execuing the script   ");
  blu("  time_warp      = 1                                            ");
  blk("}                                                               ");
  blk("                                                                ");
}
