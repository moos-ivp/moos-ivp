/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: UCMD_GUI.h                                           */
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

#ifndef UCMD_PLOT_GUI_HEADER
#define UCMD_PLOT_GUI_HEADER

#include <list>
#include <string>
#include <vector>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Hold_Browser.H>
#include "CommandFolio.h"
#include "CommandPost.h"

class UCMD_GUI : Fl_Window {
public:
  UCMD_GUI(int w, int h, const char *label, bool show_posts=true);
  ~UCMD_GUI() {};
  
  void augmentMenu();
  void draw();
  void resize(int, int, int, int);
  void setCommandFolio(CommandFolio);
  void setPostSummary(const std::vector<std::string>&);
  void setButPostCount(unsigned int v) {m_but_post_count=v;}
  void setCmdPostCount(unsigned int v) {m_cmd_post_count=v;}
  
  std::vector<CommandPost> getPendingCmdPosts() const;
  void                     clearPendingCmdPosts();

  unsigned int isVisible()   {return(visible());}
  void         makeTop()     {show();}
  int          getWid()      {return(w());}
  int          getHgt()      {return(h());}

  bool         getConcedeTop();

  unsigned int getButPostCount() const {return(m_but_post_count);}
  unsigned int getCmdPostCount() const {return(m_cmd_post_count);}
  unsigned int getShowPosts() const {return(m_show_posts);}
  
 protected:
  void initWidgets();
  void initWidgetsFolio();
  void resizeWidgetsShape();
  void resizeWidgetsText();
  void roomToView();
  int  getCmdButtonsBottom();
  
 private:
  inline void cb_ButtonCmdAction_i(int);
  static void cb_ButtonCmdAction(Fl_Widget*, int);

  inline void cb_TogglePostView_i();
  static void cb_TogglePostView(Fl_Widget*);
  
  inline void cb_Close_i();
  static void cb_Close(Fl_Widget*);

  inline void cb_ConcedeTopWindow_i();
  static void cb_ConcedeTopWindow(Fl_Widget*);

  static void cb_Quit();

 protected:
  Fl_Menu_Bar     *m_menubar;
  Fl_Hold_Browser *m_brw_posts;

  std::string              m_select_vname; // or it could be "all"
  std::string              m_display_mode;
  
  // Index one per CommandItem label matched to buttons
  std::vector<Fl_Button*>  m_cmd_buttons;
  std::vector<std::string> m_cmd_labels;
  std::vector<std::string> m_cmd_vnames;
  std::vector<std::string> m_cmd_bcolor;

  std::vector<CommandPost> m_pending_cmd_posts;

  std::vector<int>         m_row_bottoms;
  std::vector<std::string> m_row_vnames;

  std::list<std::string>   m_posts;
  
  CommandFolio m_cmd_folio;

  unsigned int m_but_post_count;
  unsigned int m_cmd_post_count;
  
  double m_start_wid;

  double m_brw_hgt;
  bool   m_show_posts;

  unsigned int m_max_label_len;
  
  bool   m_concede_top;
};
#endif





