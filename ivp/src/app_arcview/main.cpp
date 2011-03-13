/*****************************************************************/
/*    NAME: Michael  Benjamin                                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: June 20th, 2005                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "ARC_GUI.h"
#include "MBUtils.h"

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

  // Look for a request for help
  res = getArg(argc, argv, 0, "-h", "--help");
  if(res) {
    help_message();
    return(0);
  }

  int summits = 1;
  res = getArg(argc, argv, 0, "-2", "--two-summits");
  if(res) {
    summits = 2;
    return(0);
  }
  
  int domain = 410;
  res = getArg(argc, argv, 1, "-d", "--domain");
  if(res) {
    domain = atoi(argv[res]) - 191;
    if(domain < 100)
      domain = 100;
    if(domain > 1000)
      domain = 1000;   
  }

  Fl::add_idle(idleProc);
  ARC_GUI* gui = new ARC_GUI(domain+300, 450, "ARC-Viewer");

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




