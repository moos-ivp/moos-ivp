/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PAIR_GUI.h                                           */
/*    DATE: Jan 9th, 2025                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef PAIR_GUI_HEADER
#define PAIR_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>
#include "MarineGUI.h"
#include "PairModel.h"
#include "PAIR_Viewer.h"

class PAIR_GUI : public MarineGUI {
public:
  PAIR_GUI(int w, int h, const char *l=0);
  ~PAIR_GUI() {};
  
  void  resize(int, int, int, int);
  void  updateXY();
  int   handle(int);
  void  setPairModel(PairModel);

 protected:
  void  augmentMenu();
  void  initWidgets();
  void  resizeWidgetsShape();
  void  resizeWidgetsText();

 public:
  // The active pair_model share across several objects
  PairModel *m_pair_model;
  // The pair_model acting as the starting point, can be reset to.
  PairModel  m_pair_model_set;

  PAIR_Viewer *m_pair_viewer;

 protected:
  double    m_curr_time;

  //=================================================
  // Field Panel
  //=================================================
  // Field Panel - Column ONE Ownship fields
  Fl_Output  *m_fld_os_x;
  Fl_Output  *m_fld_os_y;

  // Field Panel - Column TWO Ownship fields
  Fl_Output  *m_fld_os_h;
  Fl_Output  *m_fld_os_v;

  // Field Panel - Column THREE Contact fields
  Fl_Output  *m_fld_cn_x;
  Fl_Output  *m_fld_cn_y;

  // Field Panel - Column FOUR Contact fields
  Fl_Output  *m_fld_cn_h;
  Fl_Output  *m_fld_cn_v;

  // Field Panel - Column FIVE Range fields
  Fl_Output  *m_fld_range;
  Fl_Output  *m_fld_range_rate;

  // Field Panel - Column SIX CPA/ROC fields
  Fl_Output  *m_fld_cpa;
  Fl_Output  *m_fld_roc;

  // Field Panel - Column SEVEN alert fields
  Fl_Output  *m_fld_alert_rng;
  Fl_Output  *m_fld_warn_time;

  // Field Panel - Column EIGHT stat fields
  Fl_Output  *m_fld_alerted;
  Fl_Output  *m_fld_warned;

  // Field Panel - Column NINE time-of fields
  Fl_Output  *m_fld_to_alerted;
  Fl_Output  *m_fld_to_warned;
  
  // Field Panel - Column TEN time fields
  Fl_Output  *m_fld_curr_time;
  Fl_Output  *m_fld_delt_time;
  
private:
  inline void cb_CenterView_i();
  static void cb_CenterView(Fl_Widget*);

  inline void cb_ToggleCircle_i(int);
  static void cb_ToggleCircle(Fl_Widget*, int);

  inline void cb_LinearMoveShips_i(int);
  static void cb_LinearMoveShips(Fl_Widget*, int);

  inline void cb_LinearMoveOwnship_i(int);
  static void cb_LinearMoveOwnship(Fl_Widget*, int);

  inline void cb_LinearMoveContact_i(int);
  static void cb_LinearMoveContact(Fl_Widget*, int);

  inline void cb_AltOwnshipRange_i(int);
  static void cb_AltOwnshipRange(Fl_Widget*, int);

  inline void cb_AltBearing_i(int);
  static void cb_AltBearing(Fl_Widget*, int);

  inline void cb_SetPair_i();
  static void cb_SetPair(Fl_Widget*);

  inline void cb_SwapPair_i();
  static void cb_SwapPair(Fl_Widget*);

  inline void cb_ResetPair_i(int);
  static void cb_ResetPair(Fl_Widget*, int);

  inline void cb_AltShapeScale_i(int);
  static void cb_AltShapeScale(Fl_Widget*, int);

  inline void cb_DumpCmdLineArgs_i();
  static void cb_DumpCmdLineArgs(Fl_Widget*);

  // ------------------------------------------
  inline void cb_AltOSX_i(int);
  static void cb_AltOSX(Fl_Widget*, int);

  inline void cb_AltOSY_i(int);
  static void cb_AltOSY(Fl_Widget*, int);

  inline void cb_AltOSH_i(int);
  static void cb_AltOSH(Fl_Widget*, int);

  inline void cb_AltOSV_i(int);
  static void cb_AltOSV(Fl_Widget*, int);

  // ------------------------------------------
  inline void cb_AltCNX_i(int);
  static void cb_AltCNX(Fl_Widget*, int);

  inline void cb_AltCNY_i(int);
  static void cb_AltCNY(Fl_Widget*, int);

  inline void cb_AltCNV_i(int);
  static void cb_AltCNV(Fl_Widget*, int);

  inline void cb_AltCNH_i(int);
  static void cb_AltCNH(Fl_Widget*, int);

  int m_start_hgt;
  int m_start_wid;
};
#endif
