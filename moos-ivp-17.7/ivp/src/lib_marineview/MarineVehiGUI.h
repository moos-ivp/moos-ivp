/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MarineVehiGUI.h                                      */
/*    DATE: Nov 11th 2004                                        */
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

#ifndef MARINE_VEHI_GUI_HEADER
#define MARINE_VEHI_GUI_HEADER

#include <string>
#include "MarineGUI.h"

class MarineVehiGUI : public MarineGUI {
public:
  MarineVehiGUI(int w, int h, const char *l=0);
  virtual ~MarineVehiGUI() {}

  bool   setRadioVehiAttrib(std::string attr, std::string val="");
  void   updateRadios();
  void   setMenuItemColors();

 protected: // Protected member functions
  void   augmentMenu();

  virtual bool syncNodesAtoB() {return(false);}

 protected: // Widget callback functions
  inline void cb_SetRadioVehiAttrib_i(int);
  static void cb_SetRadioVehiAttrib(Fl_Widget*, int);

  inline void cb_CycleFocus_i();
  static void cb_CycleFocus(Fl_Widget*);

  inline void cb_ToggleTrailColor_i();
  static void cb_ToggleTrailColor(Fl_Widget*);

  inline void cb_AltTrailLength_i(int);
  static void cb_AltTrailLength(Fl_Widget*, int);

  inline void cb_AltTrailSize_i(int);
  static void cb_AltTrailSize(Fl_Widget*, int);

  inline void cb_AltShapeScale_i(int);
  static void cb_AltShapeScale(Fl_Widget*, int);

  inline void cb_DataView_i(int);
  static void cb_DataView(Fl_Widget*, int);

  inline void cb_CenterView_i(int);
  static void cb_CenterView(Fl_Widget*, int);
};
#endif











