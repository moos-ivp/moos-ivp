/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardFieldGenerator_Info.cpp                        */
/*    DATE: Apr 20th 2013                                        */
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
#include "HazardFieldGenerator_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  gen_hazards is a utility for generating a hazard file, i.e., a");
  blk("  simulated threat laydown. Objects will be generated within a  ");
  blk("  given convex polygon. Generated objects are either hazards    ");
  blk("  or benign objects.                                            "); 
  blk("      Benign objects may have a resemblance factor associated   ");
  blk("  with them. This represents how similar a benign object may    "); 
  blk("  appear to a hazardous object.                                 "); 
  blk("      Objects may also have an optimal sensing aspect specified."); 
  blk("  This represents the best approach angle for a vehicle to      ");
  blk("  optimize its sensor performance. If the optimal aspect is     ");
  blk("  specified, a sensitivity factor is also given, ssigma.         "); 
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  cout << "=======================================================" << endl;
  cout << "Usage: gen_hazards [OPTIONS]                           " << endl;
  cout << "=======================================================" << endl;
  cout << "                                                       " << endl;
  showSynopsis();
  cout << "                                                       " << endl;
  cout << "Options:                                               " << endl;
  cout << "  --help, -h                                           " << endl;
  cout << "      Display this help message.                       " << endl;
  cout << "  --version,-v                                         " << endl;
  cout << "      Display the release version of gen_hazards.      " << endl;
  cout << "  --objects=<object_set>                               " << endl;
  cout << "      Specify an object set of the form:               " << endl;
  cout << "      \"amount,type\"                                  " << endl;
  cout << "  --exp=<value>                                        " << endl;
  cout << "      hazard resemblance exponent [.01,10]             " << endl;
  cout << "  --aspect_base=<value>                                " << endl;
  cout << "      aspect base angle [0,360)                        " << endl;
  cout << "  --aspect_range=<value>                               " << endl;
  cout << "      aspect base angle [0,360)                        " << endl;
  cout << "  --aspect_ssigma_max=<value>                          " << endl;
  cout << "      maximum 2-sigma aspect factor [0,inf]            " << endl;
  cout << "  --polygon=<poly>                                     " << endl;
  cout << "      Specify a polygon region of the form:            " << endl;
  cout << "      \"0,0 : 50,0 : 50,50 : 0,50\"                    " << endl;
  cout << "                                                       " << endl;
  cout << "Note: Aspect information will be generated if at least " << endl;
  cout << "      one of the three aspect parameters are given.    " << endl;
  cout << "      If only one is given defaults for the other      " << endl;
  cout << "      aspect parameters will be used:                  " << endl;
  cout << "                                                       " << endl;
  cout << "   --aspect_base  - the default is 0                   " << endl;
  cout << "   --aspect_range - the default is 90                  " << endl;
  cout << "   --aspect_ssigma_max - the default is 180            " << endl;
  cout << "                                                       " << endl;
  cout << "Example:                                               " << endl;
  cout << "  gen_hazards --polygon=-150,-75:-150,-400:400,-400:400,-75 --objects=4,hazard --objects=6,benign --aspect_base=45 --aspect_range=20 --aspect_min_base=10 --aspect_min_range=12 --aspect_max_base=40 --aspect_max_range=45" << endl;
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("gen_hazards", "gpl");
  exit(0);
}
  








