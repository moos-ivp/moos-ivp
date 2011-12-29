/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: ShoreBroker_Info.cpp                                 */
/*   DATE: Dec 16th 2011                                        */
/****************************************************************/

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
  blk("  registration requests to shoreside pMOOSBridge to bridge user-");
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
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  BRIDGE_VAR = DEPLOY                                           ");
  blk("  BRIDGE_VAR = RETURN                                           ");
  blk("  BRIDGE_VAR = STATION_KEEP                                     ");
  blk("                                                                ");
  blk("  PIGEON_VAR = FOOBAR                                           ");
  blk("                                                                ");
  blk("  MACRO_VAR  = UP_LOITER_$N, UP_LOITER                          ");
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
  blk("  PHI_HOST_INFO    = COMMUNITY=alpha,IP=192.168.1.1,PORT=9000   ");
  blk("                     PORT_UDP=9200                              ");
  blk("  NODE_BROKER_PING = community=henry,host=192.168.1.22,         ");
  blk("                     port=9000,port_udp=9200,keyword=lemon      ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  NODE_BROKER_ACK = community=shoreside,host=192.168.1.199,     ");
  blk("                    port=9000,port_udp=9200                     ");
  blk("                                                                ");
  blk("  PMB_REGISTER    = src_var=DEPLOY_ALL,dest_community=henry,    ");
  blk("                    dest_port=9200,dest_host=192.168.1.22,      ");
  blk("                    dest_alias=DEPLOY                           ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldShoreBroker", "gpl");
  exit(0);
}

