/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardMgr_Info.cpp                                   */
/*    DATE: Oct 26th 2012                                        */
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
#include "HazardMgr_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The uFldHazardMgr is a strawman MOOS app for managing hazard  ");
  blk("  sensor information and generation of a hazard report over the ");
  blk("  course of an autonomous search mission.                       ");
  blk("                                                                ");
  blk("                                                                ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: uFldHazardMgr file.moos [OPTIONS]                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uFldHazardMgr with the given process name         ");
  blk("      rather than uFldHazardMgr.                           ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uFldHazardMgr.        ");
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
  blu("uFldHazardMgr Example MOOS Configuration                   ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uFldHazardMgr                                   ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  swath_width = 25                                              ");
  blk("  sensor_pd   = 0.9                                             ");
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
  blu("uFldHazardMgr INTERFACE                                         ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  HAZARDSET_REQUEST    = true                                   ");
  blk("  UHZ_CONFIG_ACK       = vname=archie,width=50,pd=0.86,pfa=0.74,");
  blk("                         pclass=0.6                             ");
  blk("  UHZ_DETECTION_REPORT = x=-150.3,y=-117.5,label=12             ");
  blk("  UHZ_OPTIONS_SUMMARY  = width=10,exp=6,class=0.93:width=50,exp=2,");
  blk("                         class=0.60:width=25,exp=4,class=0.85   ");
  blk("  UHZ_MISSION_PARAMS = penalty_missed_hazard=100,               ");
  blk("             penalty_nonopt_hazard=55,                          ");
  blk("             penalty_false_alarm=35,                            ");
  blk("             penalty_max_time_over=200,                         ");
  blk("             penalty_max_time_rate=0.45,                        ");
  blk("             transit_path_width=25,                             ");
  blk("             search_region = pts={-150,-75:-150,-50:40,-50:40,-75}");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  UHZ_CONFIG_REQUEST   = vname=archie,width=38,pd=0.86          ");
  blk("  UHZ_SENSOR_REQUEST   = vname=archie                           ");
  blk("  HAZARDSET_REPORT     = source=archie#name=Sarah#              ");
  blk("                         x=-150.3,y=-117.5,label=12#            ");
  blk("                         x=-151,y=-217.3,label=01#              ");
  blk("                         x=-178.8,y=-234,label=15#              ");
  blk("                         x=-59.8,y=-294.1,label=13              ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uFldHazardMgr", "gpl");
  exit(0);
}









