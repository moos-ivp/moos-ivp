/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_HLEQ_GUI.cpp                                    */
/*    DATE: Apr 6th, 2008                                        */
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

#include "ZAIC_HLEQ_GUI.h"
#include "ZAIC_HLEQ_Model.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

ZAIC_HLEQ_GUI::ZAIC_HLEQ_GUI(int g_w, int g_h, const char *g_l)
  : ZAIC_GUI(g_w, g_h, g_l)
{
  augmentMenu();
  initWidgets();
  
  this->end();
  this->resizable(this);
  this->show();
}

//----------------------------------------------------------
// Procedure: initWidgets()

void ZAIC_HLEQ_GUI::initWidgets()
{

  int txt_size=10;

  int top_marg = 30;
  int bot_marg = 45;
  int sid_marg = 5;
  int q_height = h()-(top_marg+bot_marg);

  int row1 = q_height+top_marg+10;
  
  m_zaic_model = new ZAIC_HLEQ_Model();
  m_zaic_model->setDomain(501);

  m_zaic_viewer = new ZAIC_Viewer(sid_marg, top_marg, w()-(sid_marg*2), q_height);
  m_zaic_viewer->setModel(m_zaic_model);

  m_fld_summit = new Fl_Output(60, row1, 60, 20, "summit:"); 
  m_fld_summit->textsize(txt_size); 
  m_fld_summit->labelsize(txt_size);
  m_fld_summit->clear_visible_focus();
  
  m_fld_bwidth = new Fl_Output(200, row1, 60, 20, "base_width:"); 
  m_fld_bwidth->textsize(txt_size); 
  m_fld_bwidth->labelsize(txt_size);
  m_fld_bwidth->clear_visible_focus();

  m_fld_minutil = new Fl_Output(350, row1, 60, 20, "minutil:"); 
  m_fld_minutil->textsize(txt_size); 
  m_fld_minutil->labelsize(txt_size);
  m_fld_minutil->clear_visible_focus();

  m_fld_maxutil = new Fl_Output(470, row1, 60, 20, "maxutil:"); 
  m_fld_maxutil->textsize(txt_size); 
  m_fld_maxutil->labelsize(txt_size);
  m_fld_maxutil->clear_visible_focus();

  m_fld_sumdelta = new Fl_Output(610, row1, 60, 20, "summit_delta:"); 
  m_fld_sumdelta->textsize(txt_size); 
  m_fld_sumdelta->labelsize(txt_size);
  m_fld_sumdelta->clear_visible_focus();
}


//----------------------------------------------------------
// Procedure: augmentMenu()

void ZAIC_HLEQ_GUI::augmentMenu()
{
  mbar->add("Modify Mode/Adjust Summit Position ", '0',
	    (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)0,
	    FL_MENU_RADIO|FL_MENU_VALUE);
  mbar->add("Modify Mode/Adjust BaseWidth ", '1',
	    (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)1, FL_MENU_RADIO);

  mbar->add("Modify Mode/Adjust MinUtil ", '2',
	    (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)2, FL_MENU_RADIO);

  mbar->add("Modify Mode/Adjust MaxUtil ", '3',
	    (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)3, FL_MENU_RADIO);

  mbar->add("Modify Mode/Adjust SummitDelta ", '4',
	    (Fl_Callback*)ZAIC_GUI::cb_CurrMode, (void*)4, FL_MENU_RADIO);
  
  mbar->add("Modify Mode/Toggle HLEQ ", 'z',
	    (Fl_Callback*)ZAIC_HLEQ_GUI::cb_ToggleHLEQ, 0, 0);
}

//----------------------------------------- ToggleHLEQ
inline void ZAIC_HLEQ_GUI::cb_ToggleHLEQ_i() {
  ((ZAIC_HLEQ_Model*)(m_zaic_model))->toggleHLEQ();
  updateOutput();
  m_zaic_viewer->redraw();
}
void ZAIC_HLEQ_GUI::cb_ToggleHLEQ(Fl_Widget* o) {
  ((ZAIC_HLEQ_GUI*)(o->parent()->user_data()))->cb_ToggleHLEQ_i();
}

//----------------------------------------- 
// Procedure: updateOutput()

void ZAIC_HLEQ_GUI::updateOutput() 
{
  string str;

  ZAIC_HLEQ_Model *model = (ZAIC_HLEQ_Model*)(m_zaic_model);

  str = doubleToString(model->getSummit(),2);
  m_fld_summit->value(str.c_str());

  str = doubleToString(model->getBaseWidth(),2);
  m_fld_bwidth->value(str.c_str());

  str = doubleToString(model->getMinUtil(),2);
  m_fld_minutil->value(str.c_str());

  str = doubleToString(model->getMaxUtil(),2);
  m_fld_maxutil->value(str.c_str());

  str = doubleToString(model->getSummitDelta(),2);
  m_fld_sumdelta->value(str.c_str());
}




