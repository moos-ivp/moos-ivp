/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BNG_GUI.h                                            */
/*    DATE: May 21 2013                                          */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef BNG_GUI_HEADER
#define BNG_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Counter.H>
#include "BNGViewer.h"
#include "IPFViewer.h"
#include "MarineGUI.h"
#include "MY_Output.h"
#include "PairModel.h"

class BNG_GUI : public MarineGUI {
public:
  BNG_GUI(int w, int h, const char *l=0);
  ~BNG_GUI() {};
  
  void updateXY();
  int  handle(int);
  void refreshCounters();

public:
  PairModel *m_pair_model;

  BNGViewer *m_bng_viewer;
  IPFViewer *m_ipf_viewer;

  unsigned int m_ipf_resolution;

  // Column ONE Ownship fields
  MY_Output  *p_os_x;
  MY_Output  *p_os_y;
  MY_Output  *p_os_h;
  MY_Output  *p_os_v;

  // Column TWO Contact fields
  MY_Output  *p_cn_x;
  MY_Output  *p_cn_y;
  MY_Output  *p_cn_h;
  MY_Output  *p_cn_v;
  
  // Column THREE Ownship-Contact fields
  MY_Output  *p_os_cn_bng;
  MY_Output  *p_os_cn_rel_bng;
  MY_Output  *p_os_cn_abs_rel_bng;

  // Column FOUR Contact-Ownship fields
  MY_Output  *p_cn_os_bng;
  MY_Output  *p_cn_os_rel_bng;
  MY_Output  *p_cn_os_abs_rel_bng;

  // Column FIVE Pair Fields
  MY_Output  *p_range;
  MY_Output  *p_cpa;
  MY_Output  *p_tarb;
  MY_Output  *p_spd_in_cn_hdg;

  // Column SIX Relative Position Fields
  MY_Output  *p_fore_of_cn;
  MY_Output  *p_aft_of_cn;
  MY_Output  *p_port_of_cn;
  MY_Output  *p_starboard_of_cn;

  // Column SEVEN Passing Fields
  MY_Output  *p_passes_cn;
  MY_Output  *p_passes_cn_port;
  MY_Output  *p_passes_cn_star;

  // Column EIGHT Crossing Fields
  MY_Output  *p_crosses_bow;
  MY_Output  *p_crosses_stern;
  MY_Output  *p_crosses_either;
  MY_Output  *p_crosses_bow_dist;
  MY_Output  *p_crosses_stern_dist;

  // Column NINE Behavior Fields
  MY_Output  *p_bhv_mode;
  MY_Output  *p_bhv_debug1;
  MY_Output  *p_bhv_debug2;
  MY_Output  *p_bhv_debug3;
  MY_Output  *p_bhv_debug4;

 protected:
  void augmentMenu();
 
private:
  inline void cb_CenterView_i();
  static void cb_CenterView(Fl_Widget*);

  inline void cb_ToggleRangeCircle_i(int);
  static void cb_ToggleRangeCircle(Fl_Widget*, int);

  inline void cb_LinearMoveShips_i(int);
  static void cb_LinearMoveShips(Fl_Widget*, int);

  inline void cb_AltOwnshipRange_i(int);
  static void cb_AltOwnshipRange(Fl_Widget*, int);

  inline void cb_AltOwnshipBearing_i(int);
  static void cb_AltOwnshipBearing(Fl_Widget*, int);

  inline void cb_SetPair_i();
  static void cb_SetPair(Fl_Widget*);

  inline void cb_ResetPair_i();
  static void cb_ResetPair(Fl_Widget*);

  inline void cb_AltShapeScale_i(int);
  static void cb_AltShapeScale(Fl_Widget*, int);

  inline void cb_ResetOS_i();
  static void cb_ResetOS(Fl_Widget*);

  inline void cb_ResetCN_i();
  static void cb_ResetCN(Fl_Widget*);

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

  // ------------------------------------------
  inline void cb_AltCNRNG_i();
  static void cb_AltCNRNG(Fl_Widget*);

  inline void cb_AltCNBNG_i();
  static void cb_AltCNBNG(Fl_Widget*);

  inline void cb_RotateX_i(int);
  static void cb_RotateX(Fl_Widget*, int);

  inline void cb_RotateZ_i(int);
  static void cb_RotateZ(Fl_Widget*, int);

  inline void cb_Reset_i(int);
  static void cb_Reset(Fl_Widget*, int);

  inline void cb_ToggleFrame_i();
  static void cb_ToggleFrame(Fl_Widget*);

  inline void cb_StretchRad_i(int);
  static void cb_StretchRad(Fl_Widget*, int);

  inline void cb_ModBase_i(int);
  static void cb_ModBase(Fl_Widget*, int);

  inline void cb_ModScale_i(int);
  static void cb_ModScale(Fl_Widget*, int);

  inline void cb_AdjustPrecisionIPF_i(int);
  static void cb_AdjustPrecisionIPF(Fl_Widget*, int);

};
#endif


