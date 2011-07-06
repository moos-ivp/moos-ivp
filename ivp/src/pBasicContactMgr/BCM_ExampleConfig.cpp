/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BCM_ExampleConfig.cpp                                */
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
#include "BCM_ExampleConfig.h"
#include "ColorParse.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showExampleConfig

void showExampleConfig()
{
  blk("                                                                ");
  blk("=============================================================== ");
  blk("pBasicContactMgr Example MOOS Configuration                     ");
  blk("=============================================================== ");
  blu("Blue lines:     ","Default configuration                        ");
  mag("Magenta lines:  ","Non-default configuration                    ");
  blk("                                                                ");
  blk("ProcessConfig = pBasicContactMgr                                ");
  blk("{                                                               ");
  blu("  AppTick   = 4                                                 ");
  blu("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // Alert configurations (one or more)                         ");
  mag("  alert = var=CONTACT_INFO, val=\"name=avd_$[VNAME] # contact=$[VNAME]\"");
  blk("                                                                ");
  blk("  // Properties for all alerts                                  ");
  blu("  alert_range      = 1000     "," // meters.   Range [0,inf)    ");
  blu("  alert_cpa_range  = 1000     "," // meters.   Range [0,inf)    ");
  blu("  alert_cpa_time   = 0        "," // seconds.  Range [0,inf)    ");
  blk("                                                                ");
  blk("  // Policy for retaining potentiall stale contacts             ");
  blu("  contact_max_age  = 3600     "," // seconds.  Range [0,inf)    ");
  blk("                                                                ");
  blk("  // Configuring other output                                   ");
  blu("  display_radii    = false    "," // or {true}                  ");
  blu("  verbose          = true     "," // or {false}                 ");
  blk("}                                                               ");
  blk("                                                                ");
}
