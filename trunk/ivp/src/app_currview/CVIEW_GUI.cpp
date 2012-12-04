/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CVIEW_GUI.cpp                                        */
/*    DATE: January 4th, 2011                                    */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include <cstdio>
#include "CVIEW_GUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

CVIEW_GUI::CVIEW_GUI(int g_w, int g_h, const char *g_l)
  : MarineGUI(g_w, g_h, g_l) {

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  int info_size=10;

  pviewer   = new CurrViewer(0, 30, w(), h()-100);
  m_mviewer = pviewer;

  augmentMenu();

  v_ang = new MY_Output(60, h()-60, 70, 20, "direction:"); 
  v_ang->textsize(info_size); 
  v_ang->labelsize(info_size);

  v_mag = new MY_Output(60, h()-30, 70, 20, "magnitude:"); 
  v_mag->textsize(info_size); 
  v_mag->labelsize(info_size);

  x_mtr = new MY_Output(200, h()-60, 70, 20, "meters x:"); 
  x_mtr->textsize(info_size); 
  x_mtr->labelsize(info_size);

  y_mtr = new MY_Output(200, h()-30, 70, 20, "meters y:"); 
  y_mtr->textsize(info_size); 
  y_mtr->labelsize(info_size);

  v_lat = new MY_Output(340, h()-60, 70, 20, "latitude:"); 
  v_lat->textsize(info_size); 
  v_lat->labelsize(info_size);

  v_lon = new MY_Output(340, h()-30, 70, 20, "longitude:"); 
  v_lon->textsize(info_size); 
  v_lon->labelsize(info_size);

  v_amt = new MY_Output(480, h()-60, 70, 20, "Num vectors:"); 
  v_amt->textsize(info_size); 
  v_amt->labelsize(info_size);

  v_idx = new MY_Output(480, h()-30, 70, 20, "Edit vector:"); 
  v_idx->textsize(info_size); 
  v_idx->labelsize(info_size);

  p_snap = new MY_Output(620, h()-30, 70, 20, "snap:"); 
  p_snap->textsize(info_size); 
  p_snap->labelsize(info_size);

  this->end();
  this->resizable(this);
  this->show();
}

//-------------------------------------------------------------------
// Procedure: augmentMenu

void CVIEW_GUI::augmentMenu() 
{
  m_menubar->add("File/Save", FL_CTRL + 's', (Fl_Callback*)CVIEW_GUI::cb_FileSave, (void*)0, 0);

  m_menubar->add("File/Save As", 0, (Fl_Callback*)CVIEW_GUI::cb_FileSaveAs, (void*)0, 0);

  m_menubar->add("Vectors/Clear Marked", 0, (Fl_Callback*)CVIEW_GUI::cb_Clear, (void*)0, 0);
  m_menubar->add("Vectors/Delete Marked", FL_BackSpace, (Fl_Callback*)CVIEW_GUI::cb_DeleteMarked, (void*)0, 0);
  m_menubar->add("Vectors/Delete Last", FL_CTRL + FL_BackSpace, (Fl_Callback*)CVIEW_GUI::cb_DeleteLastAdded, (void*)0, 0);
  m_menubar->add("Vectors/Current --", '-', (Fl_Callback*)CVIEW_GUI::cb_AdjustActive, (void*)-1, 0);
  m_menubar->add("Vectors/Current ++", '+', (Fl_Callback*)CVIEW_GUI::cb_AdjustActive, (void*)1, 0);

  m_menubar->add("Vectors/Move Right", FL_SHIFT + FL_Right, (Fl_Callback*)CVIEW_GUI::cb_ShiftX, (void*)10, 0);
  m_menubar->add("Vectors/Move Left",  FL_SHIFT + FL_Left, (Fl_Callback*)CVIEW_GUI::cb_ShiftX, (void*)-10, 0);
  m_menubar->add("Vectors/Move Up",    FL_SHIFT + FL_Up, (Fl_Callback*)CVIEW_GUI::cb_ShiftY, (void*)10, 0);
  m_menubar->add("Vectors/Move Down",  FL_SHIFT + FL_Down, (Fl_Callback*)CVIEW_GUI::cb_ShiftY, (void*)-10, FL_MENU_DIVIDER);
  m_menubar->add("Vectors/Move Right (slow)", FL_SHIFT + FL_ALT + FL_Right, (Fl_Callback*)CVIEW_GUI::cb_ShiftX, (void*)1, 0);
  m_menubar->add("Vectors/Move Left (slow)",  FL_SHIFT + FL_ALT + FL_Left, (Fl_Callback*)CVIEW_GUI::cb_ShiftX, (void*)-1, 0);
  m_menubar->add("Vectors/Move Up (slow)",    FL_SHIFT + FL_ALT + FL_Up, (Fl_Callback*)CVIEW_GUI::cb_ShiftY, (void*)1, 0);
  m_menubar->add("Vectors/Move Down (slow)",  FL_SHIFT + FL_ALT + FL_Down, (Fl_Callback*)CVIEW_GUI::cb_ShiftY, (void*)-1, FL_MENU_DIVIDER);
  
  m_menubar->add("Vectors/Rotate CWise", ']', (Fl_Callback*)CVIEW_GUI::cb_Rotate, (void*)10, 0);
  m_menubar->add("Vectors/Rotate CCWise",  '[', (Fl_Callback*)CVIEW_GUI::cb_Rotate, (void*)-10, 0);
  m_menubar->add("Vectors/Rotate CWise (slow)", '}', (Fl_Callback*)CVIEW_GUI::cb_Rotate, (void*)1, 0);
  m_menubar->add("Vectors/Rotate CCwise (slow)",  '{', (Fl_Callback*)CVIEW_GUI::cb_Rotate, (void*)-1, FL_MENU_DIVIDER);
  
  m_menubar->add("Vectors/Magnitude Grow", ')', (Fl_Callback*)CVIEW_GUI::cb_Magnitude, (void*)1, 0);
  m_menubar->add("Vectors/Magnitude Shrink",'(', (Fl_Callback*)CVIEW_GUI::cb_Magnitude, (void*)-1, 0);
  
  m_menubar->add("SnapValue/200.0 meters", 0, (Fl_Callback*)CVIEW_GUI::cb_SnapVal, (void*)20000, FL_MENU_RADIO);
  m_menubar->add("SnapValue/100.0 meters", 0, (Fl_Callback*)CVIEW_GUI::cb_SnapVal, (void*)10000, FL_MENU_RADIO);
  m_menubar->add("SnapValue/50.0 meters", 0, (Fl_Callback*)CVIEW_GUI::cb_SnapVal, (void*)5000, FL_MENU_RADIO);
  m_menubar->add("SnapValue/10.0 meters", 0, (Fl_Callback*)CVIEW_GUI::cb_SnapVal, (void*)1000, FL_MENU_RADIO);
  m_menubar->add("SnapValue/ 5.0 meters", 0, (Fl_Callback*)CVIEW_GUI::cb_SnapVal, (void*)500, FL_MENU_RADIO);
  m_menubar->add("SnapValue/ 2.0 meters", 0, (Fl_Callback*)CVIEW_GUI::cb_SnapVal, (void*)200, FL_MENU_RADIO);
  m_menubar->add("SnapValue/ 1.0 meters", 0, (Fl_Callback*)CVIEW_GUI::cb_SnapVal, (void*)100, FL_MENU_RADIO|FL_MENU_VALUE);
  m_menubar->add("SnapValue/ 0.1 meters", 0, (Fl_Callback*)CVIEW_GUI::cb_SnapVal, (void*)10, FL_MENU_RADIO);
  m_menubar->add("SnapValue/ 0.01 meters", 0, (Fl_Callback*)CVIEW_GUI::cb_SnapVal, (void*)1, FL_MENU_RADIO);
  m_menubar->add("SnapValue/ 0.00 meters", 0, (Fl_Callback*)CVIEW_GUI::cb_SnapVal, (void*)0, FL_MENU_RADIO);
  m_menubar->add("SnapValue/Apply SnapVal", 0, (Fl_Callback*)CVIEW_GUI::cb_ApplySnapVal, (void*)0, 0);
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

int CVIEW_GUI::handle(int event) 
{
  switch(event) {
  case FL_PUSH:
    Fl_Window::handle(event);
    updateXY();
    return(1);
    break;
  default:
    return(Fl_Window::handle(event));
  }
}

//----------------------------------------- Clear
inline void CVIEW_GUI::cb_Clear_i() {
  pviewer->clearMarked();
  updateXY();
  pviewer->redraw();
}
void CVIEW_GUI::cb_Clear(Fl_Widget* o) {
  ((CVIEW_GUI*)(o->parent()->user_data()))->cb_Clear_i();
}

//----------------------------------------- DeleteMarked
inline void CVIEW_GUI::cb_DeleteMarked_i() {
  pviewer->deleteMarked();
  updateXY();
  pviewer->redraw();
}
void CVIEW_GUI::cb_DeleteMarked(Fl_Widget* o) {
  ((CVIEW_GUI*)(o->parent()->user_data()))->cb_DeleteMarked_i();
}

//----------------------------------------- DeleteLast
inline void CVIEW_GUI::cb_DeleteLastAdded_i() {
  pviewer->deleteLastAdded();
  updateXY();
  pviewer->redraw();
}
void CVIEW_GUI::cb_DeleteLastAdded(Fl_Widget* o) {
  ((CVIEW_GUI*)(o->parent()->user_data()))->cb_DeleteLastAdded_i();
}

//----------------------------------------- FileSave
inline void CVIEW_GUI::cb_FileSave_i() {
  pviewer->fileSave();
  updateXY();
  pviewer->redraw();
}
void CVIEW_GUI::cb_FileSave(Fl_Widget* o) {
  ((CVIEW_GUI*)(o->parent()->user_data()))->cb_FileSave_i();
}

//----------------------------------------- FileSaveAs
inline void CVIEW_GUI::cb_FileSaveAs_i() {
  pviewer->fileSaveAs();
  updateXY();
  pviewer->redraw();
}
void CVIEW_GUI::cb_FileSaveAs(Fl_Widget* o) {
  ((CVIEW_GUI*)(o->parent()->user_data()))->cb_FileSaveAs_i();
}

//----------------------------------------- AdjustActive
inline void CVIEW_GUI::cb_AdjustActive_i(int v) {
  if(v<0)
    pviewer->decrementActive();
  if(v>0)
    pviewer->incrementActive();
  updateXY();
  pviewer->redraw();
}
void CVIEW_GUI::cb_AdjustActive(Fl_Widget* o, int v) {
  ((CVIEW_GUI*)(o->parent()->user_data()))->cb_AdjustActive_i(v);
}

//----------------------------------------- ShiftX
inline void CVIEW_GUI::cb_ShiftX_i(int amt) {
  pviewer->shiftHorzVector((float)(amt)/10);
  updateXY();
  pviewer->redraw();
}
void CVIEW_GUI::cb_ShiftX(Fl_Widget* o, int v) {
  ((CVIEW_GUI*)(o->parent()->user_data()))->cb_ShiftX_i(v);
}

//----------------------------------------- ShiftY
inline void CVIEW_GUI::cb_ShiftY_i(int amt) {
  pviewer->shiftVertVector((float)(amt)/10);
  updateXY();
  pviewer->redraw();
}
void CVIEW_GUI::cb_ShiftY(Fl_Widget* o, int v) {
  ((CVIEW_GUI*)(o->parent()->user_data()))->cb_ShiftY_i(v);
}

//----------------------------------------- Rotate
inline void CVIEW_GUI::cb_Rotate_i(int amt) {
  pviewer->rotateVector((float)(amt)/10);
  updateXY();
  pviewer->redraw();
}
void CVIEW_GUI::cb_Rotate(Fl_Widget* o, int v) {
  ((CVIEW_GUI*)(o->parent()->user_data()))->cb_Rotate_i(v);
}

//----------------------------------------- Magnitude
inline void CVIEW_GUI::cb_Magnitude_i(int amt) {
  pviewer->growVector((float)(amt)/100);
  updateXY();
  pviewer->redraw();
}
void CVIEW_GUI::cb_Magnitude(Fl_Widget* o, int v) {
  ((CVIEW_GUI*)(o->parent()->user_data()))->cb_Magnitude_i(v);
}

//----------------------------------------- SnapVal
inline void CVIEW_GUI::cb_SnapVal_i(int v) {
  pviewer->setParam("snap", v/100.0);
  updateXY();
  pviewer->redraw();
}
void CVIEW_GUI::cb_SnapVal(Fl_Widget* o, int v) {
  ((CVIEW_GUI*)(o->parent()->user_data()))->cb_SnapVal_i(v);
}

//----------------------------------------- ApplySnapVal
inline void CVIEW_GUI::cb_ApplySnapVal_i() {
  pviewer->applySnap();
  updateXY();
  pviewer->redraw();
}
void CVIEW_GUI::cb_ApplySnapVal(Fl_Widget* o) {
  ((CVIEW_GUI*)(o->parent()->user_data()))->cb_ApplySnapVal_i();
}

//----------------------------------------- UpdateXY
void CVIEW_GUI::updateXY() {
  
  //int index = -1;
  char buff[64];

  string x_str = pviewer->getInfo("v_x", 2);
  string y_str = pviewer->getInfo("v_y", 2);
  x_mtr->value(x_str.c_str());
  y_mtr->value(y_str.c_str());

  string vlat_str = pviewer->getInfo("v_lat", 7);
  string vlon_str = pviewer->getInfo("v_lon", 7);
  string vang_str = pviewer->getInfo("v_ang", 2);
  string vmag_str = pviewer->getInfo("v_mag", 2);
  string vidx_str = pviewer->getInfo("v_idx", 2);
  string vcnt_str = pviewer->getInfo("v_cnt", 2);

  v_lat->value(vlat_str.c_str());
  v_lon->value(vlon_str.c_str());

  v_amt->value(vcnt_str.c_str());
  v_idx->value(vidx_str.c_str());

  v_ang->value(vang_str.c_str());
  v_mag->value(vmag_str.c_str());

  // Snap Value
  sprintf(buff, "%f", pviewer->getSnap());
  p_snap->value(buff);
}


