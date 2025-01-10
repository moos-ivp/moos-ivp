/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: main.cpp                                             */
/*    DATE: Feb 12, 2007                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/
  
#include <cstdlib> 
#include <vector>
#include <iostream>
#include <FL/Fl.H>
#include "PAIR_GUI.h"
#include "MBUtils.h"

using namespace std;

void showHelpAndExit();

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
  Fl::add_idle(idleProc);

  PairModel pair_model;
  pair_model.set_osx(180);
  pair_model.set_osy(-250);
  pair_model.set_osh(8);
  pair_model.set_osv(0.9);

  pair_model.set_cnx(175);
  pair_model.set_cny(-100);
  pair_model.set_cnh(170);
  pair_model.set_cnv(0.9);

  //string tif_file = "MIT_SP.tif";
  string tif_file = "AerialMIT.tif";
  
  for(int i=1; i<argc; i++) { 
    string argi  = argv[i];

    if(strBegins(argi, "--file="))
      pair_model.initFromFile(argi.substr(7).c_str());
    
    else if(strBegins(argi, "--osx=")) 
      pair_model.set_osx(atof(argi.substr(6).c_str()));
    else if(strBegins(argi, "--osy=")) 
      pair_model.set_osy(atof(argi.substr(6).c_str()));
    else if(strBegins(argi, "--osh="))
      pair_model.set_osh(atof(argi.substr(6).c_str()));
    else if(strBegins(argi, "--osv="))
      pair_model.set_osv(atof(argi.substr(6).c_str()));
    
    else if(strBegins(argi, "--cnx=")) 
      pair_model.set_cnx(atof(argi.substr(6).c_str()));
    else if(strBegins(argi, "--cny=")) 
      pair_model.set_cny(atof(argi.substr(6).c_str()));
    else if(strBegins(argi, "--cnh="))
      pair_model.set_cnh(atof(argi.substr(6).c_str()));
    else if(strBegins(argi, "--cnv="))
      pair_model.set_cnv(atof(argi.substr(6).c_str()));

    else if(strBegins(argi, "--alert_rng="))
      pair_model.set_alert_range(atof(argi.substr(12).c_str()));
    else if(strBegins(argi, "--warn_time="))
      pair_model.set_warning_time(atof(argi.substr(12).c_str()));
    
    else if(strEnds(argi, ".tif"))
      tif_file = argi;
    else if(argi == "-noimg")
      tif_file = "";
    else if((argi == "-h") || (argi == "--help"))
      showHelpAndExit();
  }

  PAIR_GUI* gui = new PAIR_GUI(950, 700, "MIT PairViewer 2025");
  if(!gui)
    return(0);

  gui->setPairModel(pair_model);
  
  gui->m_pair_viewer->setParam("tiff_file", tif_file);

  gui->updateXY();
  gui->m_pair_viewer->draw();
  
  return Fl::run();
}


//--------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{
  cout << "Usage                                               " << endl;
  cout << "  pairview [OPTIONS] image.tif                       " << endl;
  cout << "                                                    " << endl;
  cout << "Synopsis:                                           " << endl;
  cout << "  The pairview utility renders two ships, ownship and" << endl;
  cout << "  contact, and allows the user to manipulate both   " << endl;
  cout << "  and observe properties of their relative position " << endl;
  cout << "  that are key to the     " << endl;
  cout << "                                                    " << endl;
  cout << "Options:                                            " << endl;
  cout << "  -h,--help      Displays this help message         " << endl;
  cout << "                                                    " << endl;
  cout << "  --osx=<num>    Ownship X-position                 " << endl;
  cout << "  --osy=<num>    Ownship Y-position                 " << endl;
  cout << "  --osh=<num>    Ownship Heading                    " << endl;
  cout << "  --osv=<num>    Ownship Velocity/Speed             " << endl;
  cout << "                                                    " << endl;
  cout << "  --cnx=<num>    Contact X-position                 " << endl;
  cout << "  --cny=<num>    Contact Y-position                 " << endl;
  cout << "  --cnh=<num>    Contact Heading                    " << endl;
  cout << "  --cnv=<num>    Contact Velocity/Speed             " << endl;
  cout << "                                                    " << endl;
  cout << "  --noimg        Do not open with a background image" << endl;
  cout << "                                                    " << endl;
  cout << "Examples:                                           " << endl;
  cout << "  pairview --osx=200 --osy=23.2 --osh=43 --osv=2.3  " << endl;
  cout << "           --cnx=-24 --cny=117  --cnh=18 --cnv=4.1  " << endl;
  exit(0);  
}
  






