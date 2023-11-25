/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ZAIC_SPD_GUI.cpp                                     */
/*    DATE: June 2nd, 2015                                       */
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

#include "ZAIC_SPD_GUI.h"
#include "ZAIC_SPD_Model.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

ZAIC_SPD_GUI::ZAIC_SPD_GUI(int wid, int hgt, const char *label)
  : ZAIC_GUI(wid, hgt, label)
{
  augmentMenu();
  initWidgets();
  
  this->end();
  this->resizable(this);
  this->show();
}

//----------------------------------------------------------
// Procedure: initWidgets

void ZAIC_SPD_GUI::initWidgets()
{
  int txt_size = 10;
  int top_marg = 30;
  int bot_marg = 75;
  int sid_marg = 5;
  int q_height = h()-(top_marg+bot_marg);

  int row1 = q_height+top_marg+10;
  int row2 = row1 + 30;

  m_zaic_model = new ZAIC_SPD_Model();

  m_zaic_viewer = new ZAIC_Viewer(sid_marg, top_marg, w()-(sid_marg*2), q_height);
  m_zaic_viewer->setModel(m_zaic_model);

  m_fld_low_spd = new Fl_Output(50, row1, 60, 20, "low_spd:"); 
  m_fld_low_spd->textsize(txt_size); 
  m_fld_low_spd->labelsize(txt_size);
  m_fld_low_spd->clear_visible_focus();

  m_fld_med_spd = new Fl_Output(160, row1, 60, 20, "med_spd:"); 
  m_fld_med_spd->textsize(txt_size); 
  m_fld_med_spd->labelsize(txt_size);
  m_fld_med_spd->clear_visible_focus();

  m_fld_hgh_spd = new Fl_Output(270, row1, 60, 20, "hgh_spd:"); 
  m_fld_hgh_spd->textsize(txt_size); 
  m_fld_hgh_spd->labelsize(txt_size);
  m_fld_hgh_spd->clear_visible_focus();

  m_fld_low_spd_util = new Fl_Output(430, row1, 60, 20, "low_spd_util:"); 
  m_fld_low_spd_util->textsize(txt_size); 
  m_fld_low_spd_util->labelsize(txt_size);
  m_fld_low_spd_util->clear_visible_focus();

  m_fld_hgh_spd_util = new Fl_Output(580, row1, 60, 20, "hgh_spd_util:"); 
  m_fld_hgh_spd_util->textsize(txt_size); 
  m_fld_hgh_spd_util->labelsize(txt_size);
  m_fld_hgh_spd_util->clear_visible_focus();  


  m_fld_min_spd_util = new Fl_Output(430, row2, 60, 20, "min_spd_util:"); 
  m_fld_min_spd_util->textsize(txt_size); 
  m_fld_min_spd_util->labelsize(txt_size);
  m_fld_min_spd_util->clear_visible_focus();

  m_fld_max_spd_util = new Fl_Output(580, row2, 60, 20, "max_spd_util:"); 
  m_fld_max_spd_util->textsize(txt_size); 
  m_fld_max_spd_util->labelsize(txt_size);
  m_fld_max_spd_util->clear_visible_focus();  

} 

//----------------------------------------------------------
// Procedure: augmentMenu

void ZAIC_SPD_GUI::augmentMenu()
{
  mbar->add("Modify Mode/Adjust MedSpd ", '0',
	    (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)0,
	    FL_MENU_RADIO|FL_MENU_VALUE);

  mbar->add("Modify Mode/Adjust LowVal ", '1',
	    (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)1, FL_MENU_RADIO);

  mbar->add("Modify Mode/Adjust HighVal ",     '2',
	    (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)2, FL_MENU_RADIO);

  mbar->add("Modify Mode/Adjust LowValUtil ",  '3',
	    (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)3, FL_MENU_RADIO);

  mbar->add("Modify Mode/Adjust HighValUtil ", '4',
	    (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)4, FL_MENU_RADIO);

  mbar->add("Modify Mode/Adjust LowMinUtil ",  '5',
	    (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)5, FL_MENU_RADIO);

  mbar->add("Modify Mode/Adjust HighMinUtil ", '6',
	    (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)6, FL_MENU_RADIO);

  mbar->add("Snap/Snap LowSpd to MinSpd ", 'm',
	    (Fl_Callback*)ZAIC_SPD_GUI::cb_Snap, (void*)0, 0);

  mbar->add("Snap/Snap HgSpd to MaxSpd ", 'n',
	    (Fl_Callback*)ZAIC_SPD_GUI::cb_Snap, (void*)1, 0);
}

//----------------------------------------- Snap
inline void ZAIC_SPD_GUI::cb_Snap_i(int index) {
  ZAIC_SPD_Model *model = (ZAIC_SPD_Model*)(m_zaic_model);

  if(index == 0)
    model->disableLowSpeed();
  if(index == 1)
    model->disableHighSpeed();

  updateOutput();
  m_zaic_viewer->redraw();
}
void ZAIC_SPD_GUI::cb_Snap(Fl_Widget* o, int v) {
  ((ZAIC_SPD_GUI*)(o->parent()->user_data()))->cb_Snap_i(v);
}

//----------------------------------------------------------
// Procedure: updateOutput

void ZAIC_SPD_GUI::updateOutput() 
{
  string str;
  
  ZAIC_SPD_Model *model = (ZAIC_SPD_Model*)(m_zaic_model);
  
  str = doubleToString(model->getMedVal(),2);
  m_fld_med_spd->value(str.c_str());

  str = doubleToString(model->getLowVal(),2);
  m_fld_low_spd->value(str.c_str());

  str = doubleToString(model->getHghVal(),2);
  m_fld_hgh_spd->value(str.c_str());

  str = doubleToString(model->getLowValUtil(),2);
  m_fld_low_spd_util->value(str.c_str());

  str = doubleToString(model->getHghValUtil(),2);
  m_fld_hgh_spd_util->value(str.c_str());

  str = doubleToString(model->getLowMinUtil(),2);
  m_fld_min_spd_util->value(str.c_str());

  str = doubleToString(model->getHighMinUtil(),2);
  m_fld_max_spd_util->value(str.c_str());

  model->print();
  updateFieldColors();
}


//---------------------------------------------------------
// Procedure: updateFieldColors()
//      Note: Make the currently editable field red. All others
//            go back to their starting colors.

void ZAIC_SPD_GUI::updateFieldColors() 
{
  int curr_mode = ((ZAIC_SPD_Model*)(m_zaic_model))->getCurrMode();

  Fl_Color fcolor_red  = fl_rgb_color(220, 140, 140);
  Fl_Color fcolor_wht  = fl_rgb_color(255, 255, 255);

  if(curr_mode == 0)
    m_fld_med_spd->color(fcolor_red);
  else
    m_fld_med_spd->color(fcolor_wht);

  if(curr_mode == 1)
    m_fld_low_spd->color(fcolor_red);
  else
    m_fld_low_spd->color(fcolor_wht);

  if(curr_mode == 2)
    m_fld_hgh_spd->color(fcolor_red);
  else
    m_fld_hgh_spd->color(fcolor_wht);

  if(curr_mode == 3)
    m_fld_low_spd_util->color(fcolor_red);
  else
    m_fld_low_spd_util->color(fcolor_wht);

  if(curr_mode == 4)
    m_fld_hgh_spd_util->color(fcolor_red);
  else
    m_fld_hgh_spd_util->color(fcolor_wht);

  if(curr_mode == 5)
    m_fld_min_spd_util->color(fcolor_red);
  else
    m_fld_min_spd_util->color(fcolor_wht);

  if(curr_mode == 6)
    m_fld_max_spd_util->color(fcolor_red);
  else
    m_fld_max_spd_util->color(fcolor_wht);
}






