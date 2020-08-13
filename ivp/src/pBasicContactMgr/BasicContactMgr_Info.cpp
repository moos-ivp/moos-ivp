/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BasicContactMgr_Info.cpp                             */
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
#include "ReleaseInfo.h"
#include "ColorParse.h"
#include "BasicContactMgr_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The contact manager deals with other known vehicles in its    ");
  blk("  vicinity. It handles incoming reports perhaps received via a  ");
  blk("  sensor application or over a communications link. Minimally   ");
  blk("  it posts summary reports to the MOOSDB, but may also be       ");
  blk("  configured to post alerts with user-configured content about  ");
  blk("  one or more of the contacts.                                  ");
}


//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blk("Usage: pBasicContactMgr file.moos [OPTIONS]                     ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pBasicContactMgr with the given process            ");
  blk("      name rather than pBasicContactMgr.                        ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pBasicContactMgr.          ");
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
  blu("pBasicContactMgr Example MOOS Configuration                     ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pBasicContactMgr                                ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // Alert configurations (one or more, keyed by id)            ");
  blk("  alert = id=avd, var=CONTACT_INFO                              ");
  blk("  alert = id=avd, val=\"name=avd_$[VNAME] # contact=$[VNAME]\"  ");
  blk("  alert = id=avd, range=80, alert_range_color=white             ");
  blk("  alert = id=avd, cpa_range=95, cpa_range_color=gray50          ");
  blk("                                                                ");
  blk("  // Properties for all alerts                                  ");
  blk("  default_alert_range       = 1000   // default in meters       ");
  blk("  default_cpa_range         = 1000   // default in meters       ");
  blk("  default_alert_range_color = color  // default is gray65       ");
  blk("  default_cpa_range_color   = color  // default is gray35       ");
  blk("                                                                ");
  blk("  // Policy for retaining potential stale contacts              ");
  blk("  contact_max_age  = 600             //  default in secs.       ");
  blk("  max_retired_history = 5            //  default in # of ships  ");
  blk("                                                                ");
  blk("  // Configuring other output                                   ");
  blk("  display_radii    = false  // or {true}                        ");
  blk("  alert_verbose    = false  // If true, ALERT_VERBOSE published.");
  blk("                                                                ");
  blk("  // Policy for linear extrapolation of stale contacts          ");
  blk("  decay = 15,30                       // the default in secs    ");
  blk("                                                                ");
  blk("  contacts_recap_interval = 1         // the default in secs    ");
  blk("  range_report_timeout    = 10        // the default in secs    ");
  blk("                                                                ");
  blk("  contact_local_coords    = verbatim  // default is verbatim    ");
  blk("  eval_range_far          = 20        // default in meters      ");
  blk("  eval_range_near         = 10        // default in meters      ");
  blk("  post_closest_range      = false     // default is false       ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blu("=============================================================== ");
  blu("pBasicContactMgr INTERFACE                                      ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  APPCAST_REQ       = node=henry,app=pBasicContactMgr,          ");
  blk("                      duration=6,key=uMAC_438                   ");
  blk("  BCM_ALERT_REQUEST = id=avd, var=CONTACT_INFO,                 ");
  blk("                      val=\"name=avd_$[VNAME] # contact=$[VNAME]\", ");
  blk("  BCM_DISPLAY_RADII = 112.3                                     ");
  blk("                      range=80, alert_range_color=white,        ");
  blk("                      cpa_range=95, cpa_range_color=gray50      ");
  blk("  BCM_REPORT_REQUEST = var=CONTACTS_50, range=50                ");
  blk("  CONTACT_RESOLVED  = henry                                     ");
  blk("  NAV_HEADING       = 118.8                                     ");
  blk("  NAV_X             = 112.3                                     ");
  blk("  NAV_Y             = -99.1                                     ");
  blk("  NAV_SPEED         = 2.0                                       ");
  blk("  NODE_REPORT       = NAME=alpha,TYPE=UUV,TIME=1252348077.59,   ");
  blk("                      X=51.71,Y=-35.50, LAT=43.824981,          ");
  blk("                      LON=-70.329755,SPD=2.0,HDG=118.8,         ");
  blk("                      YAW=118.8,DEPTH=4.6,LENGTH=3.8,           ");
  blk("                      MODE=MODE@ACTIVE:LOITERING                ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  Alert publications configured by the user.                    ");
  blk("                                                                ");
  blk("  APPCAST = name=pBasicContactMgr!@#iter=91!@#node=hotel!@#     ");
  blk("            messages=...                                        ");
  blk("                                                                ");
  blk("  ALERT_VERBOSE = contact=gilda,config_alert_range=40.0,        ");
  blk("                  config_alert_range_cpa=45.0,range_used=39.2,  ");
  blk("                  range_actual=40.8,range_extrap=40.8,          ");
  blk("                  range_cpa=13                                  ");
  blk("                                                                ");
  blk("  CONTACT_CLOSEST      = charlie                                ");
  blk("  CONTACT_CLOSEST_TIME = 17514261063.3                          ");
  blk("  CONTACT_CLOSEST_EVAL = 1                                      ");
  blk("  CONTACT_MGR_WARNING  = Bad Node Report Received               ");
  blk("                                                                ");
  blk("  CONTACTS_ALERTED     = (delta,avd)(charlie,avd)               ");
  blk("  CONTACTS_COUNT       = 2  (number of alerted contacts)        ");
  blk("  CONTACTS_LIST        = delta,gus,charlie,henry                ");
  blk("  CONTACTS_RECAP       = name=delta,age=11.3,range=193.1 #      ");
  blk("                         name=gus,age=0.7,range=48.2 #          ");
  blk("                         name=charlie,age=1.9,range=73.1 #      ");
  blk("                         name=henry,age=4.0,range=18.           ");
  blk("  CONTACTS_RETIRED     = bravo,foxtrot,kilroy                   ");
  blk("  CONTACTS_UNALERTED   = (gus,avd)(henry,avd)                   ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pBasicContactMgr", "gpl");
  exit(0);
}









