/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: Realm_Info.cpp                                  */
/*    DATE: December 2nd, 2020                              */
/************************************************************/

#include <cstdlib>
#include <iostream>
#include "Realm_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The pRealm application is used for shadowing the local MOOSDB ");
  blk("  and generating on-demand RealmCast reports. These reports are ");
  blk("  requested and consumed by pMarineViewer or similar app        ");
  blk("  configured to interact with pRealm. Typically pRealm is run on");
  blk("  both the shoreside and vehicle communities.                   ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: pRealm file.moos [OPTIONS]                               ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pRealm with the given process name instead of      ");
  blk("      pRealm.                                                   ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pRealm.                    ");
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
  blu("pRealm Example MOOS Configuration                               ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pRealm                                          ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  relcast_interval = 0.8       //  [0.4, 15] Default is 0.8     ");
  blk("  summary_interval = 2.0       //  [1, 10] Default is 2         ");
  blk("  wrap_length = 90             //  [1,inf] Default is 90        ");
  blk("  trunc_length = 270           //  [1,inf] Default is 270       ");
  blk("  msg_max_history = 10         //  [1,inf] Default is 10        ");
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
  blu("pRealm INTERFACE                                                ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  REALMCAST_REQ = channel=pHostInfo,duration=3,nosrc,nocom,     ");
  blk("                          nosubs,wrap,trunc                     ");
  blk("  DB_RWSUMMARY  = MOOSDB_shoreside uFldNodeComms=APPCAST_REQ:   ");
  blk("                  NODE_MESSAGE:NODE_REPORT:NODE_REPORT_LOCAL:   ");
  blk("  DB_RWSUMMARY = dbname app=sub:...:sub&pub:...pub,             ");
  blk("                 app=sub:...:sub&pub:...pub,                    ");
  blk("                                                                ");
  blk("  Note: pRealm will examine DR_RWSUMMARY and subsequently       ");
  blk("        Register for all vars identified over all apps.         ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  REALMCAST  = node=shoreside!&#proc=pLogger!&#count=6!&#mess...");
  blk("  REALMCAST_CHANNELS = node=henry#channels=MOOSDB_henry,        ");
  blk("                       pHelmIvP,pHostInfo,pLogger,pMarinePID,   ");
  blk("                       pNodeReporter,pShare,uFldNodeBroker,     ");
  blk("                       uLoadWatch,uProcessWatch,uSimMarine      ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pRealm", "gpl");
  exit(0);
}

