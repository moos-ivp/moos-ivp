/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: FldNodeComms_Info.cpp                                */
/*    DATE: Dec 4th 2011                                         */
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
#include "FldNodeComms_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  A shoreside tool for managing communications between vehicles.");
  blk("  It has knowledge of all vehicle positions based on incoming   ");
  blk("  node reports. Communications may be  limited based on vehicle ");
  blk("  range, frequency of messages, or size of message. Messages may");
  blk("  also be blocked based on a team affiliation.                  ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blk("Usage: uFldNodeComms file.moos [OPTIONS]                        ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uFldNodeComms with the given process               ");
  blk("      name rather than uFldNodeComms.                           ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uFldNodeComms.             ");
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
  blu("uFldNodeComms Example MOOS Configuration                        ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldNodeComms                                   ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  comms_range    = 100      // default (in meters)              ");
  blk("  critical_range = 30       // default (in meters)              ");
  blk("  stale_time     = 5        // default (in seconds)             ");
  blk("                                                                ");
  blk("  min_msg_interval = 30     // default (in seconds)             ");
  blk("  max_msg_length   = 1000   // default (# of characters)        ");
  blk("                                                                ");
  blk("  view_node_rpt_pulses = true // default                        ");
  blk("                                                                ");
  blk("  verbose  = true           // default                          ");
  blk("                                                                ");
  blk("  stealth  = vname=alpha, stealth=0.8                           ");
  blk("  earange  = vname=alpha, earange=4.5                           ");
  blk("                                                                ");
  blk("  groups   = true                                               ");
  blk("}                                                               ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uFldNodeComms INTERFACE                                         ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  NODE_MESSAGE      = src_node=henry,dest_node=ike,var_name=FOO,");
  blk("                      string_val=bar                            ");
  blk("  NODE_REPORT                                                   ");
  blk("  NODE_REPORT_LOCAL = NAME=alpha,TYPE=UUV,TIME=1252348077.59,   ");
  blk("                      X=51.71,Y=-35.50, LAT=43.824981,          ");
  blk("                      LON=-70.329755,SPD=2.0,HDG=118.8,         ");
  blk("                      YAW=118.8,DEPTH=4.6,LENGTH=3.8,           ");
  blk("                      MODE=MODE@ACTIVE:LOITERING                ");
  blk("                                                                ");
  blk("  UNC_VIEW_NODE_RPT_PULSES = false                              ");
  blk("  UNC_STEALTH = vname=alpha,stealth=0.4                         ");
  blk("  UNC_EARANGE = vname=alpha,earange=0.5                         ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  NODE_MESSAGE_<VNAME> = src_node=henry,dest_node=ike,          ");
  blk("                         var_name=FOO,string_val=bar            ");
  blk("  NODE_REPORT_<VNAME>  = NAME=alpha,TYPE=UUV,TIME=1252348077.5, ");
  blk("                         X=51.71,Y=-35.50, LAT=43.824981,       ");
  blk("                         LON=-70.329755,SPD=2.0,HDG=118.8,      ");
  blk("                         YAW=118.8,DEPTH=4.6,LENGTH=3.8,        ");
  blk("                         MODE=MODE@ACTIVE:LOITERING             ");
  blk("  VIEW_COMMS_PULSE     = label=one,sx=4,sy=2,tx=44,ty=55,       ");
  blk("                         beam_width=10,duration=5,fill=0.3,     ");
  blk("                         fill_color=yellow,edge_color=green     ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldNodeComms", "gpl");
  exit(0);
}









