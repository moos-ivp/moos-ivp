/*****************************************************************/
/*    NAME: Kyle Woerner, Michael Benjamin                       */
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
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The uCollisionDetector application is run by the shoreside   ");
  blk("  and detects if two vehicles have collided.  A collision is    ");
  blk("  defined as being within the distance specified by the         ");
  blk("  COLLISIONRANGE input parameter.  The default value is 5.      ");
  blk("                                                                ");
  blk("  The AppCast displays the detection range and minimum range    ");
  blk("  of each vehicle pair that is colliding.  The display of       ");
  blk("  collisions can be delayed from clearing in the AppCast using the             ");
  blk("  DELAYTIMETOCLEAR parameter to allow the user sufficient time  ");
  blk("  to view the information.                                      ");
  blk("                                                                ");
  blk("  A variable \"VEHICLE_COLLISION_$V1_$V2\" is published for     ");
  blk("  each collision with the initial detection distance.  This     ");
  blk("  is published once per collision.  An update is published      ");
  blk("  with \"false\" when a collision condition clears.  This       ");
  blk("  publication can be delayed to correspond with the clearing    ");
  blk("  of the AppCast display using the parameter PUBLISHIMMEDIATELY.");
  blk("                                                                ");
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
  blu("uCollisionDetector Example MOOS Configuration                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldCollisionDetect                             ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // range considered a collision                               ");
  blk("  collision_range     = 10                                      ");
  blk("                                                                ");
  blk("  // Delay time, in seconds, from collision condition clearing  ");
  blk("  // to removal from AppCasting                                 ");
  blk("  delay_time_to_clear = 35                                      ");
  blk("                                                                ");
  blk("  // Publish the cleared variable to MOOSDB immediately after   ");
  blk("  // collision condition clears.                                ");
  blk("  publish_immediately = true                                    ");
  blk("                                                                ");
  blk("  pulse = true                                                  ");
  blk("  pulse_range = 50                                              ");  
  blk("  pulse_duration = 15                                           ");  
  blk("                                                                ");  
  blk("  publish_pairs  = false  // default is false                   ");
  blk("  publish_single = true   // default is true                    ");
  blk("}                                                               ");

exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blu("=============================================================== ");
  blu("uCollisionDetector INTERFACE                                    ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  NODE_REPORT    =    NAME=alpha,TYPE=UUV,TIME=1252348077.59,   ");
  blk("                      X=51.71,Y=-35.50, LAT=43.824981,          ");
  blk("                      LON=-70.329755,SPD=2.0,HDG=118.8,         ");
  blk("                      YAW=118.8,DEPTH=4.6,LENGTH=3.8,           ");
  blk("                      MODE=MODE@ACTIVE:LOITERING                ");
  blk("                                                 ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  VEHICLE_COLLISION_$V1_$V2 = detection_distance                ");
  blk("    (when new collision detected)   ");
  blk("    (published only once per collision)                        ");
  blk("                                                               ");
  blk("  VEHICLE_COLLISION_$V1_$V2 = false                            ");
  blk("    (when existing collision clears ");
  blk("    (published only once per collision                          ");
  blk("                                                                ");
  blk("  where $V1 and $V2 are vehicle names.                          ");
  blk("                                                                ");
  blk("  VEHICLE_COLLISION=\"v1=archie,v2=betty,detection_distance=4.9\" ");
  blk("                                                                ");
  blk("                                                                ");

  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uCollisionDetector", "gpl");
  exit(0);
}



