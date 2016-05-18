/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_HDG_GUI.h                                       */
/*    DATE: May 10th, 2016                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ZAIC_HDG_GUI_HEADER
#define ZAIC_HDG_GUI_HEADER

#include "ZAIC_GUI.h"

class ZAIC_HDG_GUI : public ZAIC_GUI {
 public:
  ZAIC_HDG_GUI(int w, int h, const char *label=0);
  ~ZAIC_HDG_GUI() {};
  
  void updateOutput(); // overloading virtual function of ZAIC_GUI
  void augmentMenu();

 protected:
  void initWidgets();
  void updateFieldColors();
  
 public:
  Fl_Output   *m_fld_medval;
  Fl_Output   *m_fld_ldelta;
  Fl_Output   *m_fld_hdelta;
  Fl_Output   *m_fld_lowval_util;
  Fl_Output   *m_fld_hghval_util;

  Fl_Output   *m_fld_lowmin_util;
  Fl_Output   *m_fld_hghmin_util;
};
#endif
