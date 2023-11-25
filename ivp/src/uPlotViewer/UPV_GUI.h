/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: UPV_GUI.h                                            */
/*    DATE: May 18th, 2012                                       */
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

#ifndef UPV_PLOT_GUI_HEADER
#define UPV_PLOT_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "UPV_Viewer.h"
#include "MY_Output.h"

class UPV_GUI : Fl_Window {
public:
  UPV_GUI(int w, int h, const char *l=0);
  ~UPV_GUI() {}
  
  static Fl_Menu_Item menu_[];

  void updateOutput();
  int  handle(int);

  void addScopeVar(std::string);

  UPV_Viewer  *viewer() {return(m_plot_viewer);}

 protected:
  UPV_Viewer  *m_plot_viewer;
  Fl_Menu_Bar *m_menubar;
  MY_Output   *m_varname;
  MY_Output   *m_varavg;

  std::vector<std::string> m_scope_vars;

 private:
  static void cb_Quit();

  inline void cb_ChangeScopeVar_i(int);
  static void cb_ChangeScopeVar(Fl_Widget*, int);

  inline void cb_ToggleScopeVar_i(int);
  static void cb_ToggleScopeVar(Fl_Widget*, int);

  inline void cb_GridSize_i(int);
  static void cb_GridSize(Fl_Widget*, int);

  inline void cb_GridShade_i(int);
  static void cb_GridShade(Fl_Widget*, int);

  inline void cb_LineShade_i(int);
  static void cb_LineShade(Fl_Widget*, int);

  inline void cb_BackShade_i(int);
  static void cb_BackShade(Fl_Widget*, int);

};
#endif










