/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: TermCommand_Info.cpp                                 */
/*    DATE: Aug 25th 2011                                        */
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
#include "BHV_Guide_Info.h"
#include "ColorParse.h"
#include "ReleaseInfo.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                 ");
  blk("------------------------------------                      ");
  blk("  The bweb is a terminal based tool for launching a web   ");
  blk("  browser to the officially supported web page for the    ");
  blk("  named IvP Helm behavior.                                ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blu("========================================================= ");
  blu("Usage: bweb BEHAVIOR_NAME [OPTIONS]                       ");
  blu("========================================================= ");
  blk("                                                          ");
  showSynopsis();
  blk("                                                          ");
  blk("Options:                                                  ");
  mag("  --help, -h                                              ");
  blk("      Display this help message.                          ");
  mag("  --version,-v                                            ");
  blk("      Display the release version of bweb.                ");
  blk("                                                          ");
  blk("Examples:                                                 ");
  blk("  $ bweb Waypoint                                         ");
  blk("  $ bweb Loiter                                           ");
  blk("                                                          ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit()

void showReleaseInfoAndExit()
{
  showReleaseInfo("bweb", "gpl");
  exit(0);
}










