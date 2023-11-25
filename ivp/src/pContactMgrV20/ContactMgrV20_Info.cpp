/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ContactMgrV20_Info.cpp                               */
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
#include "ContactMgrV20_Info.h"

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
  blk("Usage: pContactMgrV20 file.moos [OPTIONS]                       ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pContactMgrV20 with the given process              ");
  blk("      name rather than pContactMgrV20.                          ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pContactMgrV20.            ");
  mag("  --web,-w                                                      ");
  blk("      Open web browser to:                                      ");
  blk("      https://oceanai.mit.edu/ivpman/apps/pContactMgrV20        ");
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
  blu("pContactMgrV20 Example MOOS Configuration                       ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pContactMgrV20                                  ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // Alert configurations (one or more, keyed by id)            ");
  blk("  alert = id=say, on_flag=SAY_MOOS=hello                        ");
  blk("  alert = id=say, off_flag=SAY_MOOS=bye                         ");
  blk("  alert = id=say, alert_range=80, cpa_range=100                 ");
  blk("                                                                ");
  blk("  // Type and group info about ownship                          ");
  blk("  ownship_group = blue_team                                     ");
  blk("  ownship_type  = kayak                                         ");
  blk("                                                                ");
  blk("  // Global alert filters (providing nothing is fine)           ");
  blk("  ignore_name = henry,gilda                                     ");
  blk("  match_name  = abe, cal                                        ");
  blk("                                                                ");
  blk("  ignore_group = alpha                                          ");
  blk("  match_group  = bravo,gamma                                    ");
  blk("                                                                ");
  blk("  ignore_type = usv,uuv                                         ");
  blk("  match_type  = ship                                            ");
  blk("                                                                ");
  blk("  ignore_region = pts={60,-40:60,-160:150,-160:150,-40}         ");
  blk("  match_region  = pts={60,70:60,160:150,160:150,70}             ");
  blk("                                                                ");
  blk("  strict_ignore = true             // Default is true           ");
  blk("                                                                ");
  blk("  alert_range_color = color        // default is gray65         ");
  blk("  cpa_range_color   = color        // default is gray35         ");
  blk("                                                                ");
  blk("  // Policy for retaining potential stale contacts              ");
  blk("  contact_max_age  = 60            //  Default is 60 secs.      ");
  blk("  max_retired_history = 5          //  Default is 5 contacts    ");
  blk("                                                                ");
  blk("  // Configuring other output                                   ");
  blk("  display_radii    = false         // Default is false          ");
  blk("  display_radii_id = say           // Default is first known    ");
  blk("  alert_verbose    = false         // Default is false          ");
  blk("                                                                ");
  blk("  // Policy for linear extrapolation of stale contacts          ");
  blk("  decay = 15,30                    // Default is 15,30 secs     ");
  blk("                                                                ");
  blk("  recap_interval       = 1         // Default is 1 sec          ");
  blk("  range_report_timeout = 10        // Default is 10 secs        ");
  blk("  range_report_maxsize = 20        // Default is 20 reports     ");
  blk("                                                                ");
  blk("  contact_local_coords = verbatim  // Default is verbatim       ");
  blk("  post_closest_range   = false     // Default is false          ");
  blk("  post_closest_relbng  = false     // Default is false          ");
  blk("  post_all_ranges = false          // Default is false          ");
  blk("                                                                ");
  blk("  reject_range = 2000              // Default is 2000 meters    ");
  blk("  max_contacts = 500               // Default is 500            ");
  blk("  alert_verbose = false            // Default is false          ");
  blk("                                                                ");
  blk("  app_logging = true               // Default is false           ");
  blk("                                                                ");
  blk("  hold_alerts_for_helm = true      // Default is false          ");
  blk("}                                                               ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blu("=============================================================== ");
  blu("pContactMgrV20 INTERFACE                                        ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  APPCAST_REQ       = node=henry,app=pContactMgrV20,            ");
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
  blk("  IVPHELM_STATE     = DRIVE                                     ");
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
  blk("  APPCAST = name=pContactMgrV20!@#iter=91!@#node=hotel!@#       ");
  blk("            messages=...                                        ");
  blk("                                                                ");
  blk("  ALERT_VERBOSE = contact=gilda,config_alert_range=40.0,        ");
  blk("                  config_alert_range_cpa=45.0,range_used=39.2,  ");
  blk("                  range_actual=40.8,range_extrap=40.8,          ");
  blk("                  range_cpa=13                                  ");
  blk("                                                                ");
  blk("  CONTACT_CLOSEST        = charlie                              ");
  blk("  CONTACT_CLOSEST_TIME   = 17514261063.3                        ");
  blk("  CONTACT_CLOSEST_RANGE  = 23.1              (if enabled)       ");
  blk("  CONTACT_CLOSEST_RELBNG = 344              (if enabled)        ");
  blk("  CONTACT_RANGES         = 23.1,63.2,77.5    (if enabled)       ");
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
  blk("                                                                ");
  blk("  VIEW_CIRCLE         = x=-150.3,y=-117.5,radius=10,edge_size=1 ");
  blk("                                                                ");
  blk("  PCONTACTMGRV20_PID   = 3988                                   ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pContactMgrV20", "gpl");
  exit(0);
}










