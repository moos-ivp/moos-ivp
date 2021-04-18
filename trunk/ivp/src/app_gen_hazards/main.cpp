/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Jan 27th 2012                                        */
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
#include "HazardFieldGenerator.h"
#include "HazardFieldGenerator_Info.h"

using namespace std;

int main(int argc, char *argv[])
{
  HazardFieldGenerator generator;

  string arg_summary = argv[0];
  
  for(int i=1; i<argc; i++) {
    bool   handled = false;

    string argi = argv[i];
    arg_summary += " " + argi;

    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      showReleaseInfoAndExit();
    else if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();

    else if(strBegins(argi, "--exp="))
      handled = generator.setResemblanceExp(argi.substr(6));
    else if(strBegins(argi, "--aspect_base=")) 
      handled = generator.setAspectBase(argi.substr(14));
    else if(strBegins(argi, "--aspect_range=")) 
      handled = generator.setAspectRange(argi.substr(15));

    else if(strBegins(argi, "--aspect_min_base=")) 
      handled = generator.setAspectMinBase(argi.substr(18));
    else if(strBegins(argi, "--aspect_min_range=")) 
      handled = generator.setAspectMinRange(argi.substr(19));

    else if(strBegins(argi, "--aspect_max_base=")) 
      handled = generator.setAspectMaxBase(argi.substr(18));
    else if(strBegins(argi, "--aspect_max_range=")) 
      handled = generator.setAspectMaxRange(argi.substr(19));

    else if(strBegins(argi, "--objects="))
      handled = generator.addObjectSet(argi.substr(10));
    else if(strBegins(argi, "--polygon="))
      handled = generator.addPolygon(argi.substr(10));
    
    if(!handled) {
      cout << "Unhandled arg: " << argi << endl;
      return(0);
    }
  }
  
  cout << "// " << arg_summary << endl;

  generator.generate();
  return(0);
}









