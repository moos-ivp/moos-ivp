/*****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard    */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA      */
/*   FILE: HazardMetric_Info.cpp                                 */
/*   DATE: Mar 12th 2012                                         */
/*****************************************************************/

#include <cstdlib>
#include <iostream>
#include "HazardMetric_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The uFldHazardMetric application is used for               ");
  blk("                                                                ");
  blk("                                                                ");
  blk("                                                                ");
  blk("                                                                ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: uFldHazardMetric file.moos [OPTIONS]                     ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uFldHazardMetric with the given process name       ");
  blk("      rather than uFldHazardMetric.                             ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uFldHazardMetric.          ");
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
  blu("uFldHazardMetric Example MOOS Configuration                     ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldHazardMetric                                ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  score_hazard_right =  10    // default                        ");
  blk("  score_hazard_wrong = -50    // default                        ");
  blk("  score_benign_right =   5    // default                        ");
  blk("  score_benign_wrong = -25    // default                        ");
  blk("  score_hazard_zilch = -75    // default                        ");
  blk("  score_benign_zilch =   0    // default                        ");
  blk("                                                                ");
  blk("  hazard_file   = hazards.txt                                   ");
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
  blu("uFldHazardMetric INTERFACE                                      ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  HAZARD_REPORT = x=45,y=88,hazard=true,label=05                ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  HAZARD_METRIC_REPORT  = vname=henry,score=-15                 ");
  blk("  HAZARD_METRIC_VERBOSE = vname=henry,score=-90,                ");
  blk("                          hr=9,hr_pts=90                        ");
  blk("                          hw=1,hw_pts=-50,                      ");
  blk("                          br=9,br_pts=45,                       ");
  blk("                          bw=4,bw_pts=-100,                     ");
  blk("                          hz=1,hz_pts=-75,                      ");
  blk("                          bz=1,bz_pts=0                         ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldHazardMetric", "gpl");
  exit(0);
}

