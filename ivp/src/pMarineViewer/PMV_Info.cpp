/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PMV_Info.cpp                                         */
/*    DATE: Dec 26th 2011                                        */
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
#include "ColorParse.h"
#include "ReleaseInfo.h"
#include "PMV_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  pMarineViewer is a GUI tool for rendering events in an area of");
  blk("  vehicle operation. It repeatedly updates vehicle positions    ");
  blk("  from incoming node reports, and will render several geometric ");
  blk("  types published from other MOOS apps. The viewer may also post");
  blk("  messages to the MOOSDB based on user-configured keyboard or   ");
  blk("  mouse events.                                                 ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: pMarineViewer file.moos [OPTIONS]                        ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pMarineViewer with the given process name rather   ");
  blk("      than pMarineViewer.                                       ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pMarineViewer.             ");
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
  blu("pMarineViewer Example MOOS Configuration                        ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pMarineViewer                                   ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  TIFF_FILE            = forrest19.tif                          ");
  blk("  set_pan_x            = -90                                    ");
  blk("  set_pan_y            = -280                                   ");
  blk("  zoom                 = 0.65                                   ");
  blk("  vehicle_shape_scale  = 1.35                                   ");
  blk("  seglist_edge_width   = 2.0                                    ");
  blk("  seglist_vertex_size  = 8                                      ");
  blk("  seglist_vertex_color = red                                    ");
  blk("  point_vertex_size    = 5                                      ");
  blk("  hash_delta           = 50                                     ");
  blk("  hash_shade           = 0.4                                    ");
  blk("  hash_view            = true                                   ");
  blk("                                                                ");
  blk("  left_context[survey-point] = SURVEY_UPDATES = points =        ");
  blk("         vname=$(VNAME), x=$(XPOS), y=$(YPOS), format=lawnmower,");
  blk("         label=delta, width=70, height=30, lane_width=8,        ");
  blk("         rows=north-south, degs=80                              ");
  blk("  left_context[survey-point] = SURVEY = true                    ");
  blk("                                                                ");
  blk("  SCOPE = SURVEY_UPDATES                                        ");
  blk("  SCOPE = MVIEWER_LCLICK                                        ");
  blk("  SCOPE = GPS_UPDATE_RECEIVED                                   ");
  blk("  SCOPE = TIME_AT_SURFACE                                       ");
  blk("                                                                ");
  blk("  ACTION = DEPTH_VALUE=\"depth=50\"                             ");
  blk("  ACTION = DEPTH_VALUE=\"depth=25\"                             ");
  blk("  ACTION = DEPTH_VALUE=\"depth=7\"                              ");
  blk("  ACTION = UP_LOITER=\"speed=0\"                                ");
  blk("  ACTION = UP_LOITER=\"speed=1.2\"                              ");
  blk("  ACTION = PSURFACE=true                                        ");
  blk("  ACTION = PSURFACE=false                                       ");
  blk("                                                                ");
  blk("  BUTTON_ONE   = DEPLOY # DEPLOY=true                           ");
  blk("  BUTTON_ONE   = MOOS_MANUAL_OVERIDE=false # RETURN=false       ");
  blk("  BUTTON_TWO   = RETURN # RETURN=true                           ");
  blk("  BUTTON_THREE = SURVEY-true # SURVEY=true                      ");
  blk("  BUTTON_FOUR  = SURVEY-false # SURVEY=false                    ");
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
  blu("pMarineViewer INTERFACE                                         ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  pMarineViewer has a rudimentary scoping capability and the    ");
  blk("  user may specify a list of variables to be scoped.            ");
  blk("  +                                                             ");
  blk("  VIEW_POLYGON                                                  ");
  blk("  VIEW_SEGLIST                                                  ");
  blk("  VIEW_POINT                                                    ");
  blk("  VIEW_VECTOR                                                   ");
  blk("  VIEW_MARKER                                                   ");
  blk("  VIEW_CIRCLE                                                   ");
  blk("  VIEW_GRID                                                     ");
  blk("  VIEW_RANGE_PULSE                                              ");
  blk("  VIEW_COMMS_PULSE                                              ");
  blk("  NODE_REPORT                                                   ");
  blk("  NODE_REPORT_LOCAL                                             ");
  blk("  GRID_CONFIG                                                   ");
  blk("  GRID_DELTA                                                    ");
  blk("  BEARING_LINE                                                  ");
  blk("  TRAIL_RESET                                                   ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  Any variable specified in the moos configuration block using  ");
  blk("  the connection_posting parameter.                             ");
  blk("  Any variable specified in the moos configuration block using  ");
  blk("  the button or action pull-down menu parameters.               ");
  blk("  Any variable specified in the moos configuration block using  ");
  blk("  the mouse left_context or right_context parameters.           ");
  blk("  +                                                             ");
  blk("  HELM_MAP_CLEAR                                                ");
  blk("  MVIEWER_LCLICK                                                ");
  blk("  MVIEWER_RCLICK                                                ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pMarineViewer", "gpl");
  exit(0);
}

