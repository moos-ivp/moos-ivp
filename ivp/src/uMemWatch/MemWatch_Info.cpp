/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: MemWatch_Info.cpp                                    */
/*    DATE: May 4th 2019                                         */
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
#include "MemWatch_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                      ");
  blk("------------------------------------                           ");
  blk("  The uMemWatch application is used for measuring the current  ");
  blk("  memory used by a set of MOOS apps. To measure an app, the app");
  blk("  must publish the MYAPP_PID variable with its own PID. This   ");
  blk("  app will use that PID and the command line utility \"ps\" to ");
  blk("  measure the current memory usage. This app can be configured ");
  blk("  to selectively watch items on a watch list, or watch all     ");
  blk("  apps but selectively ignore apps on an ignore list.          ");
  blk("  The primary output is through both AppCasting, and the       ");
  blk("  publication to MYAPP_MEM each time a new measurement is      ");
  blk("  made.                                                        ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: uMemWatch file.moos [OPTIONS]                            ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uMemWatch with the given process name              ");
  blk("      rather than uMemWatch.                                    ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uMemWatch.                 ");
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
  blu("uMemWatch Example MOOS Configuration                            ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uMemWatch                                       ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  absolute_time_gap = 1   // In Seconds, Default is 1.          ");
  blk("                                                                ");
  blk("  watch_only = pHelmIvP,pNodeReporter                           ");
  blk("                                                                ");
  blk("  ignore     = pBasicContactMgr                                 ");
  blk("                                                                ");
  blk("  app_logging = true  // {true or file} By default disabled     ");
  blk("}                                                               ");
  blk("                                                                ");
  blk("Note: If the watch list is non-empty, ignore list is ignored    ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uMemWatch INTERFACE                                             ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  DB_CLIENTS = pHelmIvP,pBasicContacMgr,pNodeReporter...        ");
  blk("  PHELMIVP_PID = 34821                                          ");
  blk("  PBASICCONTACTMGR_PID = 34824                                  ");
  blk("  PNODEREPORTER_PID = 34824                                     ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  PHELMIVP_MEM = 4.3                                            ");
  blk("  PBASICCONTACTMGR_MEM = 0.8                                    ");
  blk("  PNODEREPORTER_MEM = 2.1                                       ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uMemWatch", "gpl");
  exit(0);
}


