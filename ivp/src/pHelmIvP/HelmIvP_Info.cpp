/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmIvP_Info.cpp                                     */
/*    DATE: July 5th 2011                                        */
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
#include "HelmIvP_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  pHelmIvP is a behavior-based autonomous decision-making MOOS  ");
  blk("  application. It consists of a set of behaviors reasoning over ");
  blk("  a common decision space such as the vehicle heading and speed.");
  blk("  Behaviors are reconciled using multi-objective optimization   ");
  blk("  with the Interval Programming (IvP) model.                    ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: pHelmIvP file.moos [BEHAVIOR FILES]  [OPTIONS]           ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch pHelmIvP with the given process name rather        ");
  blk("      than pHelmIvP.                                            ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of pHelmIvP.                  ");
  blk("                                                                ");
  blk("Behavior files:                                                 ");
  blk("      One or more filenames to get IvP Helm config parameters.  ");
  blk("      Optional arg since behavior filename may be specified in  ");
  blk("      the pHelmIvP config block of the .moos file.              ");
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
  blu("pHelmIvP Example MOOS Configuration                             ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = pHelmIvP                                        ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  // Configuring behavior file and decision domain              ");
  blk("  ivp_behavior_dir =  /Users/bob/moos-ivp-bob/lib/              ");
  blk("  behaviors        =  filename.bhv                              ");
  blk("  domain           =  course:0:359:360                          ");
  blk("  domain           =  speed:0:2:21                              ");
  blk("  domain           =  depth:0:1000:1001                         ");
  blk("                                                                ");
  blk("  // Configure Helm Engagement policies                         ");
  blk("  start_engaged        = false     // or {true}                 ");
  blk("  allow_disengaged     = true      // or {false}                ");
  blk("  disengage_on_allstop = false     // or {true}                 ");
  blk("                                                                ");
  blk("  // Provide alternative to MOOS_MANUAL_OVERRIDE directive      ");
  blk("  other_override_var   = AUTONOMY_OVERRIDE                      ");
  blk("                                                                ");
  blk("  // Handle possible time skews from external communities       ");
  blk("  node_skew            = charlie,45  ","// vehicle_name, secs   ");
  blk("  ok_skew              = 60          ","// or {any}             ");
  blk("                                                                ");
  blk("  // Configure the verbosity of terminal output.                ");
  blk("  verbose              = terse  "," // or {true,false,quiet}    ");
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
  blu("pHelmIvP INTERFACE                                              ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  The Helm primarily registers for the MOOS variables needed by ");
  blk("  the particular behaviors used in a given configuration, and   ");
  blk("  variables used in helm mode declarations.                     ");
  blk("  +                                                             ");
  blk("  MOOS_MANUAL_OVERIDE                                           ");
  blk("  MOOS_MANUAL_OVERRIDE                                          ");
  blk("  RESTART_HELM                                                  ");
  blk("  IVPHELM_VERBOSE                                               ");
  blk("  IVPHELM_REJOURNAL                                             ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  The helm publishes MOOS variables requested for posting by the");
  blk("  behaviors running at a given point in time.                   ");
  blk("  +                                                             ");
  blk("  The helm also publishes a decision for each of its configured ");
  blk("  decision variables, e.g., DESIRED_SPEED etc. In general, if   ");
  blk("  foobar is a decision variables, DESIRED_FOOBAR may be posted  ");
  blk("  if there is a behavior reasoning about that decision variable.");
  blk("  +                                                             ");
  blk("  IVPHELM_SUMMARY                                               ");
  blk("  HELM_IPF_COUNT                                                ");
  blk("  BHV_ERROR                                                     ");
  blk("  BHV_WARNING                                                   ");
  blk("  CREATE_CPU                                                    ");
  blk("  LOOP_CPU                                                      ");
  blk("  BCOUNT                                                        ");
  blk("  TBCOUNT                                                       ");
  blk("  HITER                                                         ");
  blk("  BHV_IPF                                                       ");
  blk("  IVPHELM_POSTINGS                                              ");
  blk("  IVPHELM_STATEVARS                                             ");
  blk("  IVPHELM_LIFE_EVENT                                            ");
  blk("  IVPHELM_ENGAGED                                               ");
  blk("  IVPHELM_DOMAIN                                                ");
  blk("  IVPHELM_MODESET                                               ");
  blk("  IVPHELM_ALLSTOP                                               ");
  blk("  PLOGGER_CMD                                                   ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pHelmIvP", "navy");
  exit(0);
}

