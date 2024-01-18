/****************************************************************/
/*   NAME: Mike Benjamin                                             */
/*   ORGN: MIT, Cambridge MA                                    */
/*   FILE: MapMarkers_Info.cpp                               */
/*   DATE: December 29th, 1963                                  */
/****************************************************************/

#include <cstdlib>
#include <iostream>
#include "MapMarkers_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The pMapMarkers app is configured with a number of markers    ");
  blk("  each with a name and location, and will post a VIEW_MARKER    ");
  blk("  notification for consumption of a GUI like pMarineViewer.     ");
  blk("  Typically the set of markers is a local coordinate frame, e.g,");
  blk("  a playing field, where markers are possible coordinates for   ");
  blk("  boundaries, goals, mid-field lines and so on.                 ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: pMapMarkers file.moos [OPTIONS]                          ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pMapMarkers with the given process name            ");
  blk("      rather than pMapMarkers.                                  ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pMapMarkers.               ");
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
  blu("pMapMarkers Example MOOS Configuration                          ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pMapMarkers                                     ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  show_markers = true       // {TRUE,false}                     ");
  blk("  opfile = fld_base.opf                                         ");
  blk("                                                                ");
  blk("  marker_size  = 5          // Default: 3                       ");
  blk("  marker_shape = triangle   // Default: circle                  ");
  blk("  marker_color = gray40     // Default: light_green             ");
  blk("  marker_lcolor = white     // Default: gray60                  ");
  blk("  marker_ecolor = black     // Default: black  (edge color)     ");
  blk("                                                                ");
  blk("  show_mlabels = true   // {true,FALSE}                         ");
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
  blu("pMapMarkers INTERFACE                                           ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  MAP_MARKERS = false                                           ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  VIEW_MARKER = x=167.97,y=-202.57,width=3,primary_color=gray40,");
  blk("                type=circle,label=ZT,label_color=gray60         ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pMapMarkers", "gpl");
  exit(0);
}

