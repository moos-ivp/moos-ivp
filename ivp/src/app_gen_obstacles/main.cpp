/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
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

#include <iostream>
#include "MBUtils.h"
#include "ObstacleFieldGenerator.h"
#include "ObstacleFieldGenerator_Info.h"

using namespace std;

int main(int argc, char *argv[])
{
  ObstacleFieldGenerator generator;

  string arg_summary = argv[0];

  for(int i=1; i<argc; i++) {
    bool   handled = false;

    string argi = argv[i];
    arg_summary += " " + argi;

    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      showReleaseInfoAndExit();
    else if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();

    else if(strBegins(argi, "--amt=")) 
      handled = generator.setAmount(argi.substr(6));
    else if(strBegins(argi, "--min_range="))
      handled = generator.setMinRange(argi.substr(12));
    else if(strBegins(argi, "--min_size="))
      handled = generator.setObstacleMinSize(argi.substr(11));
    else if(strBegins(argi, "--max_size="))
      handled = generator.setObstacleMaxSize(argi.substr(11));
    else if(strBegins(argi, "--meter"))
      handled = generator.setPrecision(1);
    else if(strBegins(argi, "--poly="))
      handled = generator.setPolygon(argi.substr(7));
    
    if(!handled) {
      cout << "Unhandled arg: " << argi << endl;
      return(0);
    }
  }
  
  cout << "# " << arg_summary << endl;

  bool ok = generator.generate();
  if(!ok)
    return(1);
  return(0);
}

