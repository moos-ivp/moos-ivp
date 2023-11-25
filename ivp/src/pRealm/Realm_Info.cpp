/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Realm_Info.cpp                                       */
/*    DATE: December 2nd, 2020                                   */
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
  blk("  configured to interact with pRealm. Typically pRealm is run   ");
  blk("  on both the shoreside and vehicle communities.                ");
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
  mag("  --web,-w                                                      ");
  blk("      Open browser to: https://oceanai.mit.edu/apps/pRealm      ");
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
  blk("  msg_max_hist = 10            //  [1,inf] Default is 10        ");
  blk("                                                                ");
  blk("  hist_var  = TASK_BID                                          ");
  blk("  scope_set = name=events, vars=DEPLOY:RETURN:STATION           ");
  blk("                                                                ");
  blk("  app_logging = true  // {true or file} By default disabled     ");
  blk("                                                                ");
  blk("  // Note: To utilize the watchcast feature of pRealm,          ");
  blk("  // configure a client like pMarineViewer, to set up a watch   ");
  blk("  // cluster. For example, in pMarineViewer:                    ");
  blk("                                                                ");
  blk("  //     watch_cluster key=mission_top, vars=DEPLOY:RETURN:COVER");
  blk("                                                                ");
  blk("}                                                               ");
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
  blk("  APPCAST_REQ  = node=all,app=pRealm,duration=3.0               ");
  blk("                 key=pMarineViewer:alphaapp,thresh=run_warning  ");
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
  blk("  WATCHCAST = node=gilda!Z!var=RETURN!Z!src=pHelmIvP!Z!         ");
  blk("              comm=gilda!Z!loc_time=-3!Z!utc_time=12874303123.04!Z!");
  blk("              sval=false                                        ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pRealm", "gpl");
  exit(0);
}


