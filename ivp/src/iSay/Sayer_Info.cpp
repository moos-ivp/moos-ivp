/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Sayer_Info.h                                         */
/*    DATE: May 20th, 2013                                       */
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
#include "Sayer_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  The iSay application is a way to invoke the native speech     ");
  blk("  generation utilities of OSX (the say command) and GNU/Linux   ");
  blk("  the espeak command). It may also invoke the afplay command    ");
  blk("  available in both OSX and GNU/Linux to play a given wav or    ");
  blk("  mp3 file.                                                     ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: iSay file.moos [OPTIONS]                                 ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch iSay with the given process name other than iSay.  ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --version,-v                                                  ");
  blk("      Display the release version of iSay.                      ");
  mag("  --web,-w                                                      ");
  blk("      Open browser to: https://oceanai.mit.edu/apps/iSay        ");
  blk("                                                                ");
  blk("Note: If argv[2] does not otherwise match a known option,       ");
  blk("      then it will be interpreted as a run alias. This is       ");
  blk("      to support pAntler launching conventions.                 ");
  blk("                                                                ");
  blk("$ uPokeDB SAY_MOOS=\"say={Would you like to play a game?}, rate=200\" ");
  blk("$ uPokeDB SAY_MOOS=\"file=hal_truth.wav\"                       ");
  blk("                                                                ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("iSay Example MOOS Configuration                                 ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = iSay                                            ");
  blk("{                                                               ");
  blk("  AppTick   = 4                                                 ");
  blk("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  default_voice      = alex       // iSay --voices for others   ");
  blk("  default_rate       = 200                                      ");
  blk("  interval_policy    = from_end   // or from_start              ");
  blk("  min_utter_interval = 1                                        ");
  blk("                                                                ");
  blk("  os_mode            = linux      // Default is \"osx\"         ");
  blk("  volume             = normal {mute,vsoft,soft,loud,vloud}      ");
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
  blu("iSay INTERFACE                                                  ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  SAY_MOOS   = say={Would you like to play a game?}, rate=200   ");
  blk("  SAY_MOOS   = file=file.wav                                    ");
  blk("                                                                ");
  blk("  SAY_VOLUME = soft   // Options: mute,vsoft,soft,normal,loud,  ");
  blk("                      // vloud,softer,louder, or number [0,2].  ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  APPCAST    iSay is an appcasting MOOS app.                    ");
  blk("                                                                ");
  blk("  Publishes to the system command.                              ");
  blk("  OSX: say Hello                                                ");
  blk("  OSX: afplay file.wav                                          ");
  blk("  LINUX: espeak Hello                                           ");
  blk("  LINUX: aplay file.wav                                         ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("iSay", "gpl");
  exit(0);
}






