/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Oct 29th 2022                                        */
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

#include <cstring>
#include <iostream>
#include "MBUtils.h"
#include "BHV_Guide.h"
#include "BHV_Guide_Info.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc ,char * argv[])
{
  string mission_file;
  string run_command  = argv[0];

  BHV_Guide bhv_guide;
  
  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    bool handled = false;
    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      showReleaseInfoAndExit();
    else if((argi == "-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();

    else 
      handled = bhv_guide.setBehaviorName(argi);

    if(!handled) {
      cout << "Bad arg:[" << argi << "]. Exit Code 1." << endl;
      return(1);
    }
  }

  bhv_guide.guide();
  
  return(0);
}











