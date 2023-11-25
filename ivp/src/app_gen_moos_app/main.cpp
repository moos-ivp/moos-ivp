/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: March 23rd 2021                                      */
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
#include "AppGenerator.h"
#include "AppGenerator_Info.h"

using namespace std;

int main(int argc, char *argv[])
{
  AppGenerator generator;

  
  for(int i=1; i<argc; i++) {
    bool handled = true;

    string argi = argv[i];

    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      showReleaseInfoAndExit();
    else if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();

    if((argi=="p") || (argi=="i") || (argi=="u") ||
       (argi=="uFld") || (argi=="uSim"))
      generator.setPrefix(argi);
    
    else if(strBegins(argi, "--org="))
      generator.setOrg(argi.substr(6));
    else if(strBegins(argi, "--name="))
      generator.setName(argi.substr(7));
    else if(strBegins(argi, "--date="))
      generator.setDate(argi.substr(7));
    else if(strBegins(argi, "--body="))
      generator.setBody(argi.substr(7));
    else
      handled = false;
    
    if(!handled) {
      cout << "Unhandled arg: " << argi << endl;
      return(0);
    }
  }
  
  generator.generate();
  return(0);
}










