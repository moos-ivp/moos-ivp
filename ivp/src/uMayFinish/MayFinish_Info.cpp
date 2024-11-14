/****************************************************************/
/*   NAME: Mike Benjamin                                        */
/*   ORGN: MIT, Cambridge MA                                    */
/*   FILE: MayFinish_Info.cpp                                   */
/*   DATE: October 31st, 2024                                   */
/****************************************************************/

#include <cstdlib>
#include <iostream>
#include "MayFinish_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  uMayFinish is typically a terminal-launched MOOS app launched ");
  blk("  within a shell script, e.g., xlaunch.sh. It will connect to a ");
  blk("  MOOS community and monitor for a completion event or timeout  ");
  blk("  based on DB_UPTIME. When completed, it simply exits.          ");
  blk("  Presumably to allow the executing script to proceed to a next ");
  blk("  phase. For example a script could proceed to bring down the   ");
  blk("  MOOS community for the mission it was monitoring.             ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blu("=============================================================== ");
  blu("Usage: uMayFinish file.moos [OPTIONS]                           ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uMayFinish with the given process name             ");
  blk("      rather than uMayFinish.                                   ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uMayFinish.                ");
  mag("  --max_time=1200                                               ");
  blk("      Maximum DB_UPTIME before exiting.                         ");
  blk("                                                                ");
  blk("Note: If argv[2] does not otherwise match a known option,       ");
  blk("      then it will be interpreted as a run alias. This is       ");
  blk("      to support pAntler launching conventions.                 ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blu("=============================================================== ");
  blu("uMayFinish Example MOOS Configuration                           ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uMayFinish                                      ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  finish_var = MISSION_EVALUATED  // Default                    ");
  blk("  finish_val = true               // Default                    ");
  blk("                                                                ");
  blk("  max_db_uptime = 3600            // Default is -1 (off)        ");
  blk("}                                                               ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blu("=============================================================== ");
  blu("uMayFinish INTERFACE                                            ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  DB_UPTIME = 78.32                                             ");
  blk("  MISSION_EVALUATED = true                                      ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  APPCAST                                                       ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uMayFinish", "gpl");
  exit(0);
}

