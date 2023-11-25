/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
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
#include "FFV_GUI.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

FFV_GUI::FFV_GUI(int wid, int hgt, const char *label)
  : Common_IPF_GUI(wid, hgt, label) 
{
  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();
  this->size_range(800,600, 1400,1000, 0,0, 1);

  m_ffv_viewer = new FFV_Viewer(0, 30, w(), h()-125);
  m_viewer = (Common_IPFViewer*)(m_ffv_viewer);
 
  augmentMenu();

  initWidgets();
  resizeWidgetsShape();
  resizeWidgetsText();

  this->end();
  this->resizable(this);
  this->show();
}

//--------------------------------------------------------------
// Procedure: setViewerParam()

bool FFV_GUI::setViewerParam(string param, string value)
{
  if(m_ffv_viewer)
    return(m_ffv_viewer->setParam(param, value));
  return(false);
}

//--------------------------------------------------------------
// Procedure: setViewerParam()

bool FFV_GUI::setViewerParam(string param, double value)
{
  if(m_ffv_viewer)
    return(m_ffv_viewer->setParam(param, value));
  return(true);
}

//--------------------------------------------------------------------------- 
// Procedure: resizeWidgetsShape()     

void FFV_GUI::resizeWidgetsShape()
{
  m_ffv_viewer->resize(0, 30, w(), h()-125);

  int extra_wid = w() - m_start_wid;
  int field_hgt = 20;
  
  int row1 = h() - 90;
  int row2 = row1 + 30;
  int row3 = row2 + 30;  
  int col1 = 80;

  //===================================================
  // Column ONE 
  //===================================================
  int sac_x = col1;
  int sac_y = row1;
  int sac_wid = 60;
  m_fld_samp_count->resize(sac_x, sac_y, sac_wid, field_hgt);

  int werr_x = col1;
  int werr_y = row2;
  int werr_wid = 60;
  m_fld_worst_err->resize(werr_x, werr_y, werr_wid, field_hgt);

  int pcnt_x = col1;
  int pcnt_y = row3;
  int pcnt_wid = 60;
  m_fld_piece_count->resize(pcnt_x, pcnt_y, pcnt_wid, field_hgt);

  //===================================================
  // Column TWO
  //===================================================
  int avge_x = sac_x + sac_wid + 70;
  int avge_y = row1;
  int avge_wid = 60;
  m_fld_avg_err->resize(avge_x, avge_y, avge_wid, field_hgt);

  int sqre_x = werr_x + werr_wid + 70;
  int sqre_y = row2;
  int sqre_wid = 60;
  m_fld_square_err->resize(sqre_x, sqre_y, sqre_wid, field_hgt);

  int uas_x = pcnt_x + pcnt_wid + 70;
  int uas_y = row3;
  int uas_wid = 60;
  m_fld_unif_aug_size->resize(uas_x, uas_y, uas_wid, field_hgt);

  //===================================================
  // Column THREE
  //===================================================
  int shgh_x = avge_x + avge_wid + 80;
  int shgh_y = row1;
  int shgh_wid = 60;
  m_fld_samp_high->resize(shgh_x, shgh_y, shgh_wid, field_hgt);

  int slow_x = sqre_x + sqre_wid + 80;
  int slow_y = row2;
  int slow_wid = 60;
  m_fld_samp_low->resize(slow_x, slow_y, slow_wid, field_hgt);

  int crt_x = uas_x + uas_wid + 80;
  int crt_y = row3;
  int crt_wid = 60;
  m_fld_create_time->resize(crt_x, crt_y, crt_wid, field_hgt);

  //===================================================
  // Column FOUR
  //===================================================
  int upc_x = shgh_x + shgh_wid + 100;
  int upc_y = row1;
  int upc_wid = 170 + (extra_wid/2);
  m_fld_uniform_str->resize(upc_x, upc_y, upc_wid, field_hgt);

  int rreg_x = slow_x + slow_wid + 100;
  int rreg_y = row2;
  int rreg_wid = 170 + (extra_wid/2);
  m_fld_refine_reg_str->resize(rreg_x, rreg_y, rreg_wid, field_hgt);

  int refe_x = crt_x + crt_wid + 100;
  int refe_y = row3;
  int refe_wid = 430 + extra_wid;
  m_fld_reflector_errors->resize(refe_x, refe_y, refe_wid, field_hgt);

  //===================================================
  // Column FIVE
  //===================================================
  int autp_x = upc_x + upc_wid + 80;
  int autp_y = row1;
  int autp_wid = 180 + (extra_wid/2);
  m_fld_auto_peak->resize(autp_x, autp_y, autp_wid, field_hgt);

  int rpc_x = rreg_x + rreg_wid + 80;
  int rpc_y = row2;
  int rpc_wid = 180 + (extra_wid/2);
  m_fld_refine_pce_str->resize(rpc_x, rpc_y, rpc_wid, field_hgt);
}

//--------------------------------------------------------------------------- 
// Procedure: resizeWidgetsText()

void FFV_GUI::resizeWidgetsText()
{
  int info_size  = 10;

  // Column One ----------------------------------------------- ONE
  m_fld_samp_count->textsize(info_size); 
  m_fld_samp_count->labelsize(info_size);
  
  m_fld_worst_err->textsize(info_size); 
  m_fld_worst_err->labelsize(info_size);

  m_fld_piece_count->textsize(info_size); 
  m_fld_piece_count->labelsize(info_size);

  // Column Two ----------------------------------------------- TWO
  m_fld_avg_err->textsize(info_size); 
  m_fld_avg_err->labelsize(info_size);

  m_fld_square_err->textsize(info_size); 
  m_fld_square_err->labelsize(info_size);

  m_fld_unif_aug_size->textsize(info_size); 
  m_fld_unif_aug_size->labelsize(info_size);

  // Column Three -------------------------------------------- THREE
  m_fld_samp_high->textsize(info_size); 
  m_fld_samp_high->labelsize(info_size);

  m_fld_samp_low->textsize(info_size); 
  m_fld_samp_low->labelsize(info_size);

  m_fld_create_time->textsize(info_size); 
  m_fld_create_time->labelsize(info_size);

  // Column Four -------------------------------------------- FOUR
  m_fld_uniform_str->textsize(info_size); 
  m_fld_uniform_str->labelsize(info_size);
  
  m_fld_refine_reg_str->textsize(info_size); 
  m_fld_refine_reg_str->labelsize(info_size);
  
  m_fld_reflector_errors->textsize(info_size); 
  m_fld_reflector_errors->labelsize(info_size);

  // Column Five -------------------------------------------- FIVE
  m_fld_auto_peak->textsize(info_size); 
  m_fld_auto_peak->labelsize(info_size);

  m_fld_refine_pce_str->textsize(info_size); 
  m_fld_refine_pce_str->labelsize(info_size);
}

//---------------------------------------------------------- 
// Procedure: resize   

void FFV_GUI::resize(int lx, int ly, int lw, int lh)
{
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgetsShape();
  resizeWidgetsText();
} 


//-------------------------------------------------------------------------
// Procedure: augmentMenu()

void FFV_GUI::augmentMenu()
{
  //===================================================================
  // File    Pull-Down Menu
  //===================================================================
  m_menubar->add("File/Print Params", 0,
		 (Fl_Callback*)FFV_GUI::cb_PrintParams, 0, 0);

  //===================================================================
  // AOF    Pull-Down Menu
  //===================================================================
  m_menubar->add("AOF/Base +", FL_CTRL+'0',
		 (Fl_Callback*)FFV_GUI::cb_ModBaseAOF, (void*)+1, 0);
  m_menubar->add("AOF/Base -", FL_CTRL+'9',
		 (Fl_Callback*)FFV_GUI::cb_ModBaseAOF, (void*)-1, 0);
  m_menubar->add("AOF/AOF Patch +", 'P',
		 (Fl_Callback*)FFV_GUI::cb_ModPatchAOF, (void*)+1, 0);
  m_menubar->add("AOF/AOF Patch -", 'p',
		 (Fl_Callback*)FFV_GUI::cb_ModPatchAOF, (void*)-1, 0);
  m_menubar->add("AOF/Toggle AOF",  'a',
		 (Fl_Callback*)FFV_GUI::cb_ToggleAOF, (void*)0, 0);

  //===================================================================
  // IPF    Pull-Down Menu
  //===================================================================
  m_menubar->add("IPF/Toggle Strict", '`',
		 (Fl_Callback*)FFV_GUI::cb_ToggleStrict, (void*)0,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);
  m_menubar->add("IPF/Create   ", 'c',
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)0, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 1",   0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)1, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 2",   0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)2, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 3",   0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)3, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 4",   0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)4, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 5",   0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)5, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 7",   0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)7, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 10",  0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)10, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 15",  0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)15, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 20",  0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)20, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 25",  0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)25, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 30",  0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)30, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 40",  0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)40, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 50",  0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)50, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 75",  0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)75, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 100", 0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)100, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 250", 0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)250, FL_MENU_RADIO);
  m_menubar->add("IPF/Uniform 500", 0,
		 (Fl_Callback*)FFV_GUI::cb_MakeUniform, (void*)500,
		 FL_MENU_RADIO|FL_MENU_DIVIDER);
  m_menubar->add("IPF/Pieces 35", 0,
		 (Fl_Callback*)FFV_GUI::cb_MakePieces, (void*)35, FL_MENU_RADIO);
  m_menubar->add("IPF/Pieces 63", 0,
		 (Fl_Callback*)FFV_GUI::cb_MakePieces, (void*)63, FL_MENU_RADIO);
  m_menubar->add("IPF/Pieces 64", 0,
		 (Fl_Callback*)FFV_GUI::cb_MakePieces, (void*)64, FL_MENU_RADIO);
  m_menubar->add("IPF/Pieces 500", 0,
		 (Fl_Callback*)FFV_GUI::cb_MakePieces, (void*)500, FL_MENU_RADIO);
  m_menubar->add("IPF/Pieces 512", 0,
		 (Fl_Callback*)FFV_GUI::cb_MakePieces, (void*)512, FL_MENU_RADIO);
  m_menubar->add("IPF/Pieces 1000", 0,
		 (Fl_Callback*)FFV_GUI::cb_MakePieces, (void*)1000, FL_MENU_RADIO);
  m_menubar->add("IPF/Pieces 1024", 0,
		 (Fl_Callback*)FFV_GUI::cb_MakePieces, (void*)1024, FL_MENU_RADIO);
  m_menubar->add("IPF/Pieces 1025", 0,
		 (Fl_Callback*)FFV_GUI::cb_MakePieces, (void*)1025, FL_MENU_RADIO);
  m_menubar->add("IPF/Pieces 4000", 0,
		 (Fl_Callback*)FFV_GUI::cb_MakePieces, (void*)4000, FL_MENU_RADIO);

  //===================================================================
  // Directed-Refine    Pull-Down Menu
  //===================================================================
  m_menubar->add("Directed-Refine/Toggle Directed Refine",  'd',
		 (Fl_Callback*)FFV_GUI::cb_ToggleDirectedRefine, (void*)0,
		 FL_MENU_DIVIDER);
  m_menubar->add("Directed-Refine/Smaller-Uniform-Refine", '[',
		 (Fl_Callback*)FFV_GUI::cb_ModUniformAug, (void*)-1, 0);
  m_menubar->add("Directed-Refine/Larger-Uniform-Refine",  ']',
		 (Fl_Callback*)FFV_GUI::cb_ModUniformAug, (void*)1, 0);

  //===================================================================
  // Smart-Refine    Pull-Down Menu
  //===================================================================
  m_menubar->add("Smart-Refine/Toggle-Smart-Refine", 'm',
		 (Fl_Callback*)FFV_GUI::cb_ToggleSmartAug, (void*)0, 0);
  m_menubar->add("Smart-Refine/smart_amount=100",  0,
		 (Fl_Callback*)FFV_GUI::cb_SmartAugAmt, (void*)100, 0);
  m_menubar->add("Smart-Refine/smart_amount=200",  0,
		 (Fl_Callback*)FFV_GUI::cb_SmartAugAmt, (void*)200, 0);
  m_menubar->add("Smart-Refine/smart_amount=500",  0,
		 (Fl_Callback*)FFV_GUI::cb_SmartAugAmt, (void*)500, 0);
  m_menubar->add("Smart-Refine/smart_amount=750",  0,
		 (Fl_Callback*)FFV_GUI::cb_SmartAugAmt, (void*)750, 0);
  m_menubar->add("Smart-Refine/smart_amount=1000", 0,
		 (Fl_Callback*)FFV_GUI::cb_SmartAugAmt, (void*)1000, 0);
  m_menubar->add("Smart-Refine/smart_amount=2000", 0,
		 (Fl_Callback*)FFV_GUI::cb_SmartAugAmt, (void*)2000, 0);
  m_menubar->add("Smart-Refine/smart_amount=4000", 0,
		 (Fl_Callback*)FFV_GUI::cb_SmartAugAmt, (void*)4000, FL_MENU_DIVIDER);
  m_menubar->add("Smart-Refine/smart_percent= 10",  0,
		 (Fl_Callback*)FFV_GUI::cb_SmartAugPct, (void*)10, 0);
  m_menubar->add("Smart-Refine/smart_percent= 15",  0,
		 (Fl_Callback*)FFV_GUI::cb_SmartAugPct, (void*)15, 0);
  m_menubar->add("Smart-Refine/smart_percent= 25",  0,
		 (Fl_Callback*)FFV_GUI::cb_SmartAugPct, (void*)25, 0);
  m_menubar->add("Smart-Refine/smart_percent= 35",  0,
		 (Fl_Callback*)FFV_GUI::cb_SmartAugPct, (void*)35, 0);
  m_menubar->add("Smart-Refine/smart_percent= 50",  0,
		 (Fl_Callback*)FFV_GUI::cb_SmartAugPct, (void*)50, 0);
  m_menubar->add("Smart-Refine/smart_percent= 75",  0,
		 (Fl_Callback*)FFV_GUI::cb_SmartAugPct, (void*)75, 0);
  m_menubar->add("Smart-Refine/smart_percent= 100", 0,
		 (Fl_Callback*)FFV_GUI::cb_SmartAugPct, (void*)100, 0);

  //===================================================================
  // AutoPeak Pull-Down Menu
  //===================================================================
  m_menubar->add("AutoPeak/Toggle-AutoPeak", 't',
		 (Fl_Callback*)FFV_GUI::cb_ToggleAutoPeak, (void*)0, 0);
  
  //===================================================================
  // Analysis Pull-Down Menu
  //===================================================================
  m_menubar->add("Analysis/Sample+50K", ' ',
		 (Fl_Callback*)FFV_GUI::cb_Sample, (void*)50000, 0);
  m_menubar->add("Analysis/Rebuildx10", '1',
		 (Fl_Callback*)FFV_GUI::cb_Rebuild, (void*)10, 0);
  m_menubar->add("Analysis/Rebuildx50", '2',
		 (Fl_Callback*)FFV_GUI::cb_Rebuild, (void*)50, 0);
  m_menubar->add("Analysis/Rebuildx100", '3',
		 (Fl_Callback*)FFV_GUI::cb_Rebuild, (void*)100, 0);
  m_menubar->add("Analysis/Rebuildx100", '4',
		 (Fl_Callback*)FFV_GUI::cb_Rebuild, (void*)500, 0);
}

//----------------------------------------------------------
// Procedure: initWidgets()

void FFV_GUI::initWidgets()
{
  // Column One ----------------------------------------------- ONE
  m_fld_samp_count = new Fl_Output(0, 0, 1, 1, "Samples:"); 
  m_fld_samp_count->clear_visible_focus();
  
  m_fld_worst_err = new Fl_Output(0, 0, 1, 1, "WST-ERR:"); 
  m_fld_worst_err->clear_visible_focus();

  m_fld_piece_count = new Fl_Output(0, 0, 1, 1, "Pieces:"); 
  m_fld_piece_count->clear_visible_focus();

  // Column Two ----------------------------------------------- TWO
  m_fld_avg_err = new Fl_Output(0, 0, 1, 1, "AVG-ERR:"); 
  m_fld_avg_err->clear_visible_focus();

  m_fld_square_err = new Fl_Output(0, 0, 1, 1, "SQR-ERR:"); 
  m_fld_square_err->clear_visible_focus();

  m_fld_unif_aug_size = new Fl_Output(0, 0, 1, 1, "Unif-Aug:"); 
  m_fld_unif_aug_size->clear_visible_focus();

  // Column Three -------------------------------------------- THREE
  m_fld_samp_high = new Fl_Output(0, 0, 1, 1, "Sample High:"); 
  m_fld_samp_high->clear_visible_focus();

  m_fld_samp_low = new Fl_Output(0, 0, 1, 1, "Sample Low:"); 
  m_fld_samp_low->clear_visible_focus();

  m_fld_create_time = new Fl_Output(0, 0, 1, 1, "Create-Time:"); 
  m_fld_create_time->clear_visible_focus();

  // Column Four -------------------------------------------- FOUR
  m_fld_uniform_str = new Fl_Output(0, 0, 1, 1, "Uniform Piece:"); 
  m_fld_uniform_str->clear_visible_focus();
  
  m_fld_auto_peak = new Fl_Output(0, 0, 1, 1, "AutoPeak:"); 
  m_fld_auto_peak->clear_visible_focus();
  
  m_fld_refine_reg_str = new Fl_Output(0, 0, 1, 1, "Refine Region:"); 
  m_fld_refine_reg_str->clear_visible_focus();
  
  // Column Five -------------------------------------------- FIVE
  m_fld_refine_pce_str = new Fl_Output(0, 0, 1, 1, "Refine Piece:"); 
  m_fld_refine_pce_str->clear_visible_focus();

  m_fld_reflector_errors  = new Fl_Output(0, 0, 1, 1, "Reflector Errors:"); 
  m_fld_reflector_errors->clear_visible_focus();
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

int FFV_GUI::handle(int event) 
{
#if 0
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
#endif
  return(Fl_Window::handle(event));
}

//----------------------------------------- Mod BaseAOF
inline void FFV_GUI::cb_ModBaseAOF_i(int amt) {
  m_ffv_viewer->setParam("mod_base_aof", amt);
}

void FFV_GUI::cb_ModBaseAOF(Fl_Widget* o, int v) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_ModBaseAOF_i(v);
}

//----------------------------------------- Mod PatchAOF
inline void FFV_GUI::cb_ModPatchAOF_i(int amt) {
  m_ffv_viewer->modPatchAOF(amt);
}
void FFV_GUI::cb_ModPatchAOF(Fl_Widget* o, int v) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_ModPatchAOF_i(v);
}

//----------------------------------------- Mod UniformAug
inline void FFV_GUI::cb_ModUniformAug_i(int amt) {
  m_ffv_viewer->setParam("mod_uniform_piece", amt);
  m_ffv_viewer->makeUniformIPF();
  updateXY();
}
void FFV_GUI::cb_ModUniformAug(Fl_Widget* o, int v) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_ModUniformAug_i(v);
}

//----------------------------------------- Sample
inline void FFV_GUI::cb_Sample_i(int amt) {
  m_ffv_viewer->takeSamples(amt);
  updateXY();
}
void FFV_GUI::cb_Sample(Fl_Widget* o, int v) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_Sample_i(v);
}

//----------------------------------------- Rebuild
inline void FFV_GUI::cb_Rebuild_i(int amt) {
  m_ffv_viewer->makeUniformIPFxN(amt);
  updateXY();
}
void FFV_GUI::cb_Rebuild(Fl_Widget* o, int v) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_Rebuild_i(v);
}

//----------------------------------------- Toggle AOF
inline void FFV_GUI::cb_ToggleAOF_i() {
  m_ffv_viewer->toggleAOF();
}
void FFV_GUI::cb_ToggleAOF(Fl_Widget* o) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_ToggleAOF_i();
}

//----------------------------------------- Toggle IPF
inline void FFV_GUI::cb_ToggleIPF_i() {
  m_ffv_viewer->toggleIPF();
}
void FFV_GUI::cb_ToggleIPF(Fl_Widget* o) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_ToggleIPF_i();
}

//----------------------------------------- ToggleDirectedRefine
inline void FFV_GUI::cb_ToggleDirectedRefine_i() {
  m_ffv_viewer->setParam("directed_refine", "toggle");
  m_ffv_viewer->makeUniformIPF();
  updateXY();
}
void FFV_GUI::cb_ToggleDirectedRefine(Fl_Widget* o) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_ToggleDirectedRefine_i();
}

//----------------------------------------- Toggle SmartAug
inline void FFV_GUI::cb_ToggleSmartAug_i() {
  m_ffv_viewer->toggleSmartAug();
  updateXY();
}
void FFV_GUI::cb_ToggleSmartAug(Fl_Widget* o) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_ToggleSmartAug_i();
}

//----------------------------------------- Toggle AutoPeak
inline void FFV_GUI::cb_ToggleAutoPeak_i() {
  m_ffv_viewer->setParam("auto_peak", "toggle");
  m_ffv_viewer->makeUniformIPF();
  updateXY();
}
void FFV_GUI::cb_ToggleAutoPeak(Fl_Widget* o) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_ToggleAutoPeak_i();
}

//----------------------------------------- SmartAugAmt
inline void FFV_GUI::cb_SmartAugAmt_i(int amt) {
  m_ffv_viewer->setParam("smart_amount",  amt);
  m_ffv_viewer->setParam("smart_percent", 0);
  m_ffv_viewer->makeUniformIPF();
  updateXY();
}
void FFV_GUI::cb_SmartAugAmt(Fl_Widget* o, int i) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_SmartAugAmt_i(i);
}

//----------------------------------------- SmartAugPct
inline void FFV_GUI::cb_SmartAugPct_i(int amt) {
  m_ffv_viewer->setParam("smart_amount",  0);
  m_ffv_viewer->setParam("smart_percent", amt);
  m_ffv_viewer->makeUniformIPF();
  updateXY();
}
void FFV_GUI::cb_SmartAugPct(Fl_Widget* o, int i) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_SmartAugPct_i(i);
}

//----------------------------------------- Toggle Strict
inline void FFV_GUI::cb_ToggleStrict_i() {
  m_ffv_viewer->setParam("strict_range", "toggle");
  m_ffv_viewer->makeUniformIPF();
  updateXY();
}
void FFV_GUI::cb_ToggleStrict(Fl_Widget* o) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_ToggleStrict_i();
}

//----------------------------------------- MakeUniform
inline void FFV_GUI::cb_MakeUniform_i(int amt) {
  if(amt > 0)
    m_ffv_viewer->setParam("uniform_piece", amt);
  m_ffv_viewer->makeUniformIPF();
  updateXY();
}
void FFV_GUI::cb_MakeUniform(Fl_Widget* o, int v) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_MakeUniform_i(v);
}

//----------------------------------------- MakePieces
inline void FFV_GUI::cb_MakePieces_i(int amt) {
  if(amt > 0)
    m_ffv_viewer->setParam("uniform_amount", amt);
  m_ffv_viewer->makeUniformIPF();
  updateXY();
}
void FFV_GUI::cb_MakePieces(Fl_Widget* o, int v) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_MakePieces_i(v);
}

//----------------------------------------- PrintParams
inline void FFV_GUI::cb_PrintParams_i() {
  m_ffv_viewer->printParams();
}
void FFV_GUI::cb_PrintParams(Fl_Widget* o) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_PrintParams_i();
}


//----------------------------------------- UpdateXY
void FFV_GUI::updateXY() 
{
  string str;
  bool ok;

  // Column 1--------------------------------------------
  str = intToString((int)(m_ffv_viewer->getParam("sample_count", ok)));
  if(!ok)
    str = "n/a";
  m_fld_samp_count->value(str.c_str());

  str = doubleToString(m_ffv_viewer->getParam("worst_err", ok),4);
  if(!ok)
    str = "n/a";
  m_fld_worst_err->value(str.c_str());

  str = intToString((int)(m_ffv_viewer->getParam("piece_count", ok)));
  if(!ok)
    str = "n/a";
  m_fld_piece_count->value(str.c_str());

  // Column 2 --------------------------------------------
  str = doubleToString(m_ffv_viewer->getParam("avg_err", ok),4);
  if(!ok)
    str = "n/a";
  m_fld_avg_err->value(str.c_str());

  str = doubleToString(m_ffv_viewer->getParam("squared_err", ok),4);
  if(!ok)
    str = "n/a";
  m_fld_square_err->value(str.c_str());

  str = intToString((int)(m_ffv_viewer->getParam("unif_aug_size", ok)));
  if(!ok)
    str = "n/a";
  m_fld_unif_aug_size->value(str.c_str());

  // Column 3 --------------------------------------------
  str = doubleToString(m_ffv_viewer->getParam("sample_high", ok),4);
  if(!ok)
    str = "n/a";
  m_fld_samp_high->value(str.c_str());

  str = doubleToString(m_ffv_viewer->getParam("sample_low", ok),4);
  if(!ok)
    str = "n/a";
  m_fld_samp_low->value(str.c_str());

  str = doubleToString(m_ffv_viewer->getParam("create_time", ok), 3);
  if(!ok)
    str = "n/a";
  m_fld_create_time->value(str.c_str());

  
  // Column 4 --------------------------------------------
  str = m_ffv_viewer->getParam("uniform_piece");
  m_fld_uniform_str->value(str.c_str());

  str = m_ffv_viewer->getParam("refine_region");
  m_fld_refine_reg_str->value(str.c_str());
  
  str = m_ffv_viewer->getParam("reflector_errors");
  m_fld_reflector_errors->value(str.c_str());
  
  // Column 5 --------------------------------------------
  str = m_ffv_viewer->getParam("auto_peak");
  m_fld_auto_peak->value(str.c_str());

  str = m_ffv_viewer->getParam("refine_piece");
  m_fld_refine_pce_str->value(str.c_str());
  
}


#if 0
//----------------------------------------- set_refine_piece
void FFV_GUI::cb_set_refine_pce_i() {
  if(viewer)
    m_ffv_viewer->setParam("refine_piece", i_refine_pce_str->value());
}

void FFV_GUI::cb_set_refine_pce(Fl_Input* o, void* v) {
  ((FFV_GUI*)(o->parent()->user_data()))->cb_set_refine_pce_i();
}
#endif





