/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BasicContactMgr_Info.cpp                             */
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
  mag("  --verbose=<Boolean>                                           ");
  blk("      Display status updates and diagnostics if true.           ");
  blk("      The default is true.                                      ");
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
  blk("  // Alert configurations (one or more)                         ");
  blk("  alert = var=CONTACT_INFO, val=\"name=avd_$[VNAME] # contact=$[VNAME]\"");
  blk("                                                                ");
  blk("  // Properties for all alerts                                  ");
  blk("  alert_range      = 1000     "," // meters.   Range [0,inf)    ");
  blk("  alert_cpa_range  = 1000     "," // meters.   Range [0,inf)    ");
  blk("  alert_cpa_time   = 0        "," // seconds.  Range [0,inf)    ");
  blk("                                                                ");
  blk("  // Policy for retaining potentiall stale contacts             ");
  blk("  contact_max_age  = 3600     "," // seconds.  Range [0,inf)    ");
  blk("                                                                ");
  blk("  // Configuring other output                                   ");
  blk("  display_radii    = false    "," // or {true}                  ");
  blk("  verbose          = true     "," // or {false}                 ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("pBasicContactMgr INTERFACE                                      ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  NAV_X            = 112.3                                      ");
  blk("  NAV_Y            = -99.1                                      ");
  blk("  NAV_LAT          = 43.824981                                  ");
  blk("  NAV_LONG         = -70.329755                                 ");
  blk("  NAV_HEADING      = 118.8                                      ");
  blk("  NAV_SPEED        = 2.0                                        ");
  blk("  NAV_YAW          = 118.8                                      ");
  blk("  NAV_DEPTH        = 4.6                                        ");
  blk("  CONTACT_RESOLVED = henry                                      ");
  blk("  NODE_REPORT      = NAME=alpha,TYPE=UUV,TIME=1252348077.59,    ");
  blk("                     X=51.71,Y=-35.50, LAT=43.824981,           ");
  blk("                     LON=-70.329755,SPD=2.0,HDG=118.8,          ");
  blk("                     YAW=118.8,DEPTH=4.6,LENGTH=3.8,            ");
  blk("                     MODE=MODE@ACTIVE:LOITERING                 ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  Alert publications configured by the user.                    ");
  blk("                                                                ");
  blk("  CONTACTS_LIST      = delta,gus,charlie,henry                  ");
  blk("  CONTACTS_ALERTED   = delta,charlie                            ");
  blk("  CONTACTS_UNALERTED = gus,henry                                ");
  blk("  CONTACTS_RETIRED   = bravo,foxtrot,kilroy                     ");
  blk("  CONTACTS_RECAP     = name=delta,age=11.3,range=193.1 #        ");
  blk("                       name=gus,age=0.7,range=48.2 #            ");
  blk("                       name=charlie,age=1.9,range=73.1 #        ");
  blk("                       name=henry,age=4.0,range=18.             ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pBasicContactMgr", "gpl");
  exit(0);
}

