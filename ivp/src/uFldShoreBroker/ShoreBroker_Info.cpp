/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ShoreBroker_Info.cpp                                 */
/*    DATE: Dec 16th 2011                                        */
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
#include "ReleaseInfo.h"
#include "ColorParse.h"
#include "ShoreBroker_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  Typically run in a shoreside community. Takes reports from    ");
  blk("  remote vehicles describing how they may be reached. Posts     ");
  blk("  registration requests to shoreside pShare to share user-      ");
  blk("  provided list of variables out to vehicles. Upon learning of  ");
  blk("  vehicle JAKE will create bridges FOO_ALL and FOO_JAKE to JAKE,");
  blk("  for all such user-configured variables.                       ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                          ");
  blu("==========================================================");
  blu("Usage: uFldShoreBroker file.moos [OPTIONS]                ");
  blu("==========================================================");
  blk("                                                          ");
  showSynopsis();
  blk("                                                          ");
  blk("Options:                                                  ");
  mag("  --alias","=<ProcessName>                                ");
  blk("      Launch uFldShoreBroker with the given               ");
  blk("      process name rather than uFldShoreBroker.           ");
  mag("  --example, -e                                           ");
  blk("      Display example MOOS configuration block.           ");
  mag("  --help, -h                                              ");
  blk("      Display this help message.                          ");
  mag("  --interface, -i                                         ");
  blk("      Display MOOS publications and subscriptions.        ");
  mag("  --version,-v                                            ");
  blk("      Display release version of uFldShoreBroker.         ");
  mag("  --web,-w                                                ");
  blk("      Open browser to: https://oceanai.mit.edu/ivpman/apps/uFldShoreBroker ");
  blk("                                                          ");
  blk("Note: If argv[2] does not otherwise match a known option, ");
  blk("      then it will be interpreted as a run alias. This is ");
  blk("      to support pAntler launching conventions.           ");
  blk("                                                          ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uFldShoreBroker Example MOOS Configuration                      ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldShoreBroker                                 ");
  blk("{                                                               ");
  blk("  AppTick   = 2                                                 ");
  blk("  CommsTick = 2                                                 ");
  blk("                                                                ");
  blk("  warning_on_stale      = false (default)                       ");
  blk("  auto_bridge_realmcast = true  (default)                       ");
  blk("  auto_bridge_appcast   = true  (default)                       ");
  blk("  auto_bridge_mhash     = true  (default)                       ");
  blk("                                                                ");
  blk("  bridge = src=DEPLOY_ALL, alias=DEPLOY                         ");
  blk("  bridge = src=DEPLOY_$V,  alias=DEPLOY                         ");
  blk("                                                                ");
  blk("  try_vnode = 192.168.4.24:9200                                 ");
  blk("  try_vnode = 192.168.4.25:9200                                 ");
  blk("                                                                ");
  blk("  qbridge = RETURN                                              ");
  blk("  qbridge = NODE_REPORT, STATION_KEEP                           ");
  blk("                                                                ");
  blk("  bridge = src=UP_LOITER_$N, alias=UP_LOITER                    ");
  blk("                                                                ");
  blk("  // Note: [qbridge = FOO]  is shorthand for                    ");
  blk("  //       [bridge = src=FOO_$V,  alias=FOO] and                ");
  blk("  //       [bridge = src=FOO_ALL, alias=FOO]                    ");
  blk("                                                                ");
  blk("  app_logging = true  // {true or file} By default disabled     ");
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
  blu("uFldShoreBroker INTERFACE                                       ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  PHI_HOST_INFO    = community=alpha,hostip=123.1.1.0,          ");
  blk("                     port_db=9000,port_udp=9200,timewarp=8      ");
  blk("                     pshare_iroutes=localhost:9200&multicast9    ");
  blk("  NODE_BROKER_PING = community=henry,host=12.16.1.22,port=9000, ");
  blk("                     time_warp=8,pshare_iroutes=12.18.1.22:9200,");
  blk("                     time=1325178800.81                         ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  NODE_BROKER_ACK_VNAME = community=shoreside,                  ");
  blk("                          hostip=192.168.1.199,port_db=9000,    ");
  blk("                          pshare_iroutes=localhost:9300,        ");
  blk("                          time_warp=8,status=ok,id=123          ");
  blk("                                                                ");
  blk("  NODE_BROKER_VACK = alpha                                      ");
  blk("                                                                ");
  blk("  PSHARE_CMD =       cmd=output, src_name=DEPLOY_ALL,           ");
  blk("                     dest_name=DEPLOY,route=18.11.4.23:9200     ");
  blk("                                                                ");
  blk("  TRY_SHORE_HOST =   pshare_route=192.168.4.100:9200            ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldShoreBroker", "gpl");
  exit(0);
}










