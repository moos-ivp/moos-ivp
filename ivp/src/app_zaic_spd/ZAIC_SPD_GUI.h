/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_SPD_GUI.h                                       */
/*    DATE: June 2nd, 2015                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ZAIC_SPD_GUI_HEADER
#define ZAIC_SPD_GUI_HEADER

#include "ZAIC_GUI.h"

class ZAIC_SPD_GUI : public ZAIC_GUI {
 public:
  ZAIC_SPD_GUI(int w, int h, const char *label=0);
  ~ZAIC_SPD_GUI() {};
  
  void updateOutput(); // overloading virtual function of ZAIC_GUI
  void augmentMenu();

 protected:
  void initWidgets();
  void updateFieldColors();
  
 public:
  Fl_Output   *m_fld_medval;
  Fl_Output   *m_fld_lowval;
  Fl_Output   *m_fld_hghval;
  Fl_Output   *m_fld_lowval_util;
  Fl_Output   *m_fld_hghval_util;

  Fl_Output   *m_fld_lowmin_util;
  Fl_Output   *m_fld_hghmin_util;
};
#endif
