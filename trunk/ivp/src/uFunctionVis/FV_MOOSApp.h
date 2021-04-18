/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: FV_MOOSApp.h                                         */
/*    DATE: May 12th 2006                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef FV_MOOSAPP_HEADER
#define FV_MOOSAPP_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "FV_Viewer.h"
#include "FV_Model.h"
#include "FV_GUI.h"
#include "Demuxer.h"

class FV_MOOSApp : public CMOOSApp
{
 public:
  FV_MOOSApp();
  virtual ~FV_MOOSApp() {}

  void setViewer(FV_Viewer* viewer)  {m_viewer = viewer;}
  void setModel(FV_Model* model)     {m_model = model;}
  void setGUI(FV_GUI* gui)           {m_gui = gui;}

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

  /// Call this when you want the object's CMOOSApp::Run(...) method 
  /// to return soon.

  //   void return_from_Run();

  /// This interacts with FLTK, and therefore should execute on the 
  /// main thread, which is where all the other FLTK operations are 
  /// occurring.
  void process_demuxer_content();

 protected:
  void registerVariables();
  
 protected:
  FV_GUI*     m_gui;
  FV_Model*   m_model;
  FV_Viewer*  m_viewer;

  /// This is populated by OnNewMail, which is invoked by the MOOS 
  /// application thread.  It's content is consumed by the main thread, 
  /// so that all FLTK-related operation can happen on the same thread, 
  /// which is the safest way to use FLTK.  
  Demuxer m_demuxer;

  /// Hold this lock whenever invoking a method on 'demuxer'.
  CMOOSLock m_demuxer_lock;
};

#endif





