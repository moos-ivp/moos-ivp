/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NodeBroker_Info.cpp                                  */
/*    DATE: Dec 19th 2011                                        */
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
#include "NodeBroker_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  Typically run on a vehicle or simulated vehicle in a multi-   ");
  blk("  vehicle context. Used for making a connection to a shoreside  ");
  blk("  community by sending local information about the vehicle such ");
  blk("  as the IP address, community name, and port number being used ");
  blk("  by pMOOSBridge for incoming UDP messages. Presumably the      ");
  blk("  shoreside community uses this to know where to send outgoing  ");
  blk("  UDP messages to the vehicle.                                  ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                          ");
  blu("==========================================================");
  blu("Usage: uFldNodeBroker file.moos [OPTIONS]                 ");
  blu("==========================================================");
  blk("                                                          ");
  showSynopsis();
  blk("                                                          ");
  blk("Options:                                                  ");
  mag("  --alias","=<ProcessName>                                ");
  blk("      Launch uFldNodeBroker with the given                ");
  blk("      process name rather than uFldNodeBroker.            ");
  mag("  --example, -e                                           ");
  blk("      Display example MOOS configuration block.           ");
  mag("  --help, -h                                              ");
  blk("      Display this help message.                          ");
  mag("  --interface, -i                                         ");
  blk("      Display MOOS publications and subscriptions.        ");
  mag("  --version,-v                                            ");
  blk("      Display release version of uFldNodeBroker.          ");
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
  blu("uFldNodeBroker Example MOOS Configuration                       ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldNodeBroker                                  ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  KEYWORD        = lemon                                        ");
  blk("                                                                ");
  blk("  TRY_SHORE_HOST = pshare_route=localhost:9200                  ");
  blk("  TRY_SHORE_HOST = pshare_route=192.168.0.122:9301              ");
  blk("  TRY_SHORE_HOST = pshare_route=multicast_8                     ");
  blk("                                                                ");
  blk("  BRIDGE = src=VIEW_POLYGON                                     ");
  blk("  BRIDGE = src=VIEW_POINT                                       ");
  blk("  BRIDGE = src=VIEW_SEGLIST                                     ");
  blk("                                                                ");
  blk("  BRIDGE = src=NODE_REPORT_LOCAL, alias=NODE_REPORT             ");
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
  blu("uFldNodeBroker INTERFACE                                        ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  PHI_HOST_INFO   = community=alpha,hostip=123.1.1.0,           ");
  blk("                    port_db=9000,port_udp=9200,                 ");
  blk("                    timewarp=8                                  ");
  blk("  NODE_BROKER_ACK = community=shoreside,host=192.168.1.99,      ");
  blk("                    port=9000,port_udp=9200,timewarp=8,         ");
  blk("                    status=ok                                   ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  PMB_REGISTER =     src_var=NODE_BROKER_PING,                  ");
  blk("                     dest_community=shoreside,dest_port=9200,   ");
  blk("                     dest_host=192.168.0.1,                     ");
  blk("                     dest_alias=NODE_BROKER_PING                ");
  blk("  NODE_BROKER_PING = community=henry,host=192.168.1.1,port=9000 ");
  blk("                     port_udp=9200,keyword=lemon,timewarp=8     ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldNodeBroker", "gpl");
  exit(0);
}


