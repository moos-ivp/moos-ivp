/*********************************************************/
/*    NAME: Michael Benjamin                             */
/*    FILE: FV_MOOSApp.h                                 */
/*    DATE: May 12th 2006                                */
/*********************************************************/

#ifndef FV_MOOSAPP_HEADER
#define FV_MOOSAPP_HEADER

#include "MOOSLib.h"
#include "FV_Viewer.h"
#include "FV_Model.h"
#include "FV_GUI.h"
#include "Demuxer.h"

class FV_MOOSApp : public CMOOSApp
{
 public:
  FV_MOOSApp();
  virtual ~FV_MOOSApp() {};

  void setViewer(FV_Viewer* g_viewer)  {viewer = g_viewer;};
  void setModel(FV_Model* g_model)     {model = g_model;};
  void setGUI(FV_GUI* g_gui)           {gui = g_gui;};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

protected:
  int     loc_x;
  int     loc_y;

  int     iteration;
  double  start_time;
  int     delta;
  std::string ipf_name;
  
  FV_GUI*     gui;
  FV_Model*   model;
  FV_Viewer*  viewer;

  Demuxer demuxer;
};

#endif
