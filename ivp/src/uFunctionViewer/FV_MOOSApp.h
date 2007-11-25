/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: FV_MOOSApp.h                                         */
/*    DATE: May 12th 2006                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef FV_MOOSAPP_HEADER
#define FV_MOOSAPP_HEADER

#include "MOOSLib.h"
#include "FV_GUI.h"
#include "Demuxer.h"

class FV_MOOSApp : public CMOOSApp
{
 public:
  FV_MOOSApp();
  virtual ~FV_MOOSApp();

  void setGUI(FV_GUI* g_gui) {gui = g_gui;};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

protected:
  FV_GUI *gui;
  int loc_x;
  int loc_y;

  int iteration;
  double start_time;
  int  delta;
  std::string ipf_name;

  Demuxer demuxer;
};

#endif





