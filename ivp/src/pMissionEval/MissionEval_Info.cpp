/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: MissionEval_Info.cpp                                 */
/*    DATE: Sep 19th, 2020                                       */
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
#include "MissionEval_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The pMissionEval app is used for determining if a mission has ");
  blk("  succeeded. A set of Pass/Fail logic conditions are specified  ");
  blk("  and at the right point in time, these conditions are evaluated");
  blk("  with results posted in user-configurable flags. The point in  ");
  blk("  time when evaluation is done is also determined by user       ");
  blk("  configurable logic conditions. The goal is to have a succinct ");
  blk("  result (one or two variables) that can be easily checked by   ");
  blk("  log tools for evualtion, or uQueryDB for mission stopping     ");
  blk("  criteria.                                                     ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: pMissionEval file.moos [OPTIONS]                         ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pMissionEval with the given process name           ");
  blk("      rather than pMissionEval.                                 ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pMissionEval.              ");
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
  blu("pMissionEval Example MOOS Configuration                         ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pMissionEval                                    ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  lead_condition = STATION_REACHED = true                       ");
  blk("  pass_condition = FLAG_A = true                                ");
  blk("  pass_condition = FLAG_B = true                                ");
  blk("  fail_condition = FLAG_C = oops                                ");
  blk("  fail_condition = FLAG_D = oops                                ");
  blk("                                                                ");
  blk("  result_flag = SAY_MOOS=Checking                               ");
  blk("  pass_flag   = SAY_MOOS=pass                                   ");
  blk("  fail_flag   = SAY_MOOS=fail                                   ");
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
  blu("pMissionEval INTERFACE                                          ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  Variables participating in any of the logic conditions        ");
  blk("                                                                ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  Variables named in any of the result result flags.            ");
  blk("                                                                ");
  blk("  MISSION_RESULT     = pending/pass/fail                        ");
  blk("  MISSION_EVALUATED  = true/false                               ");
  blk("  MISSION_EVAL_STAT  = unmet_lead_conds                         ");
  blk("                     = unmet_pass_conds                         ");
  blk("                     = met_fail_conds                           ");
  blk("                     = pass                                     ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pMissionEval", "gpl");
  exit(0);
}


