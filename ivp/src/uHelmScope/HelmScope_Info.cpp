/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: HelmScopeInfo.cpp                                    */
/*    DATE: July 6th 2011                                        */
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
#include "HelmScope_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The uHelmScope application is a terminal-based (non-GUI) scope");
  blk("  onto a running IvP Helm process, and key MOOS variables.      ");
  blk("  uHelmScope provides behavior summaries, activity states, and  ");
  blk("  recent behavior postings to the MOOSDB.                       ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: uHelmScope file.moos [OPTIONS] [MOOS Variables]          ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uHelmScope with the given process name rather      ");
  blk("      than uHelmScope.                                          ");
  mag("  --clean, -c                                                   ");
  blk("      MOOS variables specified in given .moos file are excluded ");
  blk("      from the MOOSDB-Scope output block.                       ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --noscope,-x                                                  ");
  blk("      Suppress MOOSDB-Scope output block.                       ");
  mag("  --noposts,-p                                                  ");
  blk("      Suppress Behavior-Posts output block.                     ");
  mag("  --novirgins,-g                                                ");
  blk("      Suppress virgin variables in MOOSDB-Scope output block.   ");
  mag("  --streaming,-s                                                ");
  blk("      Streaming (unpaused) output enabled.                      ");
  mag("  --trunc,-t                                                    ");
  blk("      Column truncation of scope output is enabled.             ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of uHelmScope.                ");
  mag("  --web,-w                                                      ");
  blk("      Open browser to: https://oceanai.mit.edu/ivpman/apps/uHelmScope ");
  blk("                                                                ");
  blk("MOOS Variables                                                  ");
  blk("  MOOS_VAR1 MOOSVAR_2, ..., MOOSVAR_N                           ");
  blk("                                                                ");
  blk("Further Notes:                                                  ");
  blk("  (1) The order of command line arguments is irrelvant.         ");
  blk("  (2) Any MOOS variable used in a behavior run condition or used");
  blk("      in hierarchical mode declarations will be automatically   ");
  blk("      subscribed for in the MOOSDB scope.                       ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uHelmScope Example MOOS Configuration                           ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uHelmScope                                      ");
  blk("{                                                               ");
  blk("  AppTick   = 1   "," // MOOSApp default is 4                   ");
  blk("  CommsTick = 1   "," // MOOSApp default is 4                   ");
  blk("                                                                ");
  blk("  paused    = false   // default                                ");
  blk("                                                                ");
  blk("  display_moos_scope = true    "," // default                   ");
  blk("  display_bhv_posts  = true    "," // default                   ");
  blk("  display_virgins    = true    "," // default                   ");
  blk("  truncated_output   = false   "," // default                   ");
  blk("  behaviors_concise  = true    "," // default                   ");
  blk("                                                                ");
  blk("  var  = NAV_X, NAV_Y, NAV_SPEED, NAV_DEPTH   // MOOS vars are  ");
  blk("  var  = DESIRED_HEADING, DESIRED_SPEED       // case sensitive ");
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
  blu("uHelmScope INTERFACE                                           ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  All variables specified on the command line.                  ");
  blk("  All variables used in behavior run conditions or helm modes.  ");
  blk("  +                                                             ");
  blk("  DB_UPTIME          = 195.19385                                ");
  blk("  IVPHELM_SUMMARY    = iter=767,utc_time=1324844699.26,         ");
  blk("                       active_bhvs=waypt_survey$1324844507.4$   ");
  blk("                       100$6$0.79$0/0$1$1                       ");
  blk("  IVPHELM_POSTINGS   =                                          ");
  blk("  IVPHELM_STATEVARS  = DEPLOY,HSLINE,RETURN                     ");
  blk("  IVPHELM_DOMAIN     = speed,0,4,21:course,0,359,360            ");
  blk("  IVPHELM_MODESET    =                                          ");
  blk("  IVPHELM_ENGAGED    = ENGAGED                                  ");
  blk("  IVPHELM_LIFE_EVENT = time=2.25, iter=1, bname=hsline,         ");
  blk("                       btype=BHV_HSLine, event=spawn,           ");
  blk("                       seed=helm_startup                        ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  IVPHELM_REJOURNAL  = 1                                        ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uHelmScope", "gpl");
  exit(0);
}










