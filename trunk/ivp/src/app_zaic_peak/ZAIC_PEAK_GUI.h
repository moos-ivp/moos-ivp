/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_GUI.h                                           */
/*    DATE: June 17th, 2006                                      */
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

#ifndef ZAIC_PEAK_GUI_HEADER
#define ZAIC_PEAK_GUI_HEADER

#include "ZAIC_GUI.h"

class ZAIC_PEAK_GUI : public ZAIC_GUI {
public:
  ZAIC_PEAK_GUI(int w, int h, const char *label=0);
  ~ZAIC_PEAK_GUI() {}
  
  void updateOutput();
  void updateFieldColors();
  void augmentMenu();

public:
  Fl_Output   *m_fld_summit1;
  Fl_Output   *m_fld_pwidth1;
  Fl_Output   *m_fld_bwidth1;
  Fl_Output   *m_fld_sdelta1;

  Fl_Output   *m_fld_summit2;
  Fl_Output   *m_fld_pwidth2;
  Fl_Output   *m_fld_bwidth2;
  Fl_Output   *m_fld_sdelta2;

  Fl_Output   *m_fld_maxutil;
  Fl_Output   *m_fld_minutil;

private:
  inline void cb_ToggleWRAP_i();
  static void cb_ToggleWRAP(Fl_Widget*);

  inline void cb_DrawMode_i(int);
  static void cb_DrawMode(Fl_Widget*, int);
};
#endif




