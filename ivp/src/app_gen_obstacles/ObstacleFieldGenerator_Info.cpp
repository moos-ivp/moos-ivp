/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ObstacleFieldGenerator_Info.cpp                      */
/*    DATE: Oct 18th 2017                                        */
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
#include "ObstacleFieldGenerator_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  gen_obstacles is a utility for generating a obstacle file,    ");
  blk("  a set of polygon obstacles, all guaranteed to be within the   ");
  blk("  user specified polygon region, none of them overlapping and   ");
  blk("  guaranteed to have a minimum separation range.                "); 
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  cout << "=======================================================" << endl;
  cout << "Usage: gen_obstacles [OPTIONS]                         " << endl;
  cout << "=======================================================" << endl;
  cout << "                                                       " << endl;
  showSynopsis();
  cout << "                                                       " << endl;
  cout << "Options:                                               " << endl;
  cout << "  --help, -h                                           " << endl;
  cout << "      Display this help message.                       " << endl;
  cout << "  --version,-v                                         " << endl;
  cout << "      Display the release version.                     " << endl;
  cout << "  --poly=<poly>                                        " << endl;
  cout << "      Specify a polygon region of the form:            " << endl;
  cout << "      \"0,0 : 50,0 : 50,50 : 0,50\"                    " << endl;
  cout << "  --meter                                              " << endl;
  cout << "      Round the obstacle vertices to nearest meter.    " << endl;
  cout << "      The default is the nearest 1/10th meter.         " << endl;
  cout << "  --min_range=<meters>                                 " << endl;
  cout << "      The minimum range between randomly generated     " << endl;
  cout << "      The default is 2 meters.                         " << endl;
  cout << "  --min_size=<meters>                                  " << endl;
  cout << "      The minimum radius of generated polygons.        " << endl;
  cout << "      The default is 6 meters.                         " << endl;
  cout << "  --max_size=<meters>                                  " << endl;
  cout << "      The maximum radius of generated polygons.        " << endl;
  cout << "      The default is 10 meters.                        " << endl;
  cout << "  --amt=<# of obstacles>                               " << endl;
  cout << "      The number of random polygon obstacles to        " << endl;
  cout << "      generate. The default is 1.                      " << endl;
  cout << "                                                       " << endl;
  cout << "Example:                                               " << endl;
  cout << "  gen_obstacles --poly=-50,-30:-50,-160:190,-160:190,-30 --amt=5 --min_size=2 --max_size=5 " << endl;
  cout << "  gen_obstacles --poly=30,-20:30,-140:120,-140:120,-20 --amt=5 --min_size=2 --max_size=5 " << endl;
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("gen_obstacles", "gpl");
  exit(0);
}
  

