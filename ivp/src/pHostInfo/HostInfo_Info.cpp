/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: HostInfo_Info.cpp                                    */
/*   DATE: Dec 9th 2011                                         */
/****************************************************************/

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
  blk("  being used by local pMOOSBridge for UDP listening, and the    ");
  blk("  community name for the local MOOSDB. Post these to facilitate ");
  blk("  automatic intervehicle communications in especially in multi- ");
  blk("  vehicle scenarios where the local IP address changes w/ DHCP. ");
}

//----------------------------------------------------------------
// Procedure: showHelp

void showHelpAndExit()
{
  blk("                                                          ");
  blk("Usage: uFldCommsRegister file.moos [OPTIONS]              ");
  blk("                                                          ");
  showSynopsis();
  blk("                                                          ");
  blk("Options:                                                  ");
  mag("  --alias","=<ProcessName>                                ");
  blk("      Launch uFldCommsRegister with the given             ");
  blk("      process name rather than uFldCommsRegister.         ");
  mag("  --example, -e                                           ");
  blk("      Display example MOOS configuration block.           ");
  mag("  --help, -h                                              ");
  blk("      Display this help message.                          ");
  mag("  --interface, -i                                         ");
  blk("      Display MOOS publications and subscriptions.        ");
  mag("  --version,-v                                            ");
  blk("      Display release version of uFldCommsRegister.       ");
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
  blu("pHostInfo Example MOOS Configuration                            ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pHostInfo                                       ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  TEMP_FILE_DIR = ./                                            ");
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
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  PHI_HOST_IP         = 192.168.0.1                             ");
  blk("  PHI_HOST_IP_ALL     = 192.168.0.1,10.1.127.3                  ");
  blk("  PHI_HOST_IP_VERBOSE = OSX_WIFI=192.168.0.1,OSX_ETHERNET=      ");
  blk("                        10.1.127.3                              ");
  blk("  PHI_HOST_PORT       = 9000                                    ");
  blk("  PHI_HOST_PORT_UDP   = 9200                                    ");
  blk("  PHI_HOST_INFO       = community=alpha,hostip=123.1.1.0,       ");
  blk("                        port_db=9000,port_udp=9200,             ");
  blk("                        timewarp=8                              ");
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  PMB_UDP_LISTEN    = 9200                                      ");
  blk("  HOST_INFO_REQUEST = true                                      ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pHostInfo", "gpl");
  exit(0);
}

