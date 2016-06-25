/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: GUI.cpp                                              */
/*    DATE: Apr 15th 2005                                        */
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
#include "GUI.h"
#include "MBUtils.h"

using namespace std;

//--------------------------------------------------------------
// Constructor

GUI::GUI(int wid, int hgt, const char *label)
  : Fl_Window(wid, hgt, label) 
{
  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  m_menubar = new Fl_Menu_Bar(0, 0, w(), 25);
  augmentMenu();
    
  int info_size=10;

  viewer = new Viewer(0, 30, w(), h()-125);

  p_samp_count = new Fl_Output(60, h()-90, 50, 20, "Samples:"); 
  p_samp_count->textsize(info_size); 
  p_samp_count->labelsize(info_size);
  p_samp_count->clear_visible_focus();
  
  p_worst_err = new Fl_Output(60, h()-60, 50, 20, "WST-ERR:"); 
  p_worst_err->textsize(info_size); 
  p_worst_err->labelsize(info_size);
  p_worst_err->clear_visible_focus();

  p_avg_err = new Fl_Output(200, h()-90, 50, 20, "AVG-ERR:"); 
  p_avg_err->textsize(info_size); 
  p_avg_err->labelsize(info_size);
  p_avg_err->clear_visible_focus();

  p_square_err = new Fl_Output(200, h()-60, 50, 20, "SQR-ERR:"); 
  p_square_err->textsize(info_size); 
  p_square_err->labelsize(info_size);
  p_square_err->clear_visible_focus();

  p_samp_high = new Fl_Output(360, h()-90, 50, 20, "Sample High:"); 
  p_samp_high->textsize(info_size); 
  p_samp_high->labelsize(info_size);
  p_samp_high->clear_visible_focus();

  p_samp_low = new Fl_Output(360, h()-60, 50, 20, "Sample Low:"); 
  p_samp_low->textsize(info_size); 
  p_samp_low->labelsize(info_size);
  p_samp_low->clear_visible_focus();

  p_piece_count = new Fl_Output(60, h()-30, 50, 20, "Pieces:"); 
  p_piece_count->textsize(info_size); 
  p_piece_count->labelsize(info_size);
  p_piece_count->clear_visible_focus();

  p_unif_aug_size = new Fl_Output(200, h()-30, 50, 20, "Unif-Aug:"); 
  p_unif_aug_size->textsize(info_size); 
  p_unif_aug_size->labelsize(info_size);
  p_unif_aug_size->clear_visible_focus();

  p_create_time = new Fl_Output(360, h()-30, 50, 20, "Create-Time:"); 
  p_create_time->textsize(info_size); 
  p_create_time->labelsize(info_size);
  p_create_time->clear_visible_focus();

  p_uniform_str = new Fl_Output(530, h()-90, 180, 20, "Uniform Piece:"); 
  p_uniform_str->textsize(info_size); 
  p_uniform_str->labelsize(info_size);
  p_uniform_str->clear_visible_focus();
  
  p_auto_peak = new Fl_Output(780, h()-90, 180, 20, "AutoPeak:"); 
  p_auto_peak->textsize(info_size); 
  p_auto_peak->labelsize(info_size);
  p_auto_peak->clear_visible_focus();
  
  p_refine_reg_str = new Fl_Output(530, h()-60, 180, 20, "Refine Region:"); 
  p_refine_reg_str->textsize(info_size); 
  p_refine_reg_str->labelsize(info_size);
  p_refine_reg_str->clear_visible_focus();
  
  p_refine_pce_str = new Fl_Output(780, h()-60, 180, 20, "Refine Piece:"); 
  p_refine_pce_str->textsize(info_size); 
  p_refine_pce_str->labelsize(info_size);
  p_refine_pce_str->clear_visible_focus();
  //i_refine_pce_str->callback((Fl_Callback*)GUI::cb_set_refine_pce);

  p_reflector_errors  = new Fl_Output(530, h()-30, 430, 20, "Reflector Errors:"); 
  p_reflector_errors->textsize(info_size); 
  p_reflector_errors->labelsize(info_size);
  p_reflector_errors->clear_visible_focus();

  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------------
// Procedure: augmentMenu()

void GUI::augmentMenu()
{
  //===================================================================
  // File    Pull-Down Menu
  //===================================================================
  m_menubar->add("File/Script", 0,
		 (Fl_Callback*)GUI::cb_Script, 0, 0);
  m_menubar->add("File/Print Params", 'p',
		 (Fl_Callback*)GUI::cb_PrintParams, 0, 0);
  m_menubar->add("File/Quit ", FL_CTRL+'q',
		 (Fl_Callback*)GUI::cb_Quit, 0, 0);

  //===================================================================
  // RotateZoom    Pull-Down Menu
  //===================================================================
  m_menubar->add("RotateZoom/Rotate X- ", FL_Down,
		 (Fl_Callback*)GUI::cb_RotateX, (void*)-1, 0);
  m_menubar->add("RotateZoom/Rotate X+ ", FL_Up,
		 (Fl_Callback*)GUI::cb_RotateX, (void*)1, 0);
  m_menubar->add("RotateZoom/Rotate Z- ", FL_Left,
		 (Fl_Callback*)GUI::cb_RotateZ, (void*)-1, 0);
  m_menubar->add("RotateZoom/Rotate Z+ ", FL_Right,
		 (Fl_Callback*)GUI::cb_RotateZ, (void*)1, 0);
  m_menubar->add("RotateZoom/Reset1 ",       '1',
		 (Fl_Callback*)GUI::cb_Reset, (void*)1, 0);
  m_menubar->add("RotateZoom/Reset2 ",       '2',
		 (Fl_Callback*)GUI::cb_Reset, (void*)2, 0);
  m_menubar->add("RotateZoom/Reset3 ",       '3',
		 (Fl_Callback*)GUI::cb_Reset, (void*)3, 0);
  m_menubar->add("RotateZoom/Reset4 ",       '4',
		 (Fl_Callback*)GUI::cb_Reset, (void*)4, 0);
  m_menubar->add("RotateZoom/Toggle Frame ", 'f',
		 (Fl_Callback*)GUI::cb_ToggleFrame, (void*)-1, FL_MENU_DIVIDER);
  m_menubar->add("RotateZoom/Frame-Height-- ", FL_ALT+'f',
		 (Fl_Callback*)GUI::cb_FrameHeight, (void*)-1, 0);
  m_menubar->add("RotateZoom/Frame-Height++ ", FL_CTRL+'f',
		 (Fl_Callback*)GUI::cb_FrameHeight, (void*)1, FL_MENU_DIVIDER);
  m_menubar->add("RotateZoom/Zoom In",          'i',
		 (Fl_Callback*)GUI::cb_Zoom, (void*)-1, 0);
  m_menubar->add("RotateZoom/Zoom Out",         'o',
		 (Fl_Callback*)GUI::cb_Zoom, (void*)1, 0);
  m_menubar->add("RotateZoom/Zoom Reset",       'Z',
		 (Fl_Callback*)GUI::cb_Zoom, (void*)0, 0);
  
  //===================================================================
  // AOF    Pull-Down Menu
  //===================================================================
  m_menubar->add("AOF/Scale +",     'S',
		 (Fl_Callback*)GUI::cb_ModScale, (void*)+2, 0);
  m_menubar->add("AOF/Scale -",     's',
		 (Fl_Callback*)GUI::cb_ModScale, (void*)-2, 0);
  m_menubar->add("AOF/Base +",      'B',
		 (Fl_Callback*)GUI::cb_ModBaseAOF, (void*)+1, 0);
  m_menubar->add("AOF/Base -",      'b',
		 (Fl_Callback*)GUI::cb_ModBaseAOF, (void*)-1, 0);
  m_menubar->add("AOF/AOF Patch +", 'P',
		 (Fl_Callback*)GUI::cb_ModPatchAOF, (void*)+1, 0);
  m_menubar->add("AOF/AOF Patch -", 'p',
		 (Fl_Callback*)GUI::cb_ModPatchAOF, (void*)-1, 0);
  m_menubar->add("AOF/Toggle AOF",  'a',
		 (Fl_Callback*)GUI::cb_ToggleAOF, (void*)0, 0);

  //===================================================================
  // IPF    Pull-Down Menu
  //===================================================================
  m_menubar->add("IPF/Toggle IPF ", 'z',
		 (Fl_Callback*)GUI::cb_ToggleIPF,  (void*)0,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);
  m_menubar->add("IPF/Base +", 'e',
		 (Fl_Callback*)GUI::cb_ModBaseIPF, (void*)+10, 0);
  m_menubar->add("IPF/Base -", 'r',
		 (Fl_Callback*)GUI::cb_ModBaseIPF, (void*)-10, FL_MENU_DIVIDER);
  m_menubar->add("IPF/Toggle Strict", '`',
		 (Fl_Callback*)GUI::cb_ToggleStrict, (void*)0,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);
  m_menubar->add("IPF/Create   ", 'c',
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)0, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 1",   0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)1, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 2",   0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)2, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 3",   0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)3, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 4",   0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)4, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 5",   0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)5, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 7",   0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)7, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 10",  0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)10, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 15",  0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)15, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 20",  0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)20, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 25",  0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)25, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 30",  0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)30, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 40",  0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)40, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 50",  0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)50, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 75",  0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)75, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 100", 0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)100, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 250", 0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)250, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 500", 0,
		 (Fl_Callback*)GUI::cb_MakeUniform, (void*)500,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);
  m_menubar->add("IPF/Pieces 35", 0,
		 (Fl_Callback*)GUI::cb_MakePieces, (void*)35, FL_MENU_RADIO);
  m_menubar->add("IPF/Pieces 63", 0,
		 (Fl_Callback*)GUI::cb_MakePieces, (void*)63, FL_MENU_RADIO);
  m_menubar->add("IPF/Pieces 64", 0,
		 (Fl_Callback*)GUI::cb_MakePieces, (void*)64, FL_MENU_RADIO);
  m_menubar->add("IPF/Pieces 500", 0,
		 (Fl_Callback*)GUI::cb_MakePieces, (void*)500, FL_MENU_RADIO);
  m_menubar->add("IPF/Pieces 512", 0,
		 (Fl_Callback*)GUI::cb_MakePieces, (void*)512, FL_MENU_RADIO);
  m_menubar->add("IPF/Pieces 1000", 0,
		 (Fl_Callback*)GUI::cb_MakePieces, (void*)1000, FL_MENU_RADIO);
  m_menubar->add("IPF/Pieces 1024", 0,
		 (Fl_Callback*)GUI::cb_MakePieces, (void*)1024, FL_MENU_RADIO);
  m_menubar->add("IPF/Pieces 1025", 0,
		 (Fl_Callback*)GUI::cb_MakePieces, (void*)1025, FL_MENU_RADIO);
  m_menubar->add("IPF/Pieces 4000", 0,
		 (Fl_Callback*)GUI::cb_MakePieces, (void*)4000, FL_MENU_RADIO);

  //===================================================================
  // Color-Map    Pull-Down Menu
  //===================================================================  
  m_menubar->add("Color-Map/Default",   0,
		 (Fl_Callback*)GUI::cb_ColorMap,  (void*)1,
		 FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("Color-Map/Copper",    0,
		 (Fl_Callback*)GUI::cb_ColorMap,  (void*)2, FL_MENU_RADIO);
  m_menubar->add("Color-Map/Bone",      0,
		 (Fl_Callback*)GUI::cb_ColorMap,  (void*)3,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);
  m_menubar->add("Color-Map/Back-White", 0,
		 (Fl_Callback*)GUI::cb_ColorBack, (void*)0, 0);
  m_menubar->add("Color-Map/Back-Blue",  0,
		 (Fl_Callback*)GUI::cb_ColorBack, (void*)1, 0);
  m_menubar->add("Color-Map/Mac-Beige", 0,
		 (Fl_Callback*)GUI::cb_ColorBack, (void*)2, 0);
  
  //===================================================================
  // Directed-Refine    Pull-Down Menu
  //===================================================================
  m_menubar->add("Directed-Refine/Toggle Directed Refine",  'd',
		 (Fl_Callback*)GUI::cb_ToggleDirectedRefine, (void*)0,
		 FL_MENU_DIVIDER);
  m_menubar->add("Directed-Refine/Smaller-Uniform-Refine", '[',
		 (Fl_Callback*)GUI::cb_ModUniformAug, (void*)-1, 0);
  m_menubar->add("Directed-Refine/Larger-Uniform-Refine",  ']',
		 (Fl_Callback*)GUI::cb_ModUniformAug, (void*)1, 0);

  //===================================================================
  // Smart-Refine    Pull-Down Menu
  //===================================================================
  m_menubar->add("Smart-Refine/Toggle-Smart-Refine", 'm',
		 (Fl_Callback*)GUI::cb_ToggleSmartAug, (void*)0, 0);
  m_menubar->add("Smart-Refine/smart_amount=100",  0,
		 (Fl_Callback*)GUI::cb_SmartAugAmt, (void*)100, 0);
  m_menubar->add("Smart-Refine/smart_amount=200",  0,
		 (Fl_Callback*)GUI::cb_SmartAugAmt, (void*)200, 0);
  m_menubar->add("Smart-Refine/smart_amount=500",  0,
		 (Fl_Callback*)GUI::cb_SmartAugAmt, (void*)500, 0);
  m_menubar->add("Smart-Refine/smart_amount=750",  0,
		 (Fl_Callback*)GUI::cb_SmartAugAmt, (void*)750, 0);
  m_menubar->add("Smart-Refine/smart_amount=1000", 0,
		 (Fl_Callback*)GUI::cb_SmartAugAmt, (void*)1000, 0);
  m_menubar->add("Smart-Refine/smart_amount=2000", 0,
		 (Fl_Callback*)GUI::cb_SmartAugAmt, (void*)2000, 0);
  m_menubar->add("Smart-Refine/smart_amount=4000", 0,
		 (Fl_Callback*)GUI::cb_SmartAugAmt, (void*)4000, FL_MENU_DIVIDER);
  m_menubar->add("Smart-Refine/smart_percent= 10",  0,
		 (Fl_Callback*)GUI::cb_SmartAugPct, (void*)10, 0);
  m_menubar->add("Smart-Refine/smart_percent= 15",  0,
		 (Fl_Callback*)GUI::cb_SmartAugPct, (void*)15, 0);
  m_menubar->add("Smart-Refine/smart_percent= 25",  0,
		 (Fl_Callback*)GUI::cb_SmartAugPct, (void*)25, 0);
  m_menubar->add("Smart-Refine/smart_percent= 35",  0,
		 (Fl_Callback*)GUI::cb_SmartAugPct, (void*)35, 0);
  m_menubar->add("Smart-Refine/smart_percent= 50",  0,
		 (Fl_Callback*)GUI::cb_SmartAugPct, (void*)50, 0);
  m_menubar->add("Smart-Refine/smart_percent= 75",  0,
		 (Fl_Callback*)GUI::cb_SmartAugPct, (void*)75, 0);
  m_menubar->add("Smart-Refine/smart_percent= 100", 0,
		 (Fl_Callback*)GUI::cb_SmartAugPct, (void*)100, 0);

  //===================================================================
  // AutoPeak Pull-Down Menu
  //===================================================================
  m_menubar->add("AutoPeak/Toggle-AutoPeak", 't',
		 (Fl_Callback*)GUI::cb_ToggleAutoPeak, (void*)0, 0);
  
  //===================================================================
  // Analysis Pull-Down Menu
  //===================================================================
  m_menubar->add("Analysis/Sample-All",   0,
		 (Fl_Callback*)GUI::cb_Sample, (void*)0,     0);
  m_menubar->add("Analysis/Sample+50K", ' ',
		 (Fl_Callback*)GUI::cb_Sample, (void*)50000, 0);
  m_menubar->add("Analysis/Rebuildx10", 0,
		 (Fl_Callback*)GUI::cb_Rebuild, (void*)10, 0);
  m_menubar->add("Analysis/Rebuildx50", 0,
		 (Fl_Callback*)GUI::cb_Rebuild, (void*)50, 0);
  m_menubar->add("Analysis/Rebuildx100", 0,
		 (Fl_Callback*)GUI::cb_Rebuild, (void*)100, 0);
}

//----------------------------------------------------------
// Procedure: handle
//     Notes: We want the various "Output" widgets to ignore keyboard
//            events (as they should, right?!), so we wrote a MY_Output
//            subclass to do just that. However the keyboard arrow keys
//            still seem to be grabbed by Fl_Window to change focuse
//            between sub-widgets. We over-ride that here to do the 
//            panning on the image by invoking the pan callbacks. By
//            then returning (1), we've indicated that the event has
//            been handled.

int GUI::handle(int event) 
{
  switch(event) {
  case FL_KEYBOARD:
    if(Fl::event_key()==FL_Down) {
      cb_RotateX_i(-1);
      return(1); 
    }
    if(Fl::event_key()==FL_Up) {
      cb_RotateX_i(+1);
      return(1); 
    }
    if(Fl::event_key()==FL_Right) {
      cb_RotateZ_i(+1);
      return(1); 
    }
    if(Fl::event_key()==FL_Left) {
      cb_RotateZ_i(-1);
      return(1); 
    }
  default:
    return(Fl_Window::handle(event));
  }
}

//----------------------------------------- Zoom In
inline void GUI::cb_Zoom_i(int val) {
  if(val < 0) viewer->setParam("mod_zoom", 1.25);
  if(val > 0) viewer->setParam("mod_zoom", 0.80);
  //if(val ==0) viewer->zoomReset();
}
void GUI::cb_Zoom(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_Zoom_i(v);
}

//----------------------------------------- Reset
inline void GUI::cb_Reset_i(int i) {
  if(i==1) viewer->setParam("reset_view", "1");
  if(i==2) viewer->setParam("reset_view", "2");
  if(i==3) viewer->setParam("reset_view", "3");
  if(i==4) viewer->setParam("reset_view", "4");
}
void GUI::cb_Reset(Fl_Widget* o, int i) {
  ((GUI*)(o->parent()->user_data()))->cb_Reset_i(i);
}

//----------------------------------------- Script
inline void GUI::cb_Script_i() {
  viewer->runScript();
}
void GUI::cb_Script(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_Script_i();
}

//----------------------------------------- Rotate  X
inline void GUI::cb_RotateX_i(int amt) {
  viewer->setParam("mod_x_rotation", (double)(amt));
}
void GUI::cb_RotateX(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_RotateX_i(v);
}

//----------------------------------------- Rotate  Z
inline void GUI::cb_RotateZ_i(int amt) {
  viewer->setParam("mod_z_rotation", (double)(amt));
}
void GUI::cb_RotateZ(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_RotateZ_i(v);
}

//----------------------------------------- Mod Scale
inline void GUI::cb_ModScale_i(int amt) {
  viewer->setParam("mod_scale", (((double)amt)/100.0));
}
void GUI::cb_ModScale(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ModScale_i(v);
}

//----------------------------------------- Mod BaseAOF
inline void GUI::cb_ModBaseAOF_i(int amt) {
  viewer->setParam("mod_base_aof", amt);
}

void GUI::cb_ModBaseAOF(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ModBaseAOF_i(v);
}

//----------------------------------------- Mod BaseIPF
inline void GUI::cb_ModBaseIPF_i(int amt) {
  viewer->setParam("mod_base_ipf", amt);
}

void GUI::cb_ModBaseIPF(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ModBaseIPF_i(v);
}

//----------------------------------------- Mod PatchAOF
inline void GUI::cb_ModPatchAOF_i(int amt) {
  viewer->modPatchAOF(amt);
}
void GUI::cb_ModPatchAOF(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ModPatchAOF_i(v);
}

//----------------------------------------- Mod UniformAug
inline void GUI::cb_ModUniformAug_i(int amt) {
  viewer->setParam("mod_focus_len", amt);
  viewer->makeUniformIPF();
  updateXY();
}
void GUI::cb_ModUniformAug(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ModUniformAug_i(v);
}

//----------------------------------------- Sample
inline void GUI::cb_Sample_i(int amt) {
  viewer->takeSamples(amt);
  updateXY();
}
void GUI::cb_Sample(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_Sample_i(v);
}

//----------------------------------------- Rebuild
inline void GUI::cb_Rebuild_i(int amt) {
  viewer->makeUniformIPFxN(amt);
  updateXY();
}
void GUI::cb_Rebuild(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_Rebuild_i(v);
}

//----------------------------------------- Toggle AOF
inline void GUI::cb_ToggleAOF_i() {
  viewer->toggleAOF();
}
void GUI::cb_ToggleAOF(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_ToggleAOF_i();
}

//----------------------------------------- Toggle IPF
inline void GUI::cb_ToggleIPF_i() {
  viewer->toggleIPF();
}
void GUI::cb_ToggleIPF(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_ToggleIPF_i();
}

//----------------------------------------- ToggleDirectedRefine
inline void GUI::cb_ToggleDirectedRefine_i() {
  viewer->setParam("directed_refine", "toggle");
  viewer->makeUniformIPF();
  updateXY();
}
void GUI::cb_ToggleDirectedRefine(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_ToggleDirectedRefine_i();
}

//----------------------------------------- Toggle SmartAug
inline void GUI::cb_ToggleSmartAug_i() {
  viewer->toggleSmartAug();
  updateXY();
}
void GUI::cb_ToggleSmartAug(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_ToggleSmartAug_i();
}

//----------------------------------------- Toggle AutoPeak
inline void GUI::cb_ToggleAutoPeak_i() {
  viewer->setParam("auto_peak", "toggle");
  viewer->makeUniformIPF();
  updateXY();
}
void GUI::cb_ToggleAutoPeak(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_ToggleAutoPeak_i();
}

//----------------------------------------- SmartAugAmt
inline void GUI::cb_SmartAugAmt_i(int amt) {
  viewer->setParam("smart_amount",  intToString(amt));
  viewer->setParam("smart_percent", "0");
  viewer->makeUniformIPF();
  updateXY();
}
void GUI::cb_SmartAugAmt(Fl_Widget* o, int i) {
  ((GUI*)(o->parent()->user_data()))->cb_SmartAugAmt_i(i);
}

//----------------------------------------- SmartAugPct
inline void GUI::cb_SmartAugPct_i(int amt) {
  viewer->setParam("smart_amount",  "0");
  viewer->setParam("smart_percent", intToString(amt));
  viewer->makeUniformIPF();
  updateXY();
}
void GUI::cb_SmartAugPct(Fl_Widget* o, int i) {
  ((GUI*)(o->parent()->user_data()))->cb_SmartAugPct_i(i);
}

//----------------------------------------- Toggle Frame
inline void GUI::cb_ToggleFrame_i() {
  viewer->setParam("draw_frame", "toggle");
}
void GUI::cb_ToggleFrame(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_ToggleFrame_i();
}

//----------------------------------------- Frame Height
inline void GUI::cb_FrameHeight_i(int amt) {
  viewer->setParam("mod_frame_height", (double)amt);
}
void GUI::cb_FrameHeight(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_FrameHeight_i(v);
}

//----------------------------------------- Toggle Strict
inline void GUI::cb_ToggleStrict_i() {
  viewer->setParam("strict_range", "toggle");
  viewer->makeUniformIPF();
  updateXY();
}
void GUI::cb_ToggleStrict(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_ToggleStrict_i();
}

//----------------------------------------- MakeUniform
inline void GUI::cb_MakeUniform_i(int amt) {
  if(amt > 0)
    viewer->setParam("uniform_piece", amt);
  viewer->makeUniformIPF();
  updateXY();
}
void GUI::cb_MakeUniform(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_MakeUniform_i(v);
}

//----------------------------------------- MakePieces
inline void GUI::cb_MakePieces_i(int amt) {
  if(amt > 0)
    viewer->setParam("uniform_amount", amt);
  viewer->makeUniformIPF();
  updateXY();
}
void GUI::cb_MakePieces(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_MakePieces_i(v);
}

//----------------------------------------- ColorMap
inline void GUI::cb_ColorMap_i(int index) {
  string str = "default";
  if(index ==2)
    str = "copper";
  else if(index == 3)
    str = "bone";
  viewer->modColorMap(str);
}
void GUI::cb_ColorMap(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ColorMap_i(v);
}

//----------------------------------------- PrintParams
inline void GUI::cb_PrintParams_i() {
  viewer->printParams();
}
void GUI::cb_PrintParams(Fl_Widget* o) {
  ((GUI*)(o->parent()->user_data()))->cb_PrintParams_i();
}

//----------------------------------------- ColorBack
inline void GUI::cb_ColorBack_i(int index) {
  if(index == 0)
    viewer->setParam("clear_color", "white");
  else if(index == 1)  // PurplishBlue
    viewer->setParam("clear_color", "0.285,0.242,0.469");
  else if(index == 2)  // PurplishBlue
    viewer->setParam("clear_color", "macbeige");
  else
    return;
  viewer->redraw();
}
void GUI::cb_ColorBack(Fl_Widget* o, int v) {
  ((GUI*)(o->parent()->user_data()))->cb_ColorBack_i(v);
}

//----------------------------------------- Quit
void GUI::cb_Quit() {
  exit(0);
}

//----------------------------------------- UpdateXY
void GUI::updateXY() 
{
  string str;
  bool ok;

  str = intToString((int)(viewer->getParam("sample_count", ok)));
  if(!ok)
    str = "n/a";
  p_samp_count->value(str.c_str());

  str = doubleToString(viewer->getParam("worst_err", ok),4);
  if(!ok)
    str = "n/a";
  p_worst_err->value(str.c_str());

  str = doubleToString(viewer->getParam("avg_err", ok),4);
  if(!ok)
    str = "n/a";
  p_avg_err->value(str.c_str());

  str = doubleToString(viewer->getParam("squared_err", ok),4);
  if(!ok)
    str = "n/a";
  p_square_err->value(str.c_str());

  str = doubleToString(viewer->getParam("sample_high", ok),4);
  if(!ok)
    str = "n/a";
  p_samp_high->value(str.c_str());

  str = doubleToString(viewer->getParam("sample_low", ok),4);
  if(!ok)
    str = "n/a";
  p_samp_low->value(str.c_str());

  str = intToString((int)(viewer->getParam("piece_count", ok)));
  if(!ok)
    str = "n/a";
  p_piece_count->value(str.c_str());

  str = intToString((int)(viewer->getParam("unif_aug_size", ok)));
  if(!ok)
    str = "n/a";
  p_unif_aug_size->value(str.c_str());

  str = doubleToString(viewer->getParam("create_time", ok), 3);
  if(!ok)
    str = "n/a";
  p_create_time->value(str.c_str());
  
  str = viewer->getParam("uniform_piece");
  p_uniform_str->value(str.c_str());

  str = viewer->getParam("auto_peak");
  p_auto_peak->value(str.c_str());

  str = viewer->getParam("refine_region");
  p_refine_reg_str->value(str.c_str());
  
  str = viewer->getParam("refine_piece");
  p_refine_pce_str->value(str.c_str());
  
  str = viewer->getParam("reflector_errors");
  p_reflector_errors->value(str.c_str());
  
}


#if 0
//----------------------------------------- set_refine_piece
void GUI::cb_set_refine_pce_i() {
  if(viewer)
    viewer->setParam("refine_piece", i_refine_pce_str->value());
}

void GUI::cb_set_refine_pce(Fl_Input* o, void* v) {
  ((GUI*)(o->parent()->user_data()))->cb_set_refine_pce_i();
}
#endif





