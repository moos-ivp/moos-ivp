/****************************************************************/
/*   NAME: Mike Benjamin                                        */
/*   ORGN: MIT, Cambridge MA                                    */
/*   FILE: SpoofNode_Info.cpp                                   */
/*   DATE: May 16th, 2022                                       */
/****************************************************************/

#include <cstdlib>
#include <iostream>
#include "SpoofNode_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk(" The pSpoofNode app injects one or more NODE_REPORT msg for a   ");
  blk(" contact that doesnt really exist. Typically on the shorseside. ");
  blk(" With this tool the user can for example arrange for a node     ");
  blk(" node to be generated with a mouse click in pMarineViewer       ");
  blk(" The pSpoofNode app will continue to post NODE_REPORT           ");
  blk(" messages indefinitely or, if a duration is given, until it has ");
  blk(" expired. If a speed is given, the position will be updated.    ");
  blk(" A useful tool for testing aspects of the contact manager and/or");
  blk(" the spawning and cleanup of helm dynamic behaviors.            ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: pSpoofNode file.moos [OPTIONS]                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pSpoofNode with the given process name             ");
  blk("      rather than pSpoofNode.                                   ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pSpoofNode.                ");
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
  blu("pSpoofNode Example MOOS Configuration                           ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pSpoofNode                                      ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  refresh_interval = 1         // Secs, default = 1             ");
  blk("  default_length = 5           // Meters, default = 5           ");
  blk("  default_vtype =              // Shape, default = kayak        ");
  blk("  default_group =              // Default = \"\"                ");
  blk("  default_vsource =            // Default = \"\"                ");
  blk("  default_color =              // Default = purple              ");
  blk("                                                                ");
  blk("  default_hdg = 45             // Degrees, default=45           ");
  blk("  default_spd = 2              // M/sec, default=0              ");
  blk("  default_duration = 0         // Sec, default=0, never expire  ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blu("=============================================================== ");
  blu("pSpoofNode INTERFACE                                    ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  SPOOF = x=12,y=9,hdg=180,spd=0.1,name=zed,group=blue_team     ");
  blk("          type=kayak,vsource=ais,len=5,color=green,dur=20       ");
  blk("                                                                ");
  blk("  SPOOF_CANCEL = vname=zed                                      ");
  blk("  SPOOF_CANCEL = group=redteam                                  ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  NODE_REPORT =                                                 ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pSpoofNode", "gpl");
  exit(0);
}

