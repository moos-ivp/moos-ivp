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
#include "PNR_ExampleConfig.h"
#include "ColorParse.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showExampleConfig

void showExampleConfig()
{
  blk("                                                                ");
  blk("=============================================================== ");
  blk("pNodeReporter Example MOOS Configuration                        ");
  blk("=============================================================== ");
  blu("Blue lines:     ","Default configuration                        ");
  mag("Magenta lines:  ","Non-default configuration                    ");
  blk("                                                                ");
  blk("ProcessConfig = pNodeReporter                                   ");
  blk("{                                                               ");
  blu("  AppTick   = 4                                                 ");
  blu("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // Configure key aspects of the node                          ");
  mag("  PLATFORM_TYPE      = glider  "," // or {uuv,auv,ship,kayak}   ");
  mag("  PLATFORM_LENGTH    = 8       "," // meters.  Range [0,inf)    ");
  blk("                                                                ");
  blk("  // Configure optional blackout functionality                  ");
  blu("  BLACKOUT_INTERVAL  = 0       "," // seconds. Range [0,inf)    ");
  blk("                                                                ");
  blk("   // Configure the optional platform report summary            ");
  mag("  PLAT_REPORT_INPUT  = COMPASS_HEADING, gap=1                   ");
  mag("  PLAT_REPORT_INPUT  = GPS_SAT, gap=5                           ");
  mag("  PLAT_REPORT_INPUT  = WIFI_QUALITY, gap=1                      ");
  blu("  PLAT_REPORT_OUTPUT = PLATFORM_REPORT_LOCAL                    ");
  blk("                                                                ");
  blk("  // Configure the MOOS variable containg the node report       ");
  blu("  NODE_REPORT_OUTPUT = NODE_REPORT_LOCAL                        ");
  blk("                                                                ");
  blk("  // Threshold for conveying an absense of the helm             ");
  blu("  NOHELM_THRESHOLD   = 5       "," // seconds                   ");
  blk("                                                                ");
  blk("  // Policy for filling in missing lat/lon from x/y or v.versa  ");
  blu("  CROSSFILL_POLICY   = literal "," // or {fill-empty,use-latest}");
  blk("                                                                ");
  blk("  // Configure monitor/reporting of dual nav solution           ");
  mag("  ALT_NAV_PREFIX     = NAV_GT                                   ");
  mag("  ALT_NAV_NAME       = _GT                                      ");
  blk("}                                                               ");
  blk("                                                                ");
}
