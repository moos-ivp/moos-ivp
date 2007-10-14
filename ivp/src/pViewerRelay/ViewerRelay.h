/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: ViewerRelay.h                                   */
/*    DATE: Sep 16th 2007                                   */
/************************************************************/

#ifndef VIEWER_RELAY_HEADER
#define VIEWER_RELAY_HEADER

#include "MOOSLib.h"

class ViewerRelay : public CMOOSApp
{
 public:
  ViewerRelay() {};
  virtual ~ViewerRelay() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void RegisterVariables();

 protected:
  bool handleNewViewerLeftClick(std::string);
  bool handleNewViewerRightClick(std::string);
  bool handleNewViewerRightClickRP(std::string);

 protected:
  // insert local vars here
};

#endif 
