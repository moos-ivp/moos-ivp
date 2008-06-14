/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: Apr 15th 2005                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <time.h>
#include "GUI.h"
#include "AOF_Rings.h"
#include "MBUtils.h"
#include "IO_Utilities.h"
#include "PopulatorAOF.h"

using namespace std;

void pause(int nMS)
{
  timespec TimeSpec;
  TimeSpec.tv_sec 	= nMS / 1000;
  TimeSpec.tv_nsec 	= (nMS%1000) *1000000;
  
  nanosleep(&TimeSpec,NULL);
}


//--------------------------------------------------------
// Procedure: idleProc

void idleProc(void *)
{
  Fl::flush();
  pause(10);
}

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  int i;
  AOF *aof = 0;

  //-------------------------------------------- Handle AOF Files
  string aof_file;
  string frame_color;
  string clear_color;
  for(i=1; i<argc; i++) {
    string argi = argv[i];
    if(strContains(argv[i], ".aof"))
      aof_file = argv[i];
    if(strContains(argv[i], "frame_color="))
      frame_color = tokStringParse(argi, "frame_color", '#', '=');
    if(strContains(argv[i], "clear_color="))
      clear_color = tokStringParse(argi, "clear_color", '#', '=');
    if(strContains(argv[i], "back_color="))
      clear_color = tokStringParse(argi, "back_color", '#', '=');
  }

  if(aof_file != "") {
    PopulatorAOF populator;
    aof = populator.populate(aof_file);
  }
  
  //GUI *gui  = new GUI(1200, 900, "aof-ipf-function-viewer");
  GUI *gui  = new GUI(1000, 900, "aof-ipf-function-viewer");

  if(aof)
    gui->setAOF(aof);

  if(frame_color != "")
    gui->viewer->setParam("frame_color", frame_color);
  if(clear_color != "")
    gui->viewer->setParam("clear_color", clear_color);

  return Fl::run();
}
