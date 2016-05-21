/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_HLEQ_GUI.cpp                                    */
/*    DATE: Apr 6th, 2008                                        */
/*    DATE: May 10th, 2016 Refactored to use common superclass   */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include "ZAIC_HLEQ_GUI.h"
#include "ZAIC_HLEQ_Model.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

ZAIC_HLEQ_GUI::ZAIC_HLEQ_GUI(int g_w, int g_h, const char *g_l)
  : ZAIC_GUI(g_w, g_h, g_l) {

  int info_size=10;

  int top_marg = 30;
  int bot_marg = 45;
  int sid_marg = 5;
  int q_height = h()-(top_marg+bot_marg);
  int q_width  = w()-(sid_marg*2);

  m_zaic_model = new ZAIC_HLEQ_Model();
  m_zaic_model->setDomain(501);

  m_zaic_viewer = new ZAIC_Viewer(sid_marg, top_marg, w()-(sid_marg*2), q_height);
  m_zaic_viewer->setModel(m_zaic_model);

  m_fld_summit = new Fl_Output(60, q_height+top_marg+10, 60, 20, "summit:"); 
  m_fld_summit->textsize(info_size); 
  m_fld_summit->labelsize(info_size);
  m_fld_summit->clear_visible_focus();
  
  m_fld_bwidth = new Fl_Output(200, q_height+top_marg+10, 60, 20, "base_width:"); 
  m_fld_bwidth->textsize(info_size); 
  m_fld_bwidth->labelsize(info_size);
  m_fld_bwidth->clear_visible_focus();

  m_fld_minutil = new Fl_Output(350, q_height+top_marg+10, 60, 20, "minutil:"); 
  m_fld_minutil->textsize(info_size); 
  m_fld_minutil->labelsize(info_size);
  m_fld_minutil->clear_visible_focus();

  m_fld_maxutil = new Fl_Output(470, q_height+top_marg+10, 60, 20, "maxutil:"); 
  m_fld_maxutil->textsize(info_size); 
  m_fld_maxutil->labelsize(info_size);
  m_fld_maxutil->clear_visible_focus();

  m_fld_sumdelta = new Fl_Output(610, q_height+top_marg+10, 60, 20, "summit_delta:"); 
  m_fld_sumdelta->textsize(info_size); 
  m_fld_sumdelta->labelsize(info_size);
  m_fld_sumdelta->clear_visible_focus();

  augmentMenu();
  
  this->end();
  this->resizable(this);
  this->show();
}

//----------------------------------------------------------
// Procedure: augmentMenu

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

//----------------------------------------- UpdateOutput
void ZAIC_HLEQ_GUI::updateOutput() 
{
  string str;

  str = doubleToString(((ZAIC_HLEQ_Model*)(m_zaic_model))->getSummit(),2);
  m_fld_summit->value(str.c_str());

  str = doubleToString(((ZAIC_HLEQ_Model*)(m_zaic_model))->getBaseWidth(),2);
  m_fld_bwidth->value(str.c_str());

  str = doubleToString(((ZAIC_HLEQ_Model*)(m_zaic_model))->getMinUtil(),2);
  m_fld_minutil->value(str.c_str());

  str = doubleToString(((ZAIC_HLEQ_Model*)(m_zaic_model))->getMaxUtil(),2);
  m_fld_maxutil->value(str.c_str());

  str = doubleToString(((ZAIC_HLEQ_Model*)(m_zaic_model))->getSummitDelta(),2);
  m_fld_sumdelta->value(str.c_str());
}