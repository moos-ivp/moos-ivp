/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MVS_GUI.cpp                                          */
/*    DATE: May 14th, 2005                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "MVS_GUI.h"
#include "MBUtils.h"
#include "IO_Utilities.h"
#include "IvPFunction.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

MVS_GUI::MVS_GUI(int g_w, int g_h, const char *g_l)
  : MV_GUI(g_w, g_h, g_l) {

  mbar->add("Step/  1 Steps",  ' ', (Fl_Callback*)MVS_GUI::cb_AddSteps, (void*)1);
  mbar->add("Step/  5 Steps", 'F',  (Fl_Callback*)MVS_GUI::cb_AddSteps, (void*)5);
  mbar->add("Step/ 10 Steps",  'T', (Fl_Callback*)MVS_GUI::cb_AddSteps, (void*)10);
  mbar->add("Step/ 50 Steps",  'g', (Fl_Callback*)MVS_GUI::cb_AddSteps, (void*)50);
  mbar->add("Step/100 Steps",  'G', (Fl_Callback*)MVS_GUI::cb_AddSteps, (void*)100);
  mbar->add("Step/200 Steps",  'h', (Fl_Callback*)MVS_GUI::cb_AddSteps, (void*)200);
  mbar->add("Step/300 Steps",  'H', (Fl_Callback*)MVS_GUI::cb_AddSteps, (void*)300); 
  mbar->add("Step/PAUSE", 'p', (Fl_Callback*)MVS_GUI::cb_Pause, 0);
  mbar->add("Step/RESET", 'R', (Fl_Callback*)MVS_GUI::cb_Reset, 0);

  mbar->add("Functions/All", 'v', (Fl_Callback*)MVS_GUI::cb_Images, 0,
	    FL_MENU_RADIO|FL_MENU_VALUE);
  mbar->add("Functions/Active Only", 'v', (Fl_Callback*)MVS_GUI::cb_Images, 0,
	    FL_MENU_RADIO);
  mbar->add("Functions/None",  'v', (Fl_Callback*)MVS_GUI::cb_Images, 0,
	    FL_MENU_RADIO|FL_MENU_DIVIDER);

  mbar->add("Functions/Less IMG Resolution", 'w', (Fl_Callback*)::MVS_GUI::cb_ImageRes, 
	    (void*)+1, 0);
  mbar->add("Functions/More IMG Resolution", 'q', (Fl_Callback*)::MVS_GUI::cb_ImageRes, 
	    (void*)-1, FL_MENU_DIVIDER);
  mbar->add("Functions/Less IMG Radius", 'a', (Fl_Callback*)::MVS_GUI::cb_ImageRadius, 
	    (void*)+1, 0); 
  mbar->add("Functions/More IMG Radius", 's', (Fl_Callback*)::MVS_GUI::cb_ImageRadius, 
	    (void*)-1, 0); 
  mbar->add("Functions/Save To Disk", FL_CTRL+'s', (Fl_Callback*)::MVS_GUI::cb_FunctionSave, 
	    (void*)0, 0); 

  mbar->add("Poke/Heading++",  FL_SHIFT+FL_Right, (Fl_Callback*)MVS_GUI::cb_DeltaCrs,
	    (void*)1);
  mbar->add("Poke/Heading--",  FL_SHIFT+FL_Left, (Fl_Callback*)MVS_GUI::cb_DeltaCrs, 
	    (void*)-1);
  mbar->add("Poke/Speed++",  FL_SHIFT+FL_Up, (Fl_Callback*)MVS_GUI::cb_DeltaSpd,
	    (void*)1);
  mbar->add("Poke/Speed--",  FL_SHIFT+FL_Down, (Fl_Callback*)MVS_GUI::cb_DeltaSpd, 
	    (void*)-1);

  sim         = 0;
  info_buffer = 0;
  draw_ofs    = 2;  // 0:None  1:Active-Only  2:All
  steps_to_do = 0;
  last_active = 0;
  pause       = false;
  first_step  = true;
}

//----------------------------------------------------------
// Procedure: handle
//      Note: As it stands, this method could be eliminated entirely, and the 
//            default behavior of the parent class should work fine. But if
//            we want to tinker with event handling, this method is the place.

int MVS_GUI::handle(int event) 
{
  return(MV_GUI::handle(event));
}

//----------------------------------------------------------
// Procedure: conditional_step

void MVS_GUI::conditional_step() 
{
  if(!pause && (steps_to_do > 0)) {
    if(first_step) {
      step_decide();
      first_step = false;
    }
    step_act();
    step_decide();
    step_render();
    steps_to_do--;
  }

  int curr_mviewer_index = mviewer->getDataIndex(); 
  if(last_active != curr_mviewer_index) {
    handle_drawofs();
    last_active = curr_mviewer_index;
  }
}

//----------------------------------------------------------
// Procedure: step_decide()

void MVS_GUI::step_decide() 
{
  if(!sim) return;
  sim->clickDecide();
}  

//----------------------------------------------------------
// Procedure: step_act

void MVS_GUI::step_act() 
{
  if(!sim) return;
  sim->clickAct(); 
}

//----------------------------------------------------------
// Procedure: step_render

void MVS_GUI::step_render() 
{
  int i;

  if(!info_buffer)
    return;

  // Get current vehicle poses and update the marine viewer
  vector<string> vnames = info_buffer->getVehicleNames();
  for(i=0; i<vnames.size(); i++) {
    string v_name = vnames[i];
    if(v_name != "") {
      double v_x   = info_buffer->dQuery(v_name, "NAV_X");
      double v_y   = info_buffer->dQuery(v_name, "NAV_Y");
      double v_crs = info_buffer->dQuery(v_name, "NAV_HEADING");
      double v_spd = info_buffer->dQuery(v_name, "NAV_SPEED");
      mviewer->updateVehiclePosition(v_name.c_str(), v_x, v_y,
				     v_crs, v_spd);
    }
  }

  // Get the latest behavior info and update marine viewer
  for(i=0; i<sim->getHelmCount(); i++) {
    string bhv_info  = sim->getInfoBHV(i);
    vector<string> svector = chompString(bhv_info, ',');
    if(svector.size() == 2) {
      string helm_name = svector[0];
      string helm_info = svector[1];
      mviewer->setInfoBHV(helm_name, helm_info);
    }
  }
 
  // Get the latest grid update info and update marine viewer
  for(i=0; i<grid_labels.size(); i++) {
    string query = "GRID_DELTA_" + grid_labels[i];
    string result = info_buffer->sQuery("", query);
    if(result != "")
      mviewer->updateGrid(result);
  }


  handle_drawofs();
  mviewer->redraw();
  mviewer->setTime(sim->getCurrTime());
  updateXY();
  //  updateInfoBHV();
  Fl::flush();
}

//----------------------------------------------------------
// Procedure: handle_drawofs
//      Note: draw_ofs: 0=None  1=Active-Only  2=All
//      Note: data_index is the index of the "active" vehicle
//            according to the marine viewer.

void MVS_GUI::handle_drawofs()
{
  mviewer->clearImages();
  if(draw_ofs != 0) {
    vector<IMG*> images;
    if(draw_ofs == 1) {
      int data_index = mviewer->getDataIndex();
      images = sim->getOneImageSet(data_index);
    }
    else
      images = sim->getAllImages();
    for(int j=0; j<images.size(); j++)
      mviewer->addImage(*(images[j]));
  }
  mviewer->redraw();
}
    

//----------------------------------------- Alt Image Resolution
inline void MVS_GUI::cb_ImageRes_i(int delta) {
  mviewer->alterImageRes((double)delta * 0.01);
}
void MVS_GUI::cb_ImageRes(Fl_Widget* o, int v) {
  ((MVS_GUI*)(o->parent()->user_data()))->cb_ImageRes_i(v);
}

//----------------------------------------- Alt Image Radius
inline void MVS_GUI::cb_ImageRadius_i(int delta) {
  mviewer->alterImageRadius(delta);
}
void MVS_GUI::cb_ImageRadius(Fl_Widget* o, int v) {
  ((MVS_GUI*)(o->parent()->user_data()))->cb_ImageRadius_i(v);
}

//-----------------------------------------   AddSteps
void MVS_GUI::cb_AddSteps(Fl_Widget* o, int v) {
  ((MVS_GUI*)(o->parent()->user_data()))->add_steps(v);
}


//-----------------------------------------   Images
// draw_ofs:  0:None  1:Active-Only  2:All

inline void MVS_GUI::cb_Images_i() {
  draw_ofs++;
  if(draw_ofs > 2)
    draw_ofs = 0;
  handle_drawofs();
}
void MVS_GUI::cb_Images(Fl_Widget* o) {
  ((MVS_GUI*)(o->parent()->user_data()))->cb_Images_i();
}

//-----------------------------------------   FunctionSave

inline void MVS_GUI::cb_FunctionSave_i() 
{
  int data_index = mviewer->getDataIndex();
  IvPFunction* of = sim->getOF(data_index);
  if(of)
    saveFunction(of, "foo.ipf", false);
}
void MVS_GUI::cb_FunctionSave(Fl_Widget* o) {
  ((MVS_GUI*)(o->parent()->user_data()))->cb_FunctionSave_i();
}

//----------------------------------------- DeltaCrs
inline void MVS_GUI::cb_DeltaCrs_i(int delta) {
  int data_index = mviewer->getDataIndex();
  sim->headingDelta(data_index, delta);
  step_decide();
  step_render();
}
void MVS_GUI::cb_DeltaCrs(Fl_Widget* o, int v) {
  ((MVS_GUI*)(o->parent()->user_data()))->cb_DeltaCrs_i(v);
}

//----------------------------------------- DeltaSpd
inline void MVS_GUI::cb_DeltaSpd_i(int delta) {
  int data_index = mviewer->getDataIndex();
  sim->speedDelta(data_index, delta);
  step_decide();
  step_render();
}
void MVS_GUI::cb_DeltaSpd(Fl_Widget* o, int v) {
  ((MVS_GUI*)(o->parent()->user_data()))->cb_DeltaSpd_i(v);
}

//-----------------------------------------    PAUSE
inline void MVS_GUI::cb_Pause_i() {
  if(pause==true)        // Don't pause if we're not actively
    pause=false;         // executing steps.
  else
    if(steps_to_do > 0)
      pause=true;
}
void MVS_GUI::cb_Pause(Fl_Widget* o) {
  ((MVS_GUI*)(o->parent()->user_data()))->cb_Pause_i();
}

//-----------------------------------------    Reset
inline void MVS_GUI::cb_Reset_i() {
  sim->resetInfoBuffer();
  mviewer->resetVehicles();
  step_render();
}
void MVS_GUI::cb_Reset(Fl_Widget* o) {
  ((MVS_GUI*)(o->parent()->user_data()))->cb_Reset_i();
}

