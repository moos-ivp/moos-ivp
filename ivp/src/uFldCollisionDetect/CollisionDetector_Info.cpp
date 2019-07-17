/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CollisionDetector_Info.cpp                           */
/*    DATE: Dec 29th 1963                                        */
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
#include "CollisionDetector_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                      ");
  blk("------------------------------------                           ");
  blk("  The uCollisionDetect application is run by the shoreside     ");
  blk("  and detects if two vehicles have had an ecounter.  An        ");
  blk("  encounter is defined as being within the distance specified  ");
  blk("  by the encounter_range parameter.                            ");
  blk("                                                               ");
  blk("  An encounter constituting a near miss or collision will also ");
  blk("  produce a posting to UCD_REPORT indicating the two vehicle   ");
  blk("  names, the CPA distance, and rank (near miss or collision).  ");
  blk("                                                               ");
  blk("  Flags may be configured to be posted upon each event type -  ");
  blk("  collision, near-miss or encounter. These flags are simply    ");
  blk("  MOOS variable and value pairs like the flags in many other   ");
  blk("  MOOS applications and helm behaviors. For example, such a    ");
  blk("  flag may be used to trigger an evaluation of the mission     ");
  blk("  efficiency for a window of time around the encounter.        ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: uCollisionDetector file.moos [OPTIONS]                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uCollisionDetector with the given process name     ");
  blk("      rather than uCollisionDetector.                           ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uCollisionDetector.        ");
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
  blu("=============================================================== ");
  blu("uFldCollisionDetect Example MOOS Configuration                  ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldCollisionDetect                             ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  condition = DEPLOY_ALL = true                                 ");
  blk("                                                                ");
  blk("  encounter_flag = ENCOUNTER = $CPA                             ");
  blk("  collision_flag = COLLISION = $CPA                             ");
  blk("  near_miss_flag = NEAR_MISS = vname1=$V1,vname2=$V2,cpa=$CPA   ");
  blk("                                                                ");
  blk("  encounter_range = 10        // (the default) in meters        ");
  blk("  near_miss_range = 6         // (the default) in meters        ");
  blk("  collision_range = 3         // (the default) in meters        ");
  blk("                                                                ");
  blk("  pulse_render   = true       // (the default)                  ");
  blk("  pulse_range    = 20         // (the default) in meters        ");  
  blk("  pulse_duration = 10         // (the default) in seconds       ");  
  blk("}                                                               ");  

exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blu("=============================================================== ");
  blu("uFldCollisionDetect INTERFACE                                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  Dynamic: Will subscribe for any MOOS variables involved in the");
  blk("           condition parameter if used.                         ");
  blk("                                                                ");
  blk("                                                                ");
  blk("  NODE_REPORT = NAME=alpha,TYPE=UUV,TIME=1252348077.59,X=51.71, ");
  blk("                Y=-35.50, LAT=43.824981,LON=-70.329755,SPD=2.0, ");
  blk("                HDG=118.8,YAW=118.8,DEPTH=4.6,LENGTH=3.8,       ");
  blk("                MODE=MODE@ACTIVE:LOITERING                      ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  Dynamic: Will pulish any MOOS variables involved in either    ");
  blk("           the collision_flag, near_miss_flag or encounter_flag ");
  blk("           parameters.                                          ");
  blk("                                                                ");
  blk("  UCD_REPORT = vname1=abe, vname2=cal, cpa=2.4, rank=collision  ");
  blk("                                                                ");
  blk("  VIEW_RANGE_PULSE = x=78.78,y=-78.405,radius=40,duration=20,   ");
  blk("                     fill=0.25,edge_color=red,fill_color=red,   ");
  blk("                     time=65286252381.7,edge_size=1             ");
  blk("                                                                ");
  blk("  ENCOUNTER_TOTAL  = 193                                        ");
  blk("                                                                ");
  blk("  COLLISION_DETECT_PARAMS = collision_range=4,near_miss_range=8,");
  blk("                            encounter_range=20                  ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uCollisionDetector", "gpl");
  exit(0);
}







