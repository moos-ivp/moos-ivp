/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ProcessWatch_Info.cpp                                */
/*    DATE: Aug 8th 2011                                         */
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
#include "ProcessWatch_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The uProcessWatch application is a simple simulator of water    ");
  blk("  current. Based on local current information from a given file,");
  blk("  it repeately reads the vehicle's present position and         ");
  blk("  publishes a force vector, presumably consumed by uSimMarine.  ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: uProcessWatch file.moos [OPTIONS]                        ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uProcessWatch with the given process name rather   ");
  blk("      than uProcessWatch.                                       ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uProcessWatch.             ");
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
  blu("uProcessWatch Example MOOS Configuration                        ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  WATCH_ALL = true                                              ");
  blk("                                                                ");
  blk("  WATCH  = pMarinePID:PID_OK                                    ");
  blk("  WATCH  = uSimMarine:USM_OK                                    ");
  blk("                                                                ");
  blk("  NOWATCH = uXMS*                                               ");
  blk("                                                                ");
  blk("  SUMMARY_WAIT = 10  ","// Seconds                              ");
  blk("                                                                ");
  blk("  POST_MAPPING = PROC_WATCH_FULL_SUMMARY, UPW_FULL_SUMMARY      ");
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
  blu("uProcessWatch INTERFACE                                         ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  DB_CLIENTS  = uXMS_419,pMarineViewer,pHelmIvP,pMarinePID,     ");
  blk("                uSimMarine,uProcessWatch,pNodeReporter,pLogger, ");
  blk("                DBWebServer,                                    ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  If configured by the user, will publish a dedicated variable  ");
  blk("  for watching a given process, e.g., WATCH=pHelmIvP:HELM_OK    ");
  blk("  will publish the variable HELM_OK regarding pHelmIvP status.  ");
  blk("                                                                ");
  blk("  PROC_WATCH_FULL_SUMMARY = pMarineViewer(1/0),pHelmIvP(1/0),   ");
  blk("                            uSimMarine(1/0),pMarinePID(1/0)     ");
  blk("  PROC_WATCH_SUMMARY      = All Present                         ");
  blk("  UPW_EVENT               = Process [pMarinePID] is noted to be ");
  blk("                            present.                            ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uProcessWatch", "gpl");
  exit(0);
}

