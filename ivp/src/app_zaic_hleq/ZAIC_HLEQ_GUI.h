/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_HLEQ_GUI.h                                      */
/*    DATE: Apr 6th, 2008                                        */
/*    DATE: May 10th, 2016 Refactored to use common superclass   */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
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
