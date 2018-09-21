/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_HLEQ_GUI.h                                      */
/*    DATE: Apr 6th, 2008                                        */
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

#ifndef ZAIC_HLEQ_GUI_HEADER
#define ZAIC_HLEQ_GUI_HEADER

#include "ZAIC_GUI.h"

class ZAIC_HLEQ_GUI : public ZAIC_GUI {
public:
  ZAIC_HLEQ_GUI(int w, int h, const char *label=0);
  ~ZAIC_HLEQ_GUI() {};
  
  void updateOutput(); // overloading virtual function of ZAIC_GUI
  void augmentMenu();

 protected:
  void initWidgets();
  
 protected:
  Fl_Output   *m_fld_summit;
  Fl_Output   *m_fld_bwidth;
  Fl_Output   *m_fld_minutil;
  Fl_Output   *m_fld_maxutil;
  Fl_Output   *m_fld_sumdelta;

 private:
  inline void cb_ToggleHLEQ_i();
  static void cb_ToggleHLEQ(Fl_Widget*);
};
#endif




