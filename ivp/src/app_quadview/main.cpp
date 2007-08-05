/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: June 20th, 2005                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "QUAD_GUI.h"
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

  Fl::add_idle(idleProc);
  QUAD_GUI* gui = new QUAD_GUI(900, 750, "Quadratic Viewer");

  gui->updateOutput();

  // Enter the GUI event loop.
  return Fl::run();
}

//--------------------------------------------------------
// Procedure: help_message()

void help_message()
{
  cout << endl;
  cout << "Usage: quadview                                   " << endl;
  cout << " Parameter     Defaults Description               " << endl;
  cout << " ----------    -------- ------------------------  " << endl;
  cout << " [-help]                Display this help message " << endl;
  cout << endl;
}




