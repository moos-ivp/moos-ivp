/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Delver_Info.cpp                                      */
/*    DATE: Sep 2nd, 2022                                        */
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
#include "Delver_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis()

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The uFldDelve app monitors traffic to/from the shoreside via  ");
  blk("  the pShare application. It tallies the total messages and     ");
  blk("  chars and the message and character RATES. The list of msgs   ");
  blk("  to monitor is derived from a message from uFldNodeBroker and  ");
  blk("  uFldShoreBroker.                                              ");
  blk("  Note: to use this, the variable PSHARE_NODE_VARS must also    ");
  blk("  be shared from the vehicle to the shoreside.                  ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  blu("=============================================================== ");
  blu("Usage: uFldDelve  file.moos [OPTIONS]                           ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uFldDelve with the given process name              ");
  blk("      rather than uFldDelve.                                    ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uFldDelve.                 ");
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
  blu("uFldDelve Example MOOS Configuration                            ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldDelve                                       ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  rate_frame = 10 // secs (default)                             ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit()

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uFldDelve INTERFACE                                             ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  NODE_PSHARE_VARS =  ACK_MESSAGE,MEDIATED_MESSAGE,NODE_REPORT, ");
  blk("                      NODE_PSHARE_VARS,TASK_MGR_STAT,REALMCAST, ");
  blk("                      APPCAST,VIEW_SEGLIST,VIEW_POINT,          ");
  blk("                      VIEW_POLYGON,WATCHCAST,REALMCAST_CHANNELS ");
  blk("  UFSB_BRIDGE_VARS =  ACK_MESSAGE_ABE,ACK_MESSAGE_ALL,          ");
  blk("                      APPCAST_REQ_ALL,APPCAST_REQ_BEN,...       ");
  blk("  UFSB_QBRIDGE_VARS = ACK_MESSAGE,APPCAST_REQ,ATURN,            ");
  blk("                      AVOID,COMMS_POLICY,CONVOY,DEPLOY,...      ");
  blk("  UFSB_NODE_COUNT = 6                                           ");
  blk("                                                                ");
  blk("  Note: Will also subscribe for any app named on the *_VARS list");
  blk("        above                                                   ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  Nothing currently.                                            ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit()

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldDelve", "gpl");
  exit(0);
}


