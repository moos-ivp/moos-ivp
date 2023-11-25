/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: HelmIvP_Info.cpp                                     */
/*    DATE: July 5th 2011                                        */
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
  mag("  --verbose=<setting>                                           ");
  blk("      May be \"VERBOSE\", \"terse\", or \"quiet\".              ");
  mag("  --web,-w                                                      ");
  blk("      Open browser to: https://oceanai.mit.edu/apps/pHelmIvP    ");
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
  blk("                                                                ");
  blk("  // Insist that at least one non-constraint behavior be active ");
  blk("  goals_mandatory      = true  "," // or {true,FALSE}           ");
  blk("                                                                ");
  blk("  // Allow unfound bhv directories to not be a problem.         ");
  blk("  bhv_dir_not_found_ok = true "," // or {true,FALSE}            ");
  blk("                                                                ");
  blk("  // Name apps to wait on before posting onHelmStart messages.  ");
  blk("  hold_on_apps = pBasicContactMgr, pTaskManager                 ");
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
  blk("  APPCAST_REQ  = node=all,app=uFldCollObDetect,duration=3.0     ");
  blk("                 key=pMarineViewer:alphaapp,thresh=run_warning  ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  The helm publishes MOOS variables requested for posting by the");
  blk("  behaviors running at a given point in time.                   ");
  blk("  +                                                             ");
  blk("  The helm also publishes a decision for each of its configured ");
  blk("  decision variables, e.g., DESIRED_SPEED etc. In general, if   ");
  blk("  foobar is a decision variable, DESIRED_FOOBAR may be posted   ");
  blk("  if there is a behavior reasoning about that decision variable.");
  blk("  +                                                             ");
  blk("  BHV_ERROR             = Error description gen'ed by behavior  ");
  blk("  BHV_IPF               = Serialization of an IvP function      ");
  blk("  BHV_WARNING           = Warning description gen'ed by behavior");
  blk("                                                                ");
  blk("  IVPHELM_STATE         = PARK, DRIVE, STANDBY, DISABLED        ");
  blk("  IVPHELM_ALLSTOP       = clear, MissingDecVars, BehaviorError  ");
  blk("                          NothingToDo                           ");
  blk("  IVPHELM_BHV_CNT       = # of behaviors instantiated           ");
  blk("  IVPHELM_BHV_CNT_EVER  = # of behaviors instantiated ever      ");
  blk("  IVPHELM_IPF_CNT       = # of IvP functions on this iteration  ");
  blk("                                                                ");
  blk("  IVPHELM_CREATE_CPU    = CPU time to create IvP functions      ");
  blk("  IVPHELM_LOOP_CPU      = CPU time to create and solve IvP prob ");
  blk("                                                                ");
  blk("  IVPHELM_DOMAIN        = speed,0,4,21:course,0,359,36          ");
  blk("  IVPHELM_LIFE_EVENT    = Desc of behavior spawn or death       ");
  blk("  IVPHELM_MODESET       = Description of the helm mode tree     ");
  blk("  IVPHELM_ITER          = The Present helm iteration (integer)  ");
  blk("  IVPHELM_STATEVARS     = MOOS vars involved in mode setting    ");
  blk("  IVPHELM_UPDATEVARS    = MOOS vars involved in behavior updates");
  blk("  IVPHELM_UPDATE_RESULT = Report on attempted behavior update   ");
  blk("  IVPHELM_SUMMARY       = A helm snapshot for use in uHelmScope ");
  blk("  IVPHELM_RESTARTED     = true when/if helm is RE-started       ");
  blk("  PLOGGER_CMD           = Request pLogger to copy the bhv file  ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pHelmIvP", "gpl");
  exit(0);
}









