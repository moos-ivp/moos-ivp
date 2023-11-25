/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PMV_Info.cpp                                         */
/*    DATE: Dec 26th 2011                                        */
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
  mag("  --verbose                                                     ");
  blk("      Increase verbostiy.                                       ");
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
  blk("  // BackView Options ==========================================");
  blk("  tiff_file            = forrest19.tif                          ");
  blk("  set_pan_x            = -90                                    ");
  blk("  set_pan_y            = -280                                   ");
  blk("  zoom                 = 0.65                                   ");
  blk("  vehicle_shape_scale  = 1.35                                   ");
  blk("                                                                ");
  blk("  // BackView Pull-Down Menu ===================================");
  blk("  hash_delta           = 50    // {10,50,200,500}               ");
  blk("  hash_shade           = 0.4   // [0.0, 1.0]                    ");
  blk("  hash_viewable        = true  // {TRUE, false}                 ");
  blk("  full_screen          = false // {true, FALSE}                 ");
  blk("                                                                ");
  blk("  // GeoAttributes Pull-Down Menu ==============================");
  blk("  polygon_viewable_all     = true  // {TRUE, false}             ");
  blk("  polygon_viewable_labels  = true  // {TRUE, false}             ");
  blk("  seglist_viewable_all     = true  // {TRUE, false}             ");
  blk("  seglist_viewable_labels  = true  // {TRUE, false}             ");
  blk("  point_viewable_all       = true  // {TRUE, false}             ");
  blk("  point_viewable_labels    = true  // {TRUE, false}             ");
  blk("  vector_viewable_all      = true  // {TRUE, false}             ");
  blk("  vector_viewable_labels   = true  // {TRUE, false}             ");
  blk("  circle_viewable_all      = true  // {TRUE, false}             ");
  blk("  circle_viewable_labels   = true  // {TRUE, false}             ");
  blk("  grid_viewable_all        = true  // {TRUE, false}             ");
  blk("  grid_viewable_labels     = true  // {TRUE, false}             ");
  blk("  grid_opaqueness          = 0.3   // {0.1, 0.2, ..., 1.0}      ");
  blk("  datum_viewable_all       = true  // {TRUE, false}             ");
  blk("  datum_viewable_labels    = true  // {TRUE, false}             ");
  blk("  marker_viewable_all      = true  // {TRUE, false}             ");
  blk("  marker_viewable_labels   = true  // {TRUE, false}             ");
  blk("  oparea_viewable_all      = true  // {TRUE, false}             ");
  blk("  oparea_viewable_labels   = true  // {TRUE, false}             ");
  blk("  drop_point_viewable_all  = true  // {TRUE, false}             ");
  blk("  drop_point_coords        = as-dropped  // {lat-lon,local-grid}");
  blk("  drop_point_vertex_size   = true  // {1, 2, 3, ..., 10}        ");
  blk("  comms_pulse_viewable_all = true  // {TRUE, false}             ");
  blk("  range_pulse_viewable_all = true  // {TRUE, false}             ");
  blk("                                                                ");
  blk("  // Vehicles Pull-Down Menu ===================================");
  blk("  vehicles_viewable       = true   // {TRUE, false}             ");
  blk("  vehicles_name_mode      = names  // {names, names+depth, off  ");
  blk("                                       names+mode, names+auxmode");
  blk("                                       names+shortmode}         ");
  blk("  stale_report_thresh     = 30     // [5, inf)                  ");
  blk("  stale_remove_thresh     = 30     // [0, inf)                  ");
  blk("  vehicles_active_color   = red    // {yellow,white,blue,green} ");
  blk("  vehicles_inactive_color = yellow // {red,white,blue,green}    ");
  blk("  trails_viewable         = true   // {TRUE, false}             ");
  blk("  trails_color            = white  // {WHITE, yellow, green}    ");
  blk("  bearing_lines           = true   // {TRUE, false}             ");
  blk("                                                                ");
  blk("  // InfoCasting Pull-Down Menu                                 ");
  blk("  infocast_viewable    = true     // {TRUE, false}              ");
  blk("  content_mode         = appcast  // {APPCAST, realmcast        ");
  blk("  refresh_mode         = events   // {paused, streaming}        ");
  blk("  nodes_font_size      = large    // {xlarge, large, medium, small, xsmall}");
  blk("  procs_font_size      = large    // {xlarge, large, medium, small, xsmall}");
  blk("  infocast_font_size   = medium   // {xlarge, large, medium, small, xsmall}");
  blk("  appcast_color_scheme = indigo   // {default, indigo, beige, white} ");
  blk("  realmcast_color_scheme = hillside // {default, indigo, beige, white, hillside}");
  blk("  realmcast_channel    = abe:pHelmIvP // node:channel           ");
  blk("  infocast_width        = 40      // {20, 25, 30, ..., 65, 70}  ");
  blk("  infocast_height       = 70      // {30, 35, 40, ..., 85, 90}  ");
  blk("                                                                ");
  blk("  realmcast_show_source        = true  // {TRUE,false}          ");
  blk("  realmcast_show_community     = true  // {TRUE,false}          ");
  blk("  realmcast_show_subscriptions = true  // {TRUE,false}          ");
  blk("  realmcast_show_masked        = true  // {TRUE,false}          ");
  blk("  realmcast_wrap_content       = false // {true,FALSE}          ");
  blk("  realmcast_trunc_content      = false // {true,FALSE}          ");
  blk("  realmcast_time_format_utc    = false // {true,FALSE}          ");
  blk("                                                                ");
  blk("  watch_cluster = key=mission_top, vars=DEPLOY:RETURN:COVER     ");
  blk("                                                                ");
  blk("  // Context Pull-Down Menu ====================================");
  blk("  left_context[survey-point] = SURVEY_UPDATES = points =        ");
  blk("         vname=$(VNAME), x=$(XPOS), y=$(YPOS), format=lawnmower,");
  blk("         label=delta, width=70, height=30, lane_width=8,        ");
  blk("         rows=north-south, degs=80                              ");
  blk("  left_context[survey-point] = SURVEY = true                    ");
  blk("                                                                ");
  blk("  // Exclusion Filter ========================================= ");
  blk("  ignore_name = abe                                             ");
  blk("  match_name = ben,cal                                          ");
  blk("                                                                ");
  blk("  // MOOS-Scope Pull-Down Menu ================================ ");
  blk("  scope = SURVEY_UPDATES                                        ");
  blk("  scope = TIME_AT_SURFACE                                       ");
  blk("                                                                ");
  blk("  // Action Pull-Down Menu                                      ");
  blk("  action = DEPTH_VALUE=\"depth=50\"                             ");
  blk("  action = DEPTH_VALUE=\"depth=25\"                             ");
  blk("  action = DEPTH_VALUE=\"depth=7\"                              ");
  blk("  action = UP_LOITER=\"speed=0\"                                ");
  blk("  action = UP_LOITER=\"speed=1.2\"                              ");
  blk("                                                                ");
  blk("  // Button Configurations                                      ");
  blk("  button_one   = DEPLOY # DEPLOY=true                           ");
  blk("  button_one   = MOOS_MANUAL_OVERRIDE=false # RETURN=false      ");
  blk("  button_two   = RETURN # RETURN=true                           ");
  blk("  button_three = SURVEY-true # SURVEY=true                      ");
  blk("  button_four  = SURVEY-false # SURVEY=false                    ");
  blk("  // button_five through button_twenty similarly supported      ");
  blk("  // button_5 or button_five format supported for all buttons   ");
  blk("                                                                ");
  blk("  // Node Report Source                                         ");
  blk("  node_report_unc = true       {true, FALSE}                    ");
  blk("                                                                ");
  blk("  app_logging = true  // {true or file} By default disabled     ");
  blk("                                                                ");
  blk("  log_the_image = true  // {true or false} By default false     ");
  blk("                                                                ");
  blk("  mission_hash_var = off  (Default is MISSION_HASH)             ");
  blk("                                                                ");
  blk("  beat_flag = HEARTBEAT=true                                    ");
  blk("                                                                ");
  blk("  show_title_ip    = true // {TRUE or false}                    ");
  blk("  show_title_user  = true // {TRUE or false}                    ");
  blk("  show_title_mhash = true // {TRUE or false}                    ");
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
  blk("  APPCAST                                                       ");
  blk("  APPCAST_REQ                                                   ");
  blk("  REALMCAST                                                     ");
  blk("  WATCHCAST                                                     ");
  blk("  VIEW_POLYGON                                                  ");
  blk("  VIEW_SEGLIST                                                  ");
  blk("  VIEW_POINT                                                    ");
  blk("  VIEW_VECTOR                                                   ");
  blk("  VIEW_MARKER                                                   ");
  blk("  VIEW_CIRCLE                                                   ");
  blk("  VIEW_ARROW                                                    ");
  blk("  VIEW_GRID                                                     ");
  blk("  VIEW_RANGE_PULSE                                              ");
  blk("  VIEW_COMMS_PULSE                                              ");
  blk("  NODE_REPORT                                                   ");
  blk("  NODE_REPORT_LOCAL                                             ");
  blk("  GRID_CONFIG                                                   ");
  blk("  GRID_DELTA                                                    ");
  blk("  BEARING_LINE                                                  ");
  blk("  TRAIL_RESET                                                   ");
  blk("  PMV_MENU_CONTEXT                                              ");
  blk("  PMV_CENTER                                                    ");
  blk("  PMV_CONFIG                                                    ");
  blk("  BLOCK_HEARTBEAT                                               ");
  blk("  RESET_MHASH                                                   ");
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
  blk("  HELM_MAP_CLEAR  = 0   (published on startup)                  ");
  blk("  PMV_CONNECT     = 0   (published on startup)                  ");
  blk("  MVIEWER_LCLICK                                                ");
  blk("  MVIEWER_RCLICK                                                ");
  blk("  MVIEWER_UNHANDLED_MAIL                                        ");
  blk("  PLOGGER_CMD                                                   ");
  blk("  REGION_INFO = lat_datum=43.8253, lon_datum=-70.3304,          ");
  blk("                img_file=/Users/.../forrest19.tif, zoom=1,      ");
  blk("                pan_x=-90, pan_y=-280                           ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pMarineViewer", "gpl");
  exit(0);
}










