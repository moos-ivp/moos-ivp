/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AppGenerator_Info.cpp                                */
/*    DATE: March 22rd 2021                                      */
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
#include "AppGenerator_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  moos_gen_app is a utility for generating ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  cout << "=======================================================" << endl;
  cout << "Usage: moos_gen_app [OPTIONS]                           " << endl;
  cout << "=======================================================" << endl;
  cout << "                                                       " << endl;
  showSynopsis();
  cout << "                                                       " << endl;
  cout << "Options:                                               " << endl;
  cout << "  --help, -h                                           " << endl;
  cout << "      Display this help message.                       " << endl;
  cout << "  --version,-v                                         " << endl;
  cout << "      Display the release version of gen_hazards.      " << endl;
  cout << "                                                       " << endl;
  cout << "Example:                                               " << endl;
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("moos_gen_app", "gpl");
  exit(0);
}
  









