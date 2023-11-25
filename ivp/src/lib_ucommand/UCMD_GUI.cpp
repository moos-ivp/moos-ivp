/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: UCMD_GUI.cpp                                         */
/*    DATE: July 1st, 2016                                       */
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
#include "UCMD_GUI.h"
#include "MBUtils.h"
#include "ColorPack.h"
#include "ColorParse.h"
#include "ACTable.h"
#include "FL/fl_draw.H"

using namespace std;

//-------------------------------------------------------------------
// Constructor

UCMD_GUI::UCMD_GUI(int wid, int hgt, const char *label, bool show_posts)
  : Fl_Window(wid, hgt, label)
{
  this->user_data((void*)(this));
  this->when(FL_WHEN_CHANGED);
  this->begin();
  this->size_range(350,250);

  m_menubar = new Fl_Menu_Bar(0, 0, w(), 20);
  m_menubar->textsize(12);

  augmentMenu();

  m_start_wid = w();
  m_select_vname = "all";
  m_display_mode = "all";
  m_show_posts   = true;
  m_brw_hgt      = 120;
  m_show_posts   = show_posts;
  
  m_but_post_count = 0;
  m_cmd_post_count = 0;

  m_concede_top = false;

  m_max_label_len = 0;
  
  initWidgets();
  resizeWidgetsShape();
  resizeWidgetsText();

  this->resizable(this);
  this->end();
  this->show();
}

//----------------------------------------------------------
// Procedure: augmentMenu

void UCMD_GUI::augmentMenu() 
{
  m_menubar->add("File/Quit ", FL_ALT+'q',
		 (Fl_Callback*)UCMD_GUI::cb_Quit, 0, 0);
  m_menubar->add("File/Close ", FL_META+'w',
		 (Fl_Callback*)UCMD_GUI::cb_Close, 0, 0);
  m_menubar->add("View/Toggle PostView", 'p',
		 (Fl_Callback*)UCMD_GUI::cb_TogglePostView, 0, 0);
  m_menubar->add("File/ConcedeTopWindow ", ' ',
		 (Fl_Callback*)UCMD_GUI::cb_ConcedeTopWindow, 0, 0);
}

//----------------------------------------------------------
// Procedure: resize

void UCMD_GUI::resize(int lx, int ly, int lw, int lh)
{
  Fl_Window::resize(lx, ly, lw, lh);
  resizeWidgetsShape();
  resizeWidgetsText();
  roomToView();
}

//----------------------------------------------------------
// Procedure: roomToView
//   Purpose: Possibly expand the height of the window if the 
//            bottom most buttons dont fit.

void UCMD_GUI::roomToView()
{
  //return; // Note Bug fix on Conlan's machine - reinvestigate
  int winx = Fl_Window::x();
  int winy = Fl_Window::y();
  int winw = Fl_Window::w();
  int winh = Fl_Window::h();

  int boty = 25;
  if(m_row_bottoms.size() != 0)
    boty = m_row_bottoms.back() + 10;
  if(m_show_posts)
    boty = m_brw_posts->y() + m_brw_posts->h() + 10;

  if(winh < boty)
    Fl_Window::resize(winx, winy, winw, boty);

  if((m_cmd_folio.size() > 0) && (winh > boty))
    Fl_Window::resize(winx, winy, winw, boty);
}

//----------------------------------------------------------
// Procedure: draw

void UCMD_GUI::draw()
{
  Fl_Window::draw();

  Fl_Color fl_grey = fl_rgb_color(127, 127, 150); 
  Fl_Color fl_dark_grey = fl_rgb_color(60, 60, 100); 

  for(unsigned int i=0; i<m_row_bottoms.size(); i++) {
    int cw = w() - 10;
    int cx = 5;
    int cy = 25;
    if(i != 0) 
      cy = m_row_bottoms[i-1] + 10;
    int ch = (m_row_bottoms[i] - cy) + 5;
    if((m_row_vnames[i] == "all") || (m_row_vnames[i] == "shore"))
      fl_draw_box(FL_BORDER_FRAME, cx, cy, cw, ch, FL_BLUE);
    else
      fl_draw_box(FL_BORDER_FRAME, cx, cy, cw, ch, fl_grey);
    fl_font(FL_HELVETICA, 14);
    fl_color(fl_dark_grey);
    fl_draw(m_row_vnames[i].c_str(), cx+20, cy+20);
  }
}

//----------------------------------------------------------
// Procedure: setCommandFolio()

void UCMD_GUI::setCommandFolio(CommandFolio folio)
{
  m_cmd_folio = folio;
  initWidgetsFolio();
  resizeWidgetsShape();
  resizeWidgetsText();
  roomToView();
  redraw();
}


//----------------------------------------------------------
// Procedure: setPostSummary()

void UCMD_GUI::setPostSummary(const vector<string>& summary)
{
  m_brw_posts->clear();

  if(summary.size() == 0)
    return;
  
  for(unsigned i=0; i<summary.size(); i++) 
    m_brw_posts->add(summary[i].c_str());

  //m_brw_posts->bottomline(m_brw_posts->size());
  
}

//----------------------------------------------------------
// Procedure: getConcedeTop()

bool UCMD_GUI::getConcedeTop()
{
  bool return_val = m_concede_top;

  m_concede_top = false;
  return(return_val);
}


//----------------------------------------------------------
// Procedure: initWidgets()

void UCMD_GUI::initWidgets() 
{
  m_brw_posts = new Fl_Hold_Browser(0, 0, 1, 1); 
  m_brw_posts->clear_visible_focus();
  m_brw_posts->align(FL_ALIGN_LEFT);
  //m_brw_posts->tooltip("Selected Scene Objects");
  //m_brw_posts->callback((Fl_Callback*)GUI_SceneEdit::cb_BrowserObjSel);
  m_brw_posts->textfont(FL_COURIER_BOLD);
  m_brw_posts->textsize(12);

}

//----------------------------------------------------------
// Procedure: initWidgetsFolio()
//   Purpose: Widgets that should be initialized only after the
//            CommandFolio has been set.

void UCMD_GUI::initWidgetsFolio() 
{
  //=========================================================
  // Handle the Label(CommandItem) Buttons
  //=========================================================
  // m_cmd_buttons[0]=LoiterButton    m_labels[0]="loiter"
  // m_cmd_buttons[1]=StationButton   m_labels[1]="station"
  // m_cmd_buttons[2]=DefendButton    m_labels[2]="defend"

  // Part 1: Delete any previously created command buttons
  for(unsigned int i=0; i<m_cmd_buttons.size(); i++) {
    this->remove(m_cmd_buttons[i]);
    delete(m_cmd_buttons[i]);
  }
  m_cmd_buttons.clear();
  m_cmd_labels.clear();
  m_cmd_vnames.clear();
  m_max_label_len = 0;
  
  // Part 2: Get the set of Vehicle names (they will be in alph order)
  set<string> vehicles = m_cmd_folio.getAllReceivers();
  vehicles.erase("each");

  long int cbutton_ctr = 0;
  
  list<string> vlist;
  for(set<string>::iterator q=vehicles.begin(); q!=vehicles.end(); q++) {
    string vname = *q;
    if((vname == "all") || (vname == "shore"))
      vlist.push_front(vname);
    else
      vlist.push_back(vname);
  }

  // Part 4: Now handle all the other receivers (excluding "all")
  for(list<string>::iterator p=vlist.begin(); p!=vlist.end(); p++) {
    string vname = *p;

    set<string> labels = m_cmd_folio.getAllLabels(vname);
    for(set<string>::iterator q=labels.begin(); q!=labels.end(); q++) {
      string label = *q;
      Fl_Button *button = new Fl_Button(0, 0, 1, 1, 0); 
      button->copy_label(label.c_str());
      button->clear_visible_focus();
      button->callback((Fl_Callback*)UCMD_GUI::cb_ButtonCmdAction,(void*)cbutton_ctr);

      string bcolor = m_cmd_folio.getCmdColor(label);
      if(bcolor != "") {
	ColorPack cpack(bcolor);
	double red_dbl = cpack.red();
	double grn_dbl = cpack.grn();
	double blu_dbl = cpack.blu();
	int red_int = red_dbl * 255;
	int grn_int = grn_dbl * 255;
	int blu_int = blu_dbl * 255;
	Fl_Color fcolor = fl_rgb_color(red_int, grn_int, blu_int);
	button->color(fcolor);
      } 
	  
      
      this->add(button);
      m_cmd_buttons.push_back(button);
      m_cmd_labels.push_back(label);
      m_cmd_vnames.push_back(vname);
      cbutton_ctr++;

      if(label.length() > m_max_label_len)
	m_max_label_len = label.length();
    }
  }

}

//-------------------------------------------------------------------
// Procedure: resizeWidgetsShape()

void UCMD_GUI::resizeWidgetsShape()
{
  m_menubar->resize(0, 0, w(), 20);
  
  if(m_cmd_vnames.size() == 0)
    return;

  //int extra_wid = w() - m_start_wid;

  int button_start_x = 100;
  int ibut_x = button_start_x;
  int ibut_y = -5;
  int ibut_w = 80;
  int ibut_h = 20;

  int extra = 0;
  if(m_max_label_len > 7) 
    extra = 8 * (m_max_label_len - 7);
  else if(m_max_label_len < 5) 
    extra = - 8 * (5 - m_max_label_len);
  ibut_w += extra;
  
  // ----------------------------------------------------
  // The Label Buttons                       
  // ----------------------------------------------------
  m_row_bottoms.clear();
  m_row_vnames.clear();

  string curr_vname;
  unsigned int rix = 0;
  for(unsigned int i=0; i<m_cmd_buttons.size(); i++) {

    // Handle when switching to new vehicle, or just starting
    if((i==0) || (m_cmd_vnames[i] != curr_vname)) {
      rix = 0;
      if(i!=0) {
	m_row_bottoms.push_back(ibut_y + ibut_h);
	m_row_vnames.push_back(curr_vname);
      }
      curr_vname = m_cmd_vnames[i];
      ibut_y += 35;
    }
    
    ibut_x = button_start_x + (rix * (ibut_w + 15));

    if((ibut_x + ibut_w + 20) > w()) {    // If run out of room
      ibut_x = button_start_x;
      ibut_y += 25;
      rix = 1;
    }
    else 
      rix++;

    m_cmd_buttons[i]->resize(ibut_x, ibut_y, ibut_w, ibut_h);    
  } 
  m_row_bottoms.push_back(ibut_y + ibut_h);
  m_row_vnames.push_back(curr_vname);

  // ----------------------------------------------------
  // The Browser
  // ----------------------------------------------------
  if(m_show_posts) {
    int brw_x = 5;
    int brw_y = ibut_y + ibut_h + 10;
    int brw_w = w() - 10;
    int brw_h = h() - brw_y - 10;
    if(brw_h < 120)
      brw_h = 120;
    m_brw_hgt = brw_h;
    m_brw_posts->resize(brw_x, brw_y, brw_w, brw_h);
  }
}
  
  

//-------------------------------------------------------------------
// Procedure: resizeWidgetsText()

void UCMD_GUI::resizeWidgetsText()
{
  int info_size = 12;
  
  for(unsigned int i=0; i<m_cmd_buttons.size(); i++) 
    m_cmd_buttons[i]->labelsize(info_size);
}



//-------------------------------------------------------------------
// Procedure: getCmdButtonsBottom()

int UCMD_GUI::getCmdButtonsBottom()
{
  int bottom = 0;
  for(unsigned int i=0; i<m_cmd_buttons.size(); i++) {
    int y = m_cmd_buttons[i]->y();
    int h = m_cmd_buttons[i]->h();
    if((y+h) > bottom)
      bottom = y+h;
  }
  return(bottom);
}

//----------------------------------------------------
// Procedure: getPendingCmdPosts()

vector<CommandPost> UCMD_GUI::getPendingCmdPosts() const
{
  return(m_pending_cmd_posts);
}

//----------------------------------------------------
// Procedure: clearPendingCmdPosts()

void UCMD_GUI::clearPendingCmdPosts()
{
  m_pending_cmd_posts.clear();
}
  

//----------------------------------------------------
// Procedure: cb_ButtonCmdAction_i()

inline void UCMD_GUI::cb_ButtonCmdAction_i(int v) 
{
  string label = m_cmd_labels[v];
  string vname = m_cmd_vnames[v];
  char   pchar = 65 + (m_but_post_count % 26);
  
  cout << "Cmd Action ---> " << v << endl;
  cout << "Cmd Label  ---> " << m_cmd_labels[v] << endl;
  cout << "Cmd VName  ---> " << m_cmd_vnames[v] << endl;

  bool test_post = false;
  if(Fl::event_key(FL_Shift_L) || Fl::event_key(FL_Shift_R))
    test_post = true;

  unsigned int saved_cmd_post_count = m_cmd_post_count;
  vector<CommandItem> cmd_items = m_cmd_folio.getAllCmdItems();
  for(unsigned int i=0; i<cmd_items.size(); i++) {
    bool match = true;
    if(cmd_items[i].getCmdLabel() != label) 
      match = false;
    if(!cmd_items[i].hasReceiver(vname))
      match = false;
    if(match) {
      string pid = pchar + uintToString(m_cmd_post_count);

      // If a single vehicle, or vname is "all" and all is not limited
      if((vname != "all") || (!m_cmd_folio.hasLimitedVNames())) {
	string pid = pchar + uintToString(m_cmd_post_count);
	CommandPost cmd_post;
	cmd_post.setCommandItem(cmd_items[i]);
	cmd_post.setCommandTarg(vname);
	cmd_post.setCommandTest(test_post);
	cmd_post.setCommandPID(pid);
	m_pending_cmd_posts.push_back(cmd_post);
	m_cmd_post_count++;
      }
      // VName is "all" and limited_vnames. We don't want to publish
      // ACTION_ALL, since this may send commands to other unintended
      // vehicles. Instead publish ACTION_VNAME_1, ..., ACTION_VNAME_N.
      // Where VNAME_1, ..., VNAME_N are the identified limited VNames.
      else {
	set<string> vnames = m_cmd_folio.getLimitedVNames();
	set<string>::iterator p;
	for(p=vnames.begin(); p!=vnames.end(); p++) {
	  string pid = pchar + uintToString(m_cmd_post_count);
	  string lim_vname = *p;
	  CommandPost cmd_post;
	  cmd_post.setCommandItem(cmd_items[i]);
	  cmd_post.setCommandTarg(lim_vname);
	  cmd_post.setCommandTest(test_post);
	  cmd_post.setCommandPID(pid);
	  m_pending_cmd_posts.push_back(cmd_post);
	  m_cmd_post_count++;
	}
      }
    }
  }
  if(test_post)
    m_cmd_post_count = saved_cmd_post_count;
  
  if(!test_post)
    m_but_post_count++;
}

void UCMD_GUI::cb_ButtonCmdAction(Fl_Widget* o, int v) 
{
  ((UCMD_GUI*)(o->parent()->user_data()))->cb_ButtonCmdAction_i((int)v);
}

//----------------------------------------------------
// Procedure: cb_TogglePostView_i()

inline void UCMD_GUI::cb_TogglePostView_i() 
{
  m_show_posts = !m_show_posts;

  if(m_show_posts)
    m_brw_posts->show();
  else
    m_brw_posts->hide();

  if(m_show_posts) {
    int brw_x = m_brw_posts->x();
    int brw_y = m_brw_posts->y();
    int brw_w = m_brw_posts->w();
    int brw_h = m_brw_hgt;
    m_brw_posts->resize(brw_x, brw_y, brw_w, brw_h);
  }

  roomToView();
  redraw();
}

void UCMD_GUI::cb_TogglePostView(Fl_Widget* o) 
{
  ((UCMD_GUI*)(o->parent()->user_data()))->cb_TogglePostView_i();
}

//---------------------------------------------------
// Procedure: Close

void UCMD_GUI::cb_Close_i()
{
  Fl_Widget::hide();
}

void UCMD_GUI::cb_Close(Fl_Widget* o) {
  ((UCMD_GUI*)(o->parent()->user_data()))->cb_Close_i();
}

//---------------------------------------------------
// Procedure: cb_Quit

void UCMD_GUI::cb_Quit() {
  exit(0);
}

//---------------------------------------------------
// Procedure: cb_ConcedeTopWindow

void UCMD_GUI::cb_ConcedeTopWindow_i()
{
  m_concede_top = true;
}

void UCMD_GUI::cb_ConcedeTopWindow(Fl_Widget* o) {
  ((UCMD_GUI*)(o->parent()->user_data()))->cb_ConcedeTopWindow_i();
}





