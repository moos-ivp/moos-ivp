/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_VECT_GUI.cpp                                    */
/*    DATE: May 16th, 2016                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include "ZAIC_VECT_GUI.h"
#include "ZAIC_VECT_Model.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

ZAIC_VECT_GUI::ZAIC_VECT_GUI(int g_w, int g_h, const char *g_l)
  : ZAIC_GUI(g_w, g_h, g_l) {

  int txt_size = 10;
  int top_marg = 30;
  int bot_marg = 45;
  int sid_marg = 5;
  int q_height = h()-(top_marg+bot_marg);
  //int q_width  = w()-(sid_marg*2);

  m_zaic_model = new ZAIC_VECT_Model();
  m_zaic_model->setDomain(501);

  m_zaic_viewer = new ZAIC_Viewer(sid_marg, top_marg, w()-(sid_marg*2), q_height);
  m_zaic_viewer->setModel(m_zaic_model);

  m_min_util = new Fl_Output(100, q_height+top_marg+10, 60, 20, "min_util:"); 
  m_min_util->textsize(txt_size); 
  m_min_util->labelsize(txt_size);
  m_min_util->clear_visible_focus();

  m_max_util = new Fl_Output(200, q_height+top_marg+10, 60, 20, "min_util:"); 
  m_max_util->textsize(txt_size); 
  m_max_util->labelsize(txt_size);
  m_max_util->clear_visible_focus();

  // Really no editing in this GUI - even the move left/right disabled here.
  int index = mbar->find_index("Modify ZAIC/Move Left  ");
  if(index != -1)
    mbar->remove(index);
  index = mbar->find_index("Modify ZAIC/Move Right ");
  if(index != -1)
    mbar->remove(index);
  index = mbar->find_index("Modify ZAIC");
  if(index != -1)
    mbar->remove(index);

  this->end();
  this->resizable(this);
  this->show();
}

//--------------------------------------------------------------
// Procedure: setZAIC()

void ZAIC_VECT_GUI::setZAIC(ZAIC_Vector* zaic)
{
  if(!m_zaic_model)
    return;
  
  ((ZAIC_VECT_Model*)(m_zaic_model))->setZAIC(zaic);
  updateOutput();
}

//--------------------------------------------------------------
// Procedure: setVerbose()

void ZAIC_VECT_GUI::setVerbose(bool verbose)
{
  if(m_zaic_viewer)
    m_zaic_viewer->setVerbose(verbose);
}

//--------------------------------------------------------------
// Procedure: updateOutput

void ZAIC_VECT_GUI::updateOutput() 
{
  string str;

  str = doubleToString(((ZAIC_VECT_Model*)(m_zaic_model))->getMinUtil(),2);
  m_min_util->value(str.c_str());

  str = doubleToString(((ZAIC_VECT_Model*)(m_zaic_model))->getMaxUtil(),2);
  m_max_util->value(str.c_str());
}
