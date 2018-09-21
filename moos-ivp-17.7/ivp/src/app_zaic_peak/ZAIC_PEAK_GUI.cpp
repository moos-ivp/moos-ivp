/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_PEAK_GUI.cpp                                    */
/*    DATE: June 17th, 2006                                      */
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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "ZAIC_PEAK_GUI.h"
#include "ZAIC_PEAK_Model.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

ZAIC_PEAK_GUI::ZAIC_PEAK_GUI(int wid, int hgt, const char *label)
  : ZAIC_GUI(wid, hgt, label) {

  int txt_size=10;
  int top_marg = 30;
  int bot_marg = 80;
  int sid_marg = 5;
  int q_height = h()-(top_marg+bot_marg);

  m_zaic_model = new ZAIC_PEAK_Model();
  m_zaic_model->setDomain(501);

  m_zaic_viewer = new ZAIC_Viewer(sid_marg, top_marg, w()-(sid_marg*2), q_height);
  m_zaic_viewer->setModel(m_zaic_model);

  //==========================================================================
  m_fld_summit1 = new Fl_Output(70, q_height+top_marg+10, 60, 20, "summit1:"); 
  m_fld_summit1->textsize(txt_size); 
  m_fld_summit1->labelsize(txt_size);
  m_fld_summit1->clear_visible_focus();

  m_fld_pwidth1 = new Fl_Output(70, q_height+top_marg+40, 60, 20, "peak_width1:"); 
  m_fld_pwidth1->textsize(txt_size); 
  m_fld_pwidth1->labelsize(txt_size);
  m_fld_pwidth1->clear_visible_focus();

  m_fld_bwidth1 = new Fl_Output(210, q_height+top_marg+40, 60, 20, "base_width1:"); 
  m_fld_bwidth1->textsize(txt_size); 
  m_fld_bwidth1->labelsize(txt_size);
  m_fld_bwidth1->clear_visible_focus();

  m_fld_sdelta1 = new Fl_Output(210, q_height+top_marg+10, 60, 20, "summit_delta1:"); 
  m_fld_sdelta1->textsize(txt_size); 
  m_fld_sdelta1->labelsize(txt_size);
  m_fld_sdelta1->clear_visible_focus();

  //==========================================================================
  //Fl_Color fcolor_blue  = fl_rgb_color(140, 140, 220);
  m_fld_summit2 = new Fl_Output(345, q_height+top_marg+10, 60, 20, "summit2:"); 
  m_fld_summit2->textsize(txt_size); 
  m_fld_summit2->labelsize(txt_size);
  m_fld_summit2->clear_visible_focus();

  m_fld_pwidth2 = new Fl_Output(345, q_height+top_marg+40, 60, 20, "peak_width2:"); 
  m_fld_pwidth2->textsize(txt_size); 
  m_fld_pwidth2->labelsize(txt_size);
  m_fld_pwidth2->clear_visible_focus();

  m_fld_bwidth2 = new Fl_Output(490, q_height+top_marg+40, 60, 20, "base_width2:"); 
  m_fld_bwidth2->textsize(txt_size); 
  m_fld_bwidth2->labelsize(txt_size);
  m_fld_bwidth2->clear_visible_focus();

  m_fld_sdelta2 = new Fl_Output(490, q_height+top_marg+10, 60, 20, "summit_delta2:"); 
  m_fld_sdelta2->textsize(txt_size); 
  m_fld_sdelta2->labelsize(txt_size);
  m_fld_sdelta2->clear_visible_focus();

  //==========================================================================
  m_fld_maxutil = new Fl_Output(620, q_height+top_marg+10, 60, 20, "max_util:"); 
  m_fld_maxutil->textsize(txt_size); 
  m_fld_maxutil->labelsize(txt_size);
  m_fld_maxutil->clear_visible_focus();

  m_fld_minutil = new Fl_Output(620, q_height+top_marg+40, 60, 20, "min_util:"); 
  m_fld_minutil->textsize(txt_size); 
  m_fld_minutil->labelsize(txt_size);
  m_fld_minutil->clear_visible_focus();

  augmentMenu();
  this->end();
  this->resizable(this);
  this->show();
}


//----------------------------------------------------------
// Procedure: augmentMenu()

void ZAIC_PEAK_GUI::augmentMenu()
{
  mbar->add("ZAIC-Elements-View/ZAIC Element  One  ", 0,
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_DrawMode, (void*)1, 0);
  mbar->add("ZAIC-Elements-View/ZAIC Element  Two  ", 0, 
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_DrawMode, (void*)2, 0);
  mbar->add("ZAIC-Elements-View/Both Elements Summed   ", 0, 
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_DrawMode, (void*)3, 0);
  mbar->add("ZAIC-Elements-View/Both Elements Maxed    ", 0, 
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_DrawMode, (void*)4, 0);
  mbar->add("ZAIC-Elements-View/Elements View Toggled  ", 't', 
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_DrawMode, (void*)0, 0);

  mbar->add("Modify Mode/ZAIC 1: Adjust Summit Position ", '0', 
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_CurrMode, (void*)0, 0);
  mbar->add("Modify Mode/ZAIC 1: Adjust PeakWidth ",       '1', 
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_CurrMode, (void*)1, 0);
  mbar->add("Modify Mode/ZAIC 1: Adjust BaseWidth ",       '2', 
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_CurrMode, (void*)2, 0);
  mbar->add("Modify Mode/ZAIC 1: Adjust SummitDelta ",     '3', 
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_CurrMode, (void*)3, FL_MENU_DIVIDER);

  mbar->add("Modify Mode/ZAIC 2: Adjust Summit Position ", '4', 
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_CurrMode, (void*)4, 0);
  mbar->add("Modify Mode/ZAIC 2: Adjust PeakWidth ",       '5', 
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_CurrMode, (void*)5, 0);
  mbar->add("Modify Mode/ZAIC 2: Adjust BaseWidth ",       '6', 
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_CurrMode, (void*)6, 0);
  mbar->add("Modify Mode/ZAIC 2: Adjust SummitDelta ",     '7', 
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_CurrMode, (void*)7, FL_MENU_DIVIDER);

  mbar->add("Modify Mode/Adjust MaxUtil ", '8', 
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_CurrMode, (void*)8, 0);
  mbar->add("Modify Mode/Adjust MinUtil ",         '9', 
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_CurrMode, (void*)9, 0);
  
  mbar->add("Modify ZAIC/Move Left  ", FL_Left,    
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_MoveX, (void*)-1, 0);
  mbar->add("Modify ZAIC/Move Right ", FL_Right,   
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_MoveX, (void*)1, 0);
  mbar->add("Modify ZAIC/Toggle WRAP ", 'w',       
	    (Fl_Callback*)ZAIC_PEAK_GUI::cb_ToggleWRAP, (void*)0, 0);
}


//----------------------------------------- ToggleWRAP
inline void ZAIC_PEAK_GUI::cb_ToggleWRAP_i() {
  ((ZAIC_PEAK_Model*)(m_zaic_model))->toggleValueWrap();
  updateOutput();
  m_zaic_viewer->redraw();
}
void ZAIC_PEAK_GUI::cb_ToggleWRAP(Fl_Widget* o) {
  ((ZAIC_PEAK_GUI*)(o->parent()->user_data()))->cb_ToggleWRAP_i();
}


//----------------------------------------- DrawMode
inline void ZAIC_PEAK_GUI::cb_DrawMode_i(int v) {
  ((ZAIC_PEAK_Model*)(m_zaic_model))->drawMode(v);
  m_zaic_viewer->redraw();
}
void ZAIC_PEAK_GUI::cb_DrawMode(Fl_Widget* o, int v) {
  ((ZAIC_PEAK_GUI*)(o->parent()->user_data()))->cb_DrawMode_i(v);
}


//----------------------------------------- UpdateOutput
void ZAIC_PEAK_GUI::updateOutput() 
{
  string str;

  str = doubleToString(((ZAIC_PEAK_Model*)(m_zaic_model))->getSummit1(),2);
  m_fld_summit1->value(str.c_str());

  str = doubleToString(((ZAIC_PEAK_Model*)(m_zaic_model))->getPeakWidth1(),2);
  m_fld_pwidth1->value(str.c_str());

  str = doubleToString(((ZAIC_PEAK_Model*)(m_zaic_model))->getBaseWidth1(),2);
  m_fld_bwidth1->value(str.c_str());

  str = doubleToString(((ZAIC_PEAK_Model*)(m_zaic_model))->getSummitDelta1(),2);
  m_fld_sdelta1->value(str.c_str());


  str = doubleToString(((ZAIC_PEAK_Model*)(m_zaic_model))->getSummit2(),2);
  m_fld_summit2->value(str.c_str());

  str = doubleToString(((ZAIC_PEAK_Model*)(m_zaic_model))->getPeakWidth2(),2);
  m_fld_pwidth2->value(str.c_str());

  str = doubleToString(((ZAIC_PEAK_Model*)(m_zaic_model))->getBaseWidth2(),2);
  m_fld_bwidth2->value(str.c_str());

  str = doubleToString(((ZAIC_PEAK_Model*)(m_zaic_model))->getSummitDelta2(),2);
  m_fld_sdelta2->value(str.c_str());


  str = doubleToString(((ZAIC_PEAK_Model*)(m_zaic_model))->getMaxUtil(),2);
  m_fld_maxutil->value(str.c_str());

  str = doubleToString(((ZAIC_PEAK_Model*)(m_zaic_model))->getMinUtil(),2);
  m_fld_minutil->value(str.c_str());

  updateFieldColors();
}


//---------------------------------------------------------
// Procedure: updateFieldColors()
//      Note: Make the currently editable field red. All others
//            go back to their starting colors.

void ZAIC_PEAK_GUI::updateFieldColors() 
{
  int curr_mode = ((ZAIC_PEAK_Model*)(m_zaic_model))->getCurrMode();

  Fl_Color fcolor_red  = fl_rgb_color(220, 140, 140);
  Fl_Color fcolor_blu  = fl_rgb_color(140, 140, 220);
  Fl_Color fcolor_wht  = fl_rgb_color(255, 255, 255);

  if(curr_mode == 0)
    m_fld_summit1->color(fcolor_red);
  else
    m_fld_summit1->color(fcolor_wht);

  if(curr_mode == 1)
    m_fld_pwidth1->color(fcolor_red);
  else
    m_fld_pwidth1->color(fcolor_wht);

  if(curr_mode == 2)
    m_fld_bwidth1->color(fcolor_red);
  else
    m_fld_bwidth1->color(fcolor_wht);

  if(curr_mode == 3)
    m_fld_sdelta1->color(fcolor_red);
  else
    m_fld_sdelta1->color(fcolor_wht);


  if(curr_mode == 4)
    m_fld_summit2->color(fcolor_red);
  else
    m_fld_summit2->color(fcolor_blu);

  if(curr_mode == 5)
    m_fld_pwidth2->color(fcolor_red);
  else
    m_fld_pwidth2->color(fcolor_blu);

  if(curr_mode == 6)
    m_fld_bwidth2->color(fcolor_red);
  else
    m_fld_bwidth2->color(fcolor_blu);

  if(curr_mode == 7)
    m_fld_sdelta2->color(fcolor_red);
  else
    m_fld_sdelta2->color(fcolor_blu);


  if(curr_mode == 8)
    m_fld_maxutil->color(fcolor_red);
  else
    m_fld_maxutil->color(fcolor_wht);

  if(curr_mode == 9)
    m_fld_minutil->color(fcolor_red);
  else
    m_fld_minutil->color(fcolor_wht);
}




