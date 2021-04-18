/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: FrontGridRender_Info.h                               */
/*    DATE: Arp 13th, 2012                                       */
/*****************************************************************/

#include <cstdlib>
#include <iostream>
#include "FrontGridRender_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The pFrontGridRender application is used for posting a grid   ");
  blk("  representing a front simulation, or a third party estimate of ");
  blk("  a front. Primary output is a VIEW_GRID message consumed likely");
  blk("  by the pMarineViewer application.                             ");
  blk("                                                                ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: pFrontGridRender file.moos [OPTIONS]                     ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pFrontGridRender with the given process name       ");
  blk("      rather than pFrontGridRender.                             ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pFrontGridRender.          ");
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
  blu("pFrontGridRender Example MOOS Configuration                     ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pFrontGridRender                                ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  GRID_CONFIG = pts={-50,-40: -10,0: 180,0: 180,-150: -50,-150} ");
  blk("  GRID_CONFIG = cell_size=5                                     ");
  blk("  GRID_CONFIG = cell_vars=x:0:y:0:z:0                           ");
  blk("  GRID_CONFIG = cell_min=x:0                                    ");
  blk("  GRID_CONFIG = cell_max=x:10                                   ");
  blk("                                                                ");
  blk("  GRID_REFRESH_INTERVAL = 0.15  // secs, default                ");
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
  blu("pFrontGridRender INTERFACE                                    ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  UCTD_TRUE_PARAMETERS = vname=shoreside,offset=-100,angle=50,  ");
  blk("                         amplitude=50,period=120,wavelength=300,");
  blk("                         alpha=100,beta=40,tempnorth=20,        ");
  blk("                         tempsouth=25                           ");
  blk("  UCTD_GUESS_PARAMETERS = vname=shoreside,offset=-140,angle=22, ");
  blk("                          amplitude=50,period=120,wavelength=99,");
  blk("                          alpha=100,beta=40,tempnorth=20,       ");
  blk("                          tempsouth=22                          ");
  blk("                                                                ");
  blk("  FGR_SHOW = truth, guess, show, or none                        ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  VIEW_GRID = pts={-100,-200:200,-200:200,25:-100,25},          ");
  blk("              cell_size=7,cell_vars=x:0,cell=0:x:24.65342,...   ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pFrontGridRender", "gpl");
  exit(0);
}

