/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ObstacleManager_Info.cpp                             */
/*    DATE: Aug 27th 2014                                        */
/*****************************************************************/

#include <cstdlib>
#include <iostream>
#include "ObstacleManager_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The pObstacleMgr manages incoming sensor data about obstacles ");
  blk("  and posts alerts suitable for spawning obstacle avoidance     ");
  blk("  behaviors.                                                    ");
  blk("                                                                ");
  blk("                                                                ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: pObstacleMgr file.moos [OPTIONS]                     ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pObstacleMgr with the given process name       ");
  blk("      rather than pObstacleMgr.                             ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pObstacleMgr.          ");
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
  blu("pObstacleMgr Example MOOS Configuration                         ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pObstacleMgr                                    ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  point_var = TRACKED_FEATURE  // default is TRACKED_FEATURE    ");
  blk("                                                                ");
  blk("  max_pts_per_cluster = 20   // default is 20                   ");
  blk("  max_age_per_point   = 20   // (secs)  default is 20           ");
  blk("                                                                ");
  blk("  alert_range  = 20          // (meters) default is 20          ");
  blk("  ignore_range = -1          // (meters) default is -1, (off)   ");
  blk("                                                                ");
  blk("  lasso = true               // default is false                ");
  blk("  lasso_points = 6           // default is 6                    ");
  blk("  lasso_radius = 5           // (meters) default is 5           ");
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
  blu("pObstacleMgr INTERFACE                                          ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  TRACKED_FEATURE = x=5,y=8,label=a,size=4,color=1              ");
  blk("                                                                ");
  blk("  NAV_X = 103.0                                                 ");
  blk("  NAV_Y = -23.8                                                 ");
  blk("  OBM_ALERT_REQUEST = alert_range=25,                           ");
  blk("                      update_var=OBSTACLE_ALERT                 ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  VEHICLE_CONNECT = true                                        ");
  blk("  VIEW_POLYGON    = pts={32,-100:38,-98:40,-100:32,-104},       ");
  blk("                    label=d,edge_color=white,vertex_color=blue  ");
  blk("  OBSTACLE_ALERT  = name=d#                                     ");
  blk("                    poly=pts={32,-100:38,-98:40,-100:32,-104},  ");
  blk("                    label=d                                     ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pObstacleMgr", "gpl");
  exit(0);
}

