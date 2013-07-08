/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: Feb 12, 2007                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <cstdlib>
#include <vector>
#include <iostream>
#include <FL/Fl.H>
#include "BNG_GUI.h"
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
  Fl::add_idle(idleProc);

  int gui_wid = 0.90 * Fl::w();
  int gui_hgt = 0.85 * Fl::h();
  BNG_GUI* gui = new BNG_GUI(gui_wid, gui_hgt, "MIT BNGViewer");
  
  if(!gui)
    return(0);

  //string tif_file = "Pianosa.tif";  // default
  string tif_file = "AerialMIT.tif";  // default

  gui->m_pair_model->set_osx(0);
  gui->m_pair_model->set_osy(-100);
  gui->m_pair_model->set_osh(0);
  gui->m_pair_model->set_osv(2.4);

  gui->m_pair_model->set_cnx(-80);
  gui->m_pair_model->set_cny(-75);
  gui->m_pair_model->set_cnh(45);
  gui->m_pair_model->set_cnv(1.8);
  
  int i;
  for(i=1; i<argc; i++) {
    string argi  = argv[i];

    string argi_copy  = argv[i];
    string argi_value = biteString(argi_copy, '=');
    double dval = atof(argi_value.c_str());
    
    if(strBegins(argi, "--file="))
      gui->m_pair_model->initFromFile(argi_value);
    
    else if(strBegins(argi, "--osx=")) 
      gui->m_pair_model->set_osx(dval);
    else if(strBegins(argi, "--osy=")) 
      gui->m_pair_model->set_osy(dval);
    else if(strBegins(argi, "--osh="))
      gui->m_pair_model->set_osh(dval);
    else if(strBegins(argi, "--osv="))
      gui->m_pair_model->set_osv(dval);
    
    else if(strBegins(argi, "--cnx=")) 
      gui->m_pair_model->set_cnx(dval);
    else if(strBegins(argi, "--cny=")) 
      gui->m_pair_model->set_cny(dval);
    else if(strBegins(argi, "--cnh="))
      gui->m_pair_model->set_cnh(dval);
    else if(strBegins(argi, "--cnv="))
      gui->m_pair_model->set_cnv(dval);

    else if(strContains(argi, ".tif"))
      tif_file = argi;
    else if(argi == "-noimg")
      tif_file = "";
  }
  
  gui->m_bng_viewer->setParam("tiff_file", tif_file);
  gui->m_bng_viewer->setCenterView();
  gui->m_ipf_viewer->resetIPF(1000);
  gui->updateXY();
  
  return Fl::run();
}


