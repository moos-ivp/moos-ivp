/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SSV_GUI.h                                            */
/*    DATE: September 11th, 2007                                 */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef SSV_GUI_HEADER
#define SSV_GUI_HEADER

#include "MOOSLock.h"
#include "SSV_Viewer.h"
#include "MarineVehiGUI.h"
#include "MY_Output.h"
#include "MY_Button.h"
#include "FL/Fl_Box.H"

class SSV_GUI : public MarineVehiGUI {
public:
  SSV_GUI(int w, int h, const char *l=0);
  virtual ~SSV_GUI() {};

  void updateXY();
  int  handle(int);
  void setCurrTime(double v) {m_curr_time = v;};
  void augmentMenu();
  void addContactButton(int ix, std::string vname);

  SSV_Viewer *mviewer;

  std::string  getPendingVar(unsigned int index);
  std::string  getPendingVal(unsigned int index);
  void         clearPending();
  void         pushPending(std::string, std::string);
  unsigned int getPendingSize() {return(m_pending_vars.size());};

private:
  void  updateButtonColor(MY_Button*);

  inline void cb_CentricToggle_i(int);
  static void cb_CentricToggle(Fl_Widget*, int);

  inline void cb_Radial_i(int);
  static void cb_Radial(Fl_Widget*, int);

  inline void cb_Bearings_i();
  static void cb_Bearings(Fl_Widget*);

  inline void cb_CycleFocus_i(int);
  static void cb_CycleFocus(Fl_Widget*, int);

  inline void cb_ButtonView_i(int);
  static void cb_ButtonView(Fl_Widget*, int);

  inline void cb_MOOS_Button_i(int);
  static void cb_MOOS_Button(Fl_Widget*, int);

  inline void cb_SSV_SetGeoAttr_i(int);
  static void cb_SSV_SetGeoAttr(Fl_Widget*, int);

protected:
  MY_Output  *v_nam;
  MY_Output  *time;
  MY_Output  *x_mtr;
  MY_Output  *y_mtr;
  MY_Output  *v_spd;
  MY_Output  *v_crs;
  MY_Output  *v_dep;
  MY_Output  *v_ais;
  MY_Output  *v_lat;
  MY_Output  *v_lon;
  MY_Output  *d_hash;
  MY_Output  *d_radial;
  MY_Output  *m_rbearing;
  MY_Output  *m_contact_range;
  MY_Output  *m_warp;

  Fl_Box     *m_deploy_box_text;
  Fl_Box     *m_deploy_box_body;
  Fl_Box     *m_station_box_body;
  Fl_Box     *m_engage_box_body;

  Fl_Box     *m_cdeploy_box_text;
  Fl_Box     *m_cdeploy_box_body;
  Fl_Box     *m_cstation_box_body;
  Fl_Box     *m_cengage_box_body;

  MY_Button  *m_deploy_all_on;
  MY_Button  *m_deploy_all_off;
  MY_Button  *m_station_all_on;
  MY_Button  *m_station_all_off;
  MY_Button  *m_engage_all_ok;
  MY_Button  *m_engage_all_off;

  MY_Button  *m_deploy_cur_on;
  MY_Button  *m_deploy_cur_off;
  MY_Button  *m_station_cur_on;
  MY_Button  *m_station_cur_off;
  MY_Button  *m_engage_cur_ok;
  MY_Button  *m_engage_cur_off;

  MY_Button  *ownship_b0;
  MY_Button  *contact_b1;
  MY_Button  *contact_b2;
  MY_Button  *contact_b3;
  MY_Button  *contact_b4;
  MY_Button  *contact_b5;
  MY_Button  *contact_b6;

  std::vector<std::string> m_pending_vars;
  std::vector<std::string> m_pending_vals;

  CMOOSLock  m_ssv_mutex;

  double m_curr_time;
};
#endif

