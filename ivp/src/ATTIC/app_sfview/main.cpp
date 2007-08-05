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

  //-------------------------------------------- Handle IPF Files
  vector<string> ipf_files;
  for(i=1; i<argc; i++)
    if(strContains(argv[i], ".ipf"))
      ipf_files.push_back(argv[i]);

  vector<IvPFunction*> ipfs;
  for(i=0; i<ipf_files.size(); i++) {
    IvPFunction *ipf = readFunction(ipf_files[i]);
    ipfs.push_back(ipf);
    if(ipf == 0)
      cout << "NULL IPF!!!!" << endl;
    cout << "size of ipf: " << ipf->getPDMap()->size() << endl;
    ipf->getPDMap()->normalize(-100,200);
  }

  Fl::add_idle(idleProc);

  //GUI *gui  = new GUI(1200, 900, "aof-ipf-function-viewer");
  GUI *gui  = new GUI(800, 600, "aof-ipf-function-viewer");

  //-------------------------------------------- Handle AOF Files
  string aof_file;
  int counter=0;
  for(i=1; i<argc; i++) {
    if(strContains(argv[i], ".aof")) {
      aof_file = argv[i];
      PopulatorAOF populator;
      aof = populator.populate(aof_file);
      cout << "main has a valid AOF" << endl;
      gui->setAOF(aof, counter);
      counter++;
    }
  }

  for(i=0; i<ipfs.size(); i++)
    gui->addIPF(ipfs[i]);

  return Fl::run();
}
