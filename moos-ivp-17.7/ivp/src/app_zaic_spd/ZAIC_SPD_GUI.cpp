/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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
  m_zaic_model->setDomain(501);

  m_zaic_viewer = new ZAIC_Viewer(sid_marg, top_marg, w()-(sid_marg*2), q_height);
  m_zaic_viewer->setModel(m_zaic_model);

  m_fld_lowval = new Fl_Output(50, row1, 60, 20, "lowval:"); 
  m_fld_lowval->textsize(txt_size); 
  m_fld_lowval->labelsize(txt_size);
  m_fld_lowval->clear_visible_focus();

  m_fld_medval = new Fl_Output(160, row1, 60, 20, "medval:"); 
  m_fld_medval->textsize(txt_size); 
  m_fld_medval->labelsize(txt_size);
  m_fld_medval->clear_visible_focus();

  m_fld_hghval = new Fl_Output(270, row1, 60, 20, "hghval:"); 
  m_fld_hghval->textsize(txt_size); 
  m_fld_hghval->labelsize(txt_size);
  m_fld_hghval->clear_visible_focus();

  m_fld_lowval_util = new Fl_Output(430, row1, 60, 20, "lowval_util:"); 
  m_fld_lowval_util->textsize(txt_size); 
  m_fld_lowval_util->labelsize(txt_size);
  m_fld_lowval_util->clear_visible_focus();

  m_fld_hghval_util = new Fl_Output(580, row1, 60, 20, "hghval_util:"); 
  m_fld_hghval_util->textsize(txt_size); 
  m_fld_hghval_util->labelsize(txt_size);
  m_fld_hghval_util->clear_visible_focus();  


  m_fld_lowmin_util = new Fl_Output(430, row2, 60, 20, "lowmin_util:"); 
  m_fld_lowmin_util->textsize(txt_size); 
  m_fld_lowmin_util->labelsize(txt_size);
  m_fld_lowmin_util->clear_visible_focus();

  m_fld_hghmin_util = new Fl_Output(580, row2, 60, 20, "hghmin_util:"); 
  m_fld_hghmin_util->textsize(txt_size); 
  m_fld_hghmin_util->labelsize(txt_size);
  m_fld_hghmin_util->clear_visible_focus();  

} 

//----------------------------------------------------------
// Procedure: augmentMenu

void ZAIC_SPD_GUI::augmentMenu()
{
  mbar->add("Modify Mode/Adjust MedVal ", '0',
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
}

//----------------------------------------------------------
// Procedure: updateOutput

void ZAIC_SPD_GUI::updateOutput() 
{
  string str;
  
  ZAIC_SPD_Model *model = (ZAIC_SPD_Model*)(m_zaic_model);
  
  str = doubleToString(model->getMedVal(),2);
  m_fld_medval->value(str.c_str());

  str = doubleToString(model->getLowVal(),2);
  m_fld_lowval->value(str.c_str());

  str = doubleToString(model->getHghVal(),2);
  m_fld_hghval->value(str.c_str());

  str = doubleToString(model->getLowValUtil(),2);
  m_fld_lowval_util->value(str.c_str());

  str = doubleToString(model->getHghValUtil(),2);
  m_fld_hghval_util->value(str.c_str());


  str = doubleToString(model->getLowMinUtil(),2);
  m_fld_lowmin_util->value(str.c_str());

  str = doubleToString(model->getHighMinUtil(),2);
  m_fld_hghmin_util->value(str.c_str());

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
    m_fld_medval->color(fcolor_red);
  else
    m_fld_medval->color(fcolor_wht);

  if(curr_mode == 1)
    m_fld_lowval->color(fcolor_red);
  else
    m_fld_lowval->color(fcolor_wht);

  if(curr_mode == 2)
    m_fld_hghval->color(fcolor_red);
  else
    m_fld_hghval->color(fcolor_wht);

  if(curr_mode == 3)
    m_fld_lowval_util->color(fcolor_red);
  else
    m_fld_lowval_util->color(fcolor_wht);

  if(curr_mode == 4)
    m_fld_hghval_util->color(fcolor_red);
  else
    m_fld_hghval_util->color(fcolor_wht);

  if(curr_mode == 5)
    m_fld_lowmin_util->color(fcolor_red);
  else
    m_fld_lowmin_util->color(fcolor_wht);

  if(curr_mode == 6)
    m_fld_hghmin_util->color(fcolor_red);
  else
    m_fld_hghmin_util->color(fcolor_wht);
}





