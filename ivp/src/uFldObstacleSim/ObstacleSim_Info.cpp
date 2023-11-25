/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ObstacleSim_Info.cpp                                 */
/*    DATE: October 19th, 2017                                   */
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
#include "ObstacleSim_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The uFldObstacleSim app will simulate the posting of obstacles");
  blk("  loaded from a text file, to be shared to all vehicles in the  ");
  blk("  uField environment.                                           ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: uFldObstacleSim file.moos [OPTIONS]                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uFldObstacleSim with the given process name        ");
  blk("      rather than uFldObstacleSim.                              ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uFldObstacleSim.           ");
  mag("  --web,-w                                                      ");
  blk("      Open browser to: https://oceanai.mit.edu/ivpman/apps/uFldObstacleSim ");
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
  blu("uFldObstacleSim Example MOOS Configuration                      ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldObstacleSim                                 ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  obstacle_file    = obstacles.txt                              ");
  blk("  poly_vert_color  = color    (default is gray50)               ");
  blk("  poly_edge_color  = color    (default is gray50)               ");
  blk("  poly_fill_color  = color    (default is white)                ");
  blk("  poly_label_color = color    (default is invisible)            ");
  blk("                                                                ");
  blk("  poly_vert_size    = 1       (default is 1)                    ");
  blk("  poly_edge_size    = 1       (default is 1)                    ");
  blk("  poly_transparency = 0.15    (default is 0.15)                 ");
  blk("                                                                ");
  blk("  region_edge_color = color   (default is gray50)               ");
  blk("  region_vert_color = color   (default is white )               ");
  blk("                                                                ");
  blk("  draw_region       = true    (default is true)                 ");
  blk("  region_edge_color = color   (default is gray50)               ");
  blk("  region_vert_color = color   (default is white)                ");
  blk("                                                                ");
  blk("  post_points      = true     (default is false)                ");
  blk("  rate_points      = 5        (default is 5)                    ");
  blk("  point_size       = 5        (default is 2)                    ");
  blk("                                                                ");
  blk("  min_duration     = 10       (default is -1)                   ");
  blk("  max_duration     = 15       (default is -1)                   ");
  blk("  refresh_interval = 8        (default is -1)                   ");
  blk("                                                                ");
  blk("  reset_interval   = -1       (default is -1)                   ");
  blk("  reset_range      = 10       (default is 10)                   ");
  blk("                                                                ");
  blk("  reuse_ids        = true     (default is true)                 ");
  blk("  sensor_range     = 50       (default is 50)                   ");
  blk("                                                                ");
  blk("  app_logging  = true  // {true or file} By default disabled    ");
  blk("                                                                ");
  blk("  post_visuals = true  // {true or false} By default true       ");
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
  blu("uFldObstacleSim INTERFACE                                       ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  PMV_CONNECT      = true                                       ");
  blk("  VEHICLE_CONNECT  = true                                       ");
  blk("  NODE_REPORT                                                   ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  VIEW_POLYGON                                                  ");
  blk("  KNOWN_OBSTACLE                                                ");
  blk("  GIVEN_OBSTACLE                                                ");
  blk("  TRACKED_FEATURE                                               ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldObstacleSim", "gpl");
  exit(0);
}


