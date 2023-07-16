/****************************************************************/
/*   NAME: Mike Benjamin                                        */
/*   ORGN: MIT, Cambridge MA                                    */
/*   FILE: MissionHash_Info.cpp                                 */
/*   DATE: July 15th, 2023                                      */
/****************************************************************/

#include <cstdlib>
#include <iostream>
#include "MissionHash_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The pMissionHash app is used for generating a MISSION_HASH    ");
  blk("  posting. Normally this is produced by pMarineViewer. In       ");
  blk("  headless mission not running pMarineViewer, this app can be   ");
  blk("  used instead. They should not be both run unless the mission  ");
  blk("  feature is configured to be disabled in pMarineViewer.        ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blu("=============================================================== ");
  blu("Usage: pMissionHash file.moos [OPTIONS]                         ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pMissionHash with the given process name           ");
  blk("      rather than pMissionHash.                                 ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pMissionHash.              ");
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
  blu("pMissionHash Example MOOS Configuration                         ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pMissionHash                                    ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  mission_hash_var = MISSION_HASH  // default                   ");
  blk("                                                                ");
  blk("  // Note: If a config block is NOT provided in the mission     ");
  blk("  //       file, NO config warning will be generated.           ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blu("=============================================================== ");
  blu("pMissionHash INTERFACE                                    ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  MHASH_RESET = true                                            ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  MISSION_HASH = mhash=230716-1140V-AVID-TREE,utc=1689522045.08 ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pMissionHash", "gpl");
  exit(0);
}

