/*********************************************************/
/*    NAME: Michael Benjamin                             */
/*    FILE: FV_MOOSApp.h                                 */
/*    DATE: May 12th 2006                                */
/*********************************************************/

#ifndef FV_MOOSAPP_HEADER
#define FV_MOOSAPP_HEADER

#include "MOOSLib.h"
#include "MOOSGenLib.h"
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

  /// Call this when you want the object's CMOOSApp::Run(...) method to return
  /// soon.
//   void return_from_Run();

  /// This interacts with FLTK, and therefore should execute on the main thread,
  /// which is where all the other FLTK operations are occurring.
  void process_demuxer_content();
  
protected:
//   bool    iterate_should_return_false;
  
  int     loc_x;
  int     loc_y;

  int     iteration;
  double  start_time;
  int     delta;
  std::string ipf_name;
  
  FV_GUI*     gui;
  FV_Model*   model;
  FV_Viewer*  viewer;

  /// This is populated by OnNewMail, which is invoked by the MOOS application 
  /// thread.  It's content is consumed by the main thread, so that all FLTK-
  /// related operation can happen on the same thread, which is the safest way
  /// to use FLTK.  
  Demuxer demuxer;

  /// Hold this lock whenever invoking a method on 'demuxer'.
  CMOOSLock demuxer_lock;
};

#endif
