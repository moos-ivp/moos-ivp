/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_VECT_GUI.cpp                                    */
/*    DATE: May 16th, 2016                                       */
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

  m_min_util = new Fl_Output(100, q_height+top_marg+10, 50, 20, "min_util:"); 
  m_min_util->textsize(txt_size); 
  m_min_util->labelsize(txt_size);
  m_min_util->clear_visible_focus();

  m_max_util = new Fl_Output(200, q_height+top_marg+10, 50, 20, "min_util:"); 
  m_max_util->textsize(txt_size); 
  m_max_util->labelsize(txt_size);
  m_max_util->clear_visible_focus();

  m_fld_pieces = new Fl_Output(300, q_height+top_marg+10, 50, 20, "Pieces:"); 
  m_fld_pieces->textsize(txt_size); 
  m_fld_pieces->labelsize(txt_size);
  m_fld_pieces->clear_visible_focus();

  m_fld_tolerance = new Fl_Output(420, q_height+top_marg+10, 50, 20, "Tolerance:"); 
  m_fld_tolerance->textsize(txt_size); 
  m_fld_tolerance->labelsize(txt_size);
  m_fld_tolerance->clear_visible_focus();

  m_but_rebuild = new Fl_Button(520, q_height+top_marg+10, 50, 20, "rebuild"); 
  m_but_rebuild->labelsize(txt_size);
  m_but_rebuild->clear_visible_focus();
  m_but_rebuild->callback((Fl_Callback*)ZAIC_VECT_GUI::cb_ReBuild,(void*)0);

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

  augmentMenu();
  
  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------
// Procedure: augmentMenu()

void ZAIC_VECT_GUI::augmentMenu()
{
  mbar->add("Tolerance/Greater + 1.0", '}',
	    (Fl_Callback*)ZAIC_VECT_GUI::cb_Tolerance, (void*)10, 0);
  mbar->add("Tolerance/Greater + 0.1", ']',
	    (Fl_Callback*)ZAIC_VECT_GUI::cb_Tolerance, (void*)1, 0);
  mbar->add("Tolerance/Lower - 1.0", '{',
	    (Fl_Callback*)ZAIC_VECT_GUI::cb_Tolerance, (void*)-10, 0);
  mbar->add("Tolerance/Lower - 0.1", '[',
	    (Fl_Callback*)ZAIC_VECT_GUI::cb_Tolerance, (void*)-1, 0);
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

//--------------------------------------------------------- cb_ReBuild
inline void ZAIC_VECT_GUI::cb_ReBuild_i() {
  m_zaic_viewer->redraw();
  updateOutput();
}

void ZAIC_VECT_GUI::cb_ReBuild(Fl_Widget* o) {
  ((ZAIC_VECT_GUI*)(o->parent()->user_data()))->cb_ReBuild_i();
}

//--------------------------------------------------------- cb_Tolerance
inline void ZAIC_VECT_GUI::cb_Tolerance_i(int val) {

  cout << "OLD tolerance: " <<
    ((ZAIC_VECT_Model*)(m_zaic_model))->getTolerance() << endl;

  if(val == 10)
    ((ZAIC_VECT_Model*)(m_zaic_model))->modTolerance(1.0);
  else if(val == 1)
    ((ZAIC_VECT_Model*)(m_zaic_model))->modTolerance(0.1);
  else if(val ==-10)
    ((ZAIC_VECT_Model*)(m_zaic_model))->modTolerance(-1.0);
  else if(val == -1)
    ((ZAIC_VECT_Model*)(m_zaic_model))->modTolerance(-0.1);
  updateOutput();

  cout << "---New tolerance: " <<
    ((ZAIC_VECT_Model*)(m_zaic_model))->getTolerance() << endl;
}

void ZAIC_VECT_GUI::cb_Tolerance(Fl_Widget* o, int v) {
  ((ZAIC_VECT_GUI*)(o->parent()->user_data()))->cb_Tolerance_i(v);
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

  str = uintToString(m_zaic_viewer->getTotalPieces());
  m_fld_pieces->value(str.c_str());

  str = doubleToString(((ZAIC_VECT_Model*)(m_zaic_model))->getTolerance(),1);
  m_fld_tolerance->value(str.c_str());
}
