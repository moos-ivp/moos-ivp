/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: HostInfo_Info.cpp                                    */
/*    DATE: Dec 9th 2011                                         */
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
#include "ColorParse.h"
#include "ReleaseInfo.h"
#include "HostInfo_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  Automatically detect the vehicle's host information including ");
  blk("  the IP addresses, port being used by the MOOSDB, the port     ");
  blk("  being used by local pShare for UDP listening, and the         ");
  blk("  community name for the local MOOSDB. Post these to facilitate ");
  blk("  automatic intervehicle communications in especially in multi- ");
  blk("  vehicle scenarios where the local IP address changes w/ DHCP. ");
}

//----------------------------------------------------------------
// Procedure: showHelp

void showHelpAndExit()
{
  blk("                                                                ");
  blk("Usage: pHostInfo file.moos [OPTIONS]                            ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pHostInfo with the given process name rather       ");
  blk("      than pHostInfo.                                           ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --hostip","=<HostIP>                                          ");
  blk("      Use the given IP address as a fall-back rather than the   ");
  blk("      the fall-back address of \"localhost\".                   ");
  mag("  --HOSTIP","=<HostIP>                                          ");
  blk("      Force the use of the given IP address as the reported IP  ");
  blk("      address ignoring any other auto-discovered IP address.    ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display release version of pHostInfo.                     ");
  mag("  --web,-w                                                      ");
  blk("      Open browser to: https://oceanai.mit.edu/apps/pHelmIvP    ");
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
  blu("pHostInfo Example MOOS Configuration                            ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pHostInfo                                       ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  temp_file_dir  = ./                                           ");
  blk("  default_hostip = 192.168.0.55    // default is \"localhost\"  ");
  blk("                                                                ");
  blk("  default_hostip_force = 192.168.0.55                           ");
  blk("  prefer_interface     = wlan0                                  ");
  blk("                                                                ");
  blk("  app_logging = true  // {true or file} By default disabled     ");
  blk("}                                                               ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("pHostInfo INTERFACE                                             ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("DOCUMENTATION: moos-ivp.org/apps/pHostInfo                      ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  PHI_HOST_IP         = 192.168.0.1                             ");
  blk("  PHI_HOST_IP_ALL     = 192.168.0.1,10.1.127.3                  ");
  blk("  PHI_HOST_IP_VERBOSE = OSX_WIFI=192.168.0.1,OSX_ETHERNET=      ");
  blk("                        10.1.127.3                              ");
  blk("  PHI_HOST_PORT_DB    = 9000                                    ");
  blk("  PHI_HOST_INFO       = community=alpha,hostip=123.1.1.0,       ");
  blk("                        port_db=9000,timewarp=8,                ");
  blk("                        pshare_iroutes=localhost:9200&multicast_9");
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  PSHARE_INPUT_SUMMARY = localhost:9000:udp,                    ");
  blk("                         1.2.3.4:2000:multicast_23              ");
  blk("  HOST_INFO_REQUEST    = true                                   ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pHostInfo", "gpl");
  exit(0);
}









