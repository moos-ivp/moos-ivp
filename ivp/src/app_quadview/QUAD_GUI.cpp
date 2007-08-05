/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: QUAD_GUI.cpp                                         */
/*    DATE: June 20th, 2005                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "QUAD_GUI.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor

QUAD_GUI::QUAD_GUI(int g_w, int g_h, const char *g_l)
  : Fl_Window(g_w, g_h, g_l) {

  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();

  mbar = new Fl_Menu_Bar(0, 0, w(), 25);
  mbar->menu(menu_);
    
  int info_size=10;

  int top_marg = 30;
  int bot_marg = 80;
  int sid_marg = 5;
  int q_height = h()-(top_marg+bot_marg);
  int q_width  = w()-(sid_marg*2);

  q_viewer = new QuadViewer(sid_marg, top_marg, w()-(sid_marg*2), q_height);

  pt1 = new MY_Output(30, q_height+top_marg+10, 80, 20, "pt1:"); 
  pt1->textsize(info_size); 
  pt1->labelsize(info_size);

  pt2 = new MY_Output(160, q_height+top_marg+10, 80, 20, "pt2:"); 
  pt2->textsize(info_size); 
  pt2->labelsize(info_size);

  pt3 = new MY_Output(290, q_height+top_marg+10, 80, 20, "pt3:"); 
  pt3->textsize(info_size); 
  pt3->labelsize(info_size);

  x2 = new MY_Output(30, q_height+top_marg+40, 80, 20, "m:"); 
  x2->textsize(info_size); 
  x2->labelsize(info_size);

  x1 = new MY_Output(160, q_height+top_marg+40, 80, 20, "n:"); 
  x1->textsize(info_size); 
  x1->labelsize(info_size);

  x0 = new MY_Output(290, q_height+top_marg+40, 80, 20, "b:"); 
  x0->textsize(info_size); 
  x0->labelsize(info_size);


  min_x = new MY_Output(w()-180, q_height+top_marg+10, 60, 20, "x:"); 
  min_x->textsize(info_size); 
  min_x->labelsize(info_size);

  min_y = new MY_Output(w()-70, q_height+top_marg+10, 60, 20, "min(x):"); 
  min_y->textsize(info_size); 
  min_y->labelsize(info_size);

  max_x = new MY_Output(w()-180, q_height+top_marg+40, 60, 20, "x:"); 
  max_x->textsize(info_size); 
  max_x->labelsize(info_size);

  max_y = new MY_Output(w()-70, q_height+top_marg+40, 60, 20, "max(x):"); 
  max_y->textsize(info_size); 
  max_y->labelsize(info_size);

  this->end();
  this->resizable(this);
  this->show();
}

Fl_Menu_Item QUAD_GUI::menu_[] = {
 {"File", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Quit ", FL_CTRL+'q', (Fl_Callback*)QUAD_GUI::cb_Quit, 0, 0},
 {0},

 {"Select Point", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Current Left ", '1', (Fl_Callback*)QUAD_GUI::cb_CurrPt, (void*)1, FL_MENU_RADIO|FL_MENU_VALUE},
 {"Current Middle ", '2', (Fl_Callback*)QUAD_GUI::cb_CurrPt, (void*)2, FL_MENU_RADIO},
 {"Current Right ", '3', (Fl_Callback*)QUAD_GUI::cb_CurrPt, (void*)3, FL_MENU_RADIO},
 {0},

 {"Move Point", 0,  0, 0, 64, 0, 0, 14, 0},
 {"Move Up    ", FL_Up, (Fl_Callback*)QUAD_GUI::cb_MoveY, (void*)1, 0},
 {"Move Down  ", FL_Down, (Fl_Callback*)QUAD_GUI::cb_MoveY, (void*)-1, 0},
 {"Move Left  ", FL_Left, (Fl_Callback*)QUAD_GUI::cb_MoveX, (void*)-1, 0},
 {"Move Right ", FL_Right, (Fl_Callback*)QUAD_GUI::cb_MoveX, (void*)1, 0},
 {0},

 {0}
};

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

int QUAD_GUI::handle(int event) 
{
  int step = 1;
  switch(event) {
  case FL_PUSH:
    Fl_Window::handle(event);
    updateOutput();
    return(1);
    break;
  default:
    return(Fl_Window::handle(event));
  }
}

//----------------------------------------- Quit
void QUAD_GUI::cb_Quit() {
  exit(0);
}

//----------------------------------------- Move X
inline void QUAD_GUI::cb_MoveX_i(int amt) {
  q_viewer->move_x(amt);
  updateOutput();
  q_viewer->redraw();
}
void QUAD_GUI::cb_MoveX(Fl_Widget* o, int v) {
  ((QUAD_GUI*)(o->parent()->user_data()))->cb_MoveX_i(v);
}

//----------------------------------------- Move Y
inline void QUAD_GUI::cb_MoveY_i(int amt) {
  q_viewer->move_y(amt);
  updateOutput();
  q_viewer->redraw();
}
void QUAD_GUI::cb_MoveY(Fl_Widget* o, int v) {
  ((QUAD_GUI*)(o->parent()->user_data()))->cb_MoveY_i(v);
}

//----------------------------------------- CurrPt
inline void QUAD_GUI::cb_CurrPt_i(int index) {
  q_viewer->curr_pt(index);
  updateOutput();
  q_viewer->redraw();
}
void QUAD_GUI::cb_CurrPt(Fl_Widget* o, int v) {
  ((QUAD_GUI*)(o->parent()->user_data()))->cb_CurrPt_i(v);
}


//----------------------------------------- UpdateOutput
void QUAD_GUI::updateOutput() 
{
  string str;
  str = q_viewer->get_pstring(1);
  pt1->value(str.c_str());

  str = q_viewer->get_pstring(2);
  pt2->value(str.c_str());

  str = q_viewer->get_pstring(3);
  pt3->value(str.c_str());

  str = q_viewer->get_coeff(0);
  x0->value(str.c_str());

  str = q_viewer->get_coeff(1);
  x1->value(str.c_str());

  str = q_viewer->get_coeff(2);
  x2->value(str.c_str());

  double dmin_y = q_viewer->get_min_y();
  str = dstringCompact(doubleToString(dmin_y));
  min_y->value(str.c_str());

  double dmin_x = q_viewer->get_min_x();
  str = dstringCompact(doubleToString(dmin_x));
  min_x->value(str.c_str());

  double dmax_y = q_viewer->get_max_y();
  str = dstringCompact(doubleToString(dmax_y));
  max_y->value(str.c_str());

  double dmax_x = q_viewer->get_max_x();
  str = dstringCompact(doubleToString(dmax_x));
  max_x->value(str.c_str());
}





