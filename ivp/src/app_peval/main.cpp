/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: Mar 4th, 2006                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "Launcher.h"

using namespace std;

void help_message();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  int res = getArg(argc, argv, 0, "-h", "--help");
  if(res) {
    help_message();
    return(0);
  }

  Launcher launcher;
  for(int i=1; i<argc; i++)
    launcher.handleParam(argv[i]);

  launcher.launch();
}


//--------------------------------------------------------
// Procedure: help_message()

void help_message()
{
  cout << endl;
  cout << "Usage: peval file1.slog [file2.slog] [-h] [--help] " << endl;
  cout << "   At least one .slog file must be provided " << endl;
  cout << endl;
}





