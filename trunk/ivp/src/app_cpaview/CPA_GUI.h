/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CPA_GUI.h                                            */
/*    DATE: Feb 12 2007                                          */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef CPA_GUI_HEADER
#define CPA_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Counter.H>
#include "CPAViewer.h"
#include "IPFViewer.h"
#include "MarineGUI.h"
#include "MY_Output.h"

class CPA_GUI : public MarineGUI {
public:
  CPA_GUI(int w, int h, const char *l=0);
  ~CPA_GUI() {};
  
  void updateXY();
  int  handle(int);
  void refreshCounters();

public:
  CPA_Model *cpa_model;
  CPAViewer *cpa_viewer;
  IPFViewer *ipf_viewer;
  int        contact_index;
  int        ipf_resolution;

  Fl_Counter *cn_hdg_ctr;
  Fl_Counter *cn_spd_ctr;
  Fl_Counter *cn_rng_ctr;
  Fl_Counter *cn_bng_ctr;
  Fl_Counter *cn_xpos_ctr;
  Fl_Counter *cn_ypos_ctr;

  Fl_Counter *os_maxtol_ctr;
  Fl_Counter *os_maxspd_ctr;


  MY_Output  *p_os_maxspd;
  MY_Output  *p_os_maxtol;

 protected:
  void augmentMenu();
 
private:
  inline void cb_ResetOS_i();
  static void cb_ResetOS(Fl_Widget*);

  inline void cb_ResetCN_i();
  static void cb_ResetCN(Fl_Widget*);

  inline void cb_AltOS_MAXSPD_i();
  static void cb_AltOS_MAXSPD(Fl_Widget*);

  inline void cb_AltOS_MAXTOL_i();
  static void cb_AltOS_MAXTOL(Fl_Widget*);

  inline void cb_AltCNX_i(int);
  static void cb_AltCNX(Fl_Widget*, int);

  inline void cb_AltCNY_i(int);
  static void cb_AltCNY(Fl_Widget*, int);

  inline void cb_AltCNSPD_i(int);
  static void cb_AltCNSPD(Fl_Widget*, int);

  inline void cb_AltCNCRS_i(int);
  static void cb_AltCNCRS(Fl_Widget*, int);

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

  inline void cb_SetTypeIPF_i(int);
  static void cb_SetTypeIPF(Fl_Widget*, int);

  inline void cb_AdjustPrecisionIPF_i(int);
  static void cb_AdjustPrecisionIPF(Fl_Widget*, int);

  inline void cb_CN_INDEX_i(int);
  static void cb_CN_INDEX(Fl_Widget*, int);
};
#endif


