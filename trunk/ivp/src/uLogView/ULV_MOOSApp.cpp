/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ULV_MOOSApp.cpp                                      */
/*    DATE: September 25th, 2011                                 */
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

#include <iostream>
#include "ULV_MOOSApp.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

ULV_MOOSApp::ULV_MOOSApp() 
{
  m_pending_moos_events = 0;
  m_verbose         = false;
  m_gui             = 0; 
  m_start_time      = 0;
  m_lastredraw_time = 0;

#if 0
  VarDataPair pair("HELM_MAP_CLEAR", 0);
  m_connection_pairs.push_back(pair);
  m_pending_pairs   = true;
#endif
#if 1
  m_pending_pairs = false;
#endif
}

//----------------------------------------------------------------
// Procedure: OnNewMail

bool ULV_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  if((!m_gui) || (!m_pending_moos_events))
    return(true);

  NewMail.sort();
  
  MOOS_event e;
  e.type="OnNewMail";
  e.moos_time = MOOSTime();

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &Msg = *p;
    e.mail.push_back(MOOS_event::Mail_message(Msg));
  }
  
  m_pending_moos_events->enqueue(e);
  Fl::awake();
  
  return(true);
}


//--------------------------------------------------------------
// Procedure: OnConnectToServer()
//      Note: virtual overide of base class CMOOSApp member.

bool ULV_MOOSApp::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//-------------------------------------------------------------
// Procedure: Iterate
//      Note: virtual overide of base class CMOOSApp member.

bool ULV_MOOSApp::Iterate()
{
  if(m_pending_pairs)
    postConnectionPairs();

  if((!m_gui) || (!m_pending_moos_events))
    return(true);
  
  MOOS_event e;
  e.type="Iterate";
  e.moos_time = MOOSTime();

  m_pending_moos_events->enqueue(e);
  Fl::awake();
  
  return(true);
}

//-------------------------------------------------------------
// Procedure: postConnectionPairs
//      Note: 

void ULV_MOOSApp::postConnectionPairs()
{
  m_pending_pairs = false;
  unsigned int i, vsize = m_connection_pairs.size();
  for(i=0; i<vsize; i++) {
    VarDataPair pair = m_connection_pairs[i];
    string var = pair.get_var();
    if(pair.is_string())
      m_Comms.Notify(var, pair.get_sdata());
    else
      m_Comms.Notify(var, pair.get_ddata());
  }
}

//--------------------------------------------------------
// Procedure: onStartUp()

bool ULV_MOOSApp::OnStartUp()
{
  MOOSTrace("pXMarineViewer starting....\n");
  SetAppFreq(4);

#if 0 // mikerb
  // look for datum latitude, longitude global variables
  double lat, lon;
  bool ok1 = m_MissionReader.GetValue("LatOrigin", lat);
  bool ok2 = m_MissionReader.GetValue("LongOrigin", lon);
  if(!ok1 || !ok2)
    return(MOOSFail("Lat or Lon Origin not set in *.moos file.\n"));

  // If both lat and lon origin ok - then initialize the Geodesy.
  if(m_gui && !m_gui->mviewer->initGeodesy(lat, lon))
    return(MOOSFail("Geodesy Init in pMarineViewer failed - FAIL\n"));

  if(m_gui) {
    cout << "Setting PMV LatOrigin based on the MOOS file. " << endl;
    string datum = doubleToString(lat) + "," + doubleToString(lon);
    m_gui->mviewer->setParam("datum", datum);
  }
#endif

  if((!m_gui) || (!m_pending_moos_events))
    return(true);
  
  MOOS_event e;
  e.type="OnStartUp";
  e.moos_time = MOOSTime();

  m_pending_moos_events->enqueue(e);
  Fl::awake();

  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void ULV_MOOSApp::registerVariables()
{
  m_Comms.Register("ALOGVIEW_TIME", 0);
}

//----------------------------------------------------------------------
// Procedure: handlePendingGUI

void ULV_MOOSApp::handlePendingGUI()
{
  if(!m_gui)
    return;
  
  PostingBuffer pbuffer = m_gui->getPostingBuffer();

  unsigned int i, bsize = pbuffer.size();

  for(i=0; i<bsize; i++) {
    VarDataPair pair = pbuffer.getPending(i);
    string var  = pair.get_var();

    if(pair.is_string()) {
      string sval = pair.get_sdata();
      if(m_verbose)
	cout << "Handling Post: [" << var << "] [" << sval << "]" << endl;
      m_Comms.Notify(var, sval);
    }
    else {
      double dval = pair.get_ddata();
      if(m_verbose)
	cout << "Handling Post: [" << var << "] [" << dval << "]" << endl;
      m_Comms.Notify(var, dval);
    }
  }
}

//----------------------------------------------------------------------
// Procedure: handleNewMail

void ULV_MOOSApp::handleNewMail(const MOOS_event & e)
{
  unsigned int handled_msgs = 0;
  for (size_t i = 0; i < e.mail.size(); ++i) {
    CMOOSMsg msg = e.mail[i].msg;
    string key   = msg.GetKey();
    string sval  = msg.GetString();
    string community = msg.GetCommunity();
    string source = msg.GetSource();

    if(source != "uLogView") {

      bool handled = false;
      if(key == "ALOGVIEW_TIME") {
	double alogview_time = msg.GetDouble();
	m_gui->np_viewer->setCurrTime(alogview_time, false);
	m_gui->lp_viewer->setCurrTime(alogview_time);
	cout << "T" << flush;
	handled = true;
      }
      if(handled)
	handled_msgs++;
    }
  }

  if(handled_msgs > 0) {
    m_lastredraw_time = e.moos_time;
    m_gui->updateXY();
    m_gui->np_viewer->redraw();
    m_gui->lp_viewer->redraw();
  }
}


//----------------------------------------------------------------------
// Procedure: handleIterate

void ULV_MOOSApp::handleIterate(const MOOS_event & e) 
{
  if(!m_gui)
    return;
  m_gui->conditional_step();
  handlePendingGUI(); // mikerb - handle ctime updates to other MApps

#if 0 // mikerb
  double curr_time = e.moos_time - m_start_time;
  double time_diff = (e.moos_time - m_lastredraw_time);
  if(time_diff > 0.2) {
    m_gui->mviewer->redraw();
    m_lastredraw_time = e.moos_time;
  }
  m_gui->updateXY();
  m_gui->mviewer->setParam("curr_time", e.moos_time);
  m_gui->setCurrTime(curr_time);

  string vname = m_gui->mviewer->getStringInfo("active_vehicle_name");

  vector<VarDataPair> left_pairs = m_gui->mviewer->getLeftMousePairs();
  unsigned int i, vsize = left_pairs.size();
  for(i=0; i<vsize; i++) {
    VarDataPair pair = left_pairs[i];
    string var = pair.get_var();
    if(!pair.is_string())
      m_Comms.Notify(var, pair.get_ddata());
    else
      m_Comms.Notify(var, pair.get_sdata());
  }

  vector<VarDataPair> non_mouse_pairs = m_gui->mviewer->getNonMousePairs();
  vsize = non_mouse_pairs.size();
  for(i=0; i<vsize; i++) {
    VarDataPair pair = non_mouse_pairs[i];
    string var = pair.get_var();
    if(!pair.is_string())
      m_Comms.Notify(var, pair.get_ddata());
    else
      m_Comms.Notify(var, pair.get_sdata());
  }

#endif  
}


//----------------------------------------------------------------------
// Procedure: handleStartUp

void ULV_MOOSApp::handleStartUp(const MOOS_event & e) {
  // Keep track of whether the back images were user configured.
  // If not, we'll use the default image afterwards.
  bool tiff_a_set = false;
  bool tiff_b_set = false;

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  STRING_LIST::reverse_iterator p;
  for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
    string line  = *p;
    string param = stripBlankEnds(tolower(biteString(line, '=')));
    string value = stripBlankEnds(line);
    
    if(param == "tiff_file") {
      if(!tiff_a_set)
	tiff_a_set = m_gui->np_viewer->setParam(param, value);
    }
    else if(param == "tiff_file_b") {
      if(!tiff_b_set) 
	tiff_b_set = m_gui->np_viewer->setParam(param, value);
    }
    else if(param == "connection_posting") {
      string var = stripBlankEnds(biteString(value, '='));
      string val = stripBlankEnds(value);
      if(!strContainsWhite(var)) {
	m_pending_pairs = true;
	VarDataPair pair(var, val, "auto");
	m_connection_pairs.push_back(pair);
      }      
    }
    else {
      bool handled = m_gui->np_viewer->setParam(param, value);
      if(!handled)
        handled = m_gui->np_viewer->setParam(param, atof(value.c_str()));
    }
  }
  
  // If no images were specified, use the default images.
  if(!tiff_a_set && !tiff_b_set) {
    m_gui->np_viewer->setParam("tiff_file", "Default.tif");
    m_gui->np_viewer->setParam("tiff_file_b", "DefaultB.tif");
  }

  m_start_time = MOOSTime();
  m_gui->np_viewer->redraw();
  
  registerVariables();
}


