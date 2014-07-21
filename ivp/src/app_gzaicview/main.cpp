/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: June 20th, 2005                                      */
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
#include <cstdlib>
#include "ZAIC_GUI.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"

using namespace std;

void help_message();

//--------------------------------------------------------
// Procedure: idleProc

void idleProc(void *)
{
  Fl::flush();
  millipause(10);
}

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  int res = 0;

  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("geoview", "gpl");
    return(0);
  }
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    help_message();
    return(0);
  }
  
  int domain = 410;
  if(scanArgs(argc, argv, "-d", "--domain")) {
    domain = atoi(argv[res]) - 191;
    if(domain < 100)
      domain = 100;
    if(domain > 1000)
      domain = 1000;   
  }

  Fl::add_idle(idleProc);
  ZAIC_GUI* gui = new ZAIC_GUI(domain+300, 450, "ZAIC-Viewer");

  gui->updateOutput();

  // Enter the GUI event loop.
  return Fl::run();
}


//--------------------------------------------------------
// Procedure: help_message()

void help_message()
{
  cout << endl;
  cout << "Usage: gzaicview                                  " << endl;
  cout << " Parameter     Defaults Description               " << endl;
  cout << " ----------    -------- ------------------------  " << endl;
  cout << " [-help]                Display this help message " << endl;
  cout << " [-d]          410      Set upper value of domain " << endl;
  cout << " [--domain]    410      Set upper value of domain " << endl;
  cout << endl;
}









