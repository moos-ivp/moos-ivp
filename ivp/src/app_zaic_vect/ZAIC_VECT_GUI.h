/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_VECT_GUI.h                                      */
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

#ifndef ZAIC_VECT_GUI_HEADER
#define ZAIC_VECT_GUI_HEADER

#include "ZAIC_Vector.h"
#include "ZAIC_GUI.h"
#include <FL/Fl_Button.H>

class ZAIC_VECT_GUI : public ZAIC_GUI {
 public:
  ZAIC_VECT_GUI(int w, int h, const char *label=0);
  ~ZAIC_VECT_GUI() {};

  void setZAIC(ZAIC_Vector *zaic);
  void setVerbose(bool);

 protected:
  void updateOutput(); // overloading virtual function of ZAIC_GUI
  void augmentMenu();

 private:
  Fl_Output  *m_min_util;
  Fl_Output  *m_max_util;

  Fl_Output  *m_fld_pieces;
  Fl_Output  *m_fld_tolerance;

  Fl_Button  *m_but_rebuild;

 private:
  inline void cb_ReBuild_i();
  static void cb_ReBuild(Fl_Widget*);

  inline void cb_Tolerance_i(int);
  static void cb_Tolerance(Fl_Widget*, int);
};
#endif




