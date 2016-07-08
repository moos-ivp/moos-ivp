/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
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

class UCMD_GUI : Fl_Window {
public:
  UCMD_GUI(int w, int h, const char *l=0);
  ~UCMD_GUI() {}
  
  void augmentMenu();
  void draw();
  void resize(int, int, int, int);
  void setCommandFolio(CommandFolio);
  void setPostSummary(const std::vector<std::string>&);
  
  std::vector<CommandItem> getPendingCmdItems() const;
  std::vector<std::string> getPendingCmdTargs() const;
  void                     clearPendingCmdItems();
  void                     clearPendingCmdTargs();
  
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

  std::vector<CommandItem> m_pending_cmd_items;
  std::vector<std::string> m_pending_cmd_targs;

  std::vector<int>         m_row_bottoms;
  std::vector<std::string> m_row_vnames;

  std::list<std::string>   m_posts;
  
  CommandFolio m_cmd_folio;
  
  double m_start_wid;

  double m_brw_hgt;
  bool   m_show_posts;
};
#endif
