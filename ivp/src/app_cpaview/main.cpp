/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: Feb 12, 2007                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <vector>
#include <iostream>
#include "CPA_GUI.h"
#include "MBUtils.h"

using namespace std;

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
  int i, j;
  Fl::add_idle(idleProc);

  CPA_GUI* gui = new CPA_GUI(1500, 800, "CPAViewer");

  //string tif_file = "Pianosa.tif";  // default
  string tif_file = "AerialMIT.tif";  // default
  
  for(i=1; i<argc; i++) {
    string argi  = argv[i];
    
    if(strContains(argi, ".tif"))
      tif_file = argi;
    else if(argi == "-noimg")
      tif_file = "";
  }
  
  gui->readTiff(tif_file.c_str());
  gui->updateXY();
  
  return Fl::run();
}
