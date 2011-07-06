/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: FV_MOOSApp.cpp                                       */
/*    DATE: May 12th 2006                                        */
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
#include <iterator>
#include "FV_MOOSApp.h"
#include "FunctionEncoder.h"
#include "MBUtils.h"
#include "OF_Reflector.h"
#include "AOF_Rings.h"
#include "BuildUtils.h"
#include "ColorParse.h"

using namespace std; 

//----------------------------------------------------------
// Procedure: Constructor

FV_MOOSApp::FV_MOOSApp()
{
  m_model    = 0;
  m_viewer   = 0;
  m_gui      = 0;
  m_ipf_name = "BHV_IPF";
}

//----------------------------------------------------------
// Procedure: OnNewMail()

bool FV_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  try {
     m_demuxer_lock.Lock();

     // The main thread will may be blocking on a call to 'Fl::wait()'.
     // This will wake up the main thread if that's the case.
     Fl::awake();

     MOOSMSG_LIST::iterator p;
     for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;
	string key  = msg.GetKey();
	double dval = msg.GetDouble();
	double timestamp = msg.GetTime();

	if(key == m_ipf_name) {
	  string ipf_str = msg.GetString();
	  string community = msg.GetCommunity();
	  m_demuxer.addMuxPacket(ipf_str, timestamp, community);
        }
	else if(key == "NAV_DEPTH")
	  m_model->setDepth(dval);
	else if(key == "NAV_ALTITUDE")
	  m_model->setAltitude(dval);

    }
  }
  catch (...) {
     m_demuxer_lock.UnLock();
     throw;
  }

  m_demuxer_lock.UnLock();
  return(true);
}

//----------------------------------------------------------
// Procedure: OnConnectToServer()
//      Note: Register for variables here. 
//            Possibly read info from the mission file.
//            m_MissionReader.GetConfigurationParam("Name", <string>);
//            m_Comms.Register("VARNAME", is_float(int));

bool FV_MOOSApp::OnConnectToServer()
{
  if(!m_MissionReader.GetValue("ipf_name", m_ipf_name)) {
    MOOSTrace("Function to be monitored not provided\n");
    //return(false);
  }

  if(m_ipf_name != "")
    m_Comms.Register(m_ipf_name, 0);
  
  m_Comms.Register("NAV_DEPTH", 0);
  m_Comms.Register("NAV_ALTITUDE", 0);
  return(true);
}


//----------------------------------------------------------
// Procedure: Iterate()

bool FV_MOOSApp::Iterate()
{
  return(true);
  // This processing has all been moved to the process_demuxer_content()
  // method, so that it can happen in the same thread as other FLTK 
  // operations.
}

//----------------------------------------------------------
// Procedure: process_demuxer_content()


void FV_MOOSApp::process_demuxer_content()
{
  try {
    m_demuxer_lock.Lock();

    bool redraw_needed = false;
    DemuxedResult result = m_demuxer.getDemuxedResult();
    while(!result.isEmpty()) {
      redraw_needed = true;
      string ipf_str = result.getString();
      string community_src = result.getSource();

      //cout << "FV_MOOSApp:process_demuxer_content():" << endl;
      //cout << "str:" << ipf_str << endl;
      //cout << "src:" << community_src << endl << endl;
      
      string context_str = StringToIvPContext(ipf_str);
      string iter = biteString(context_str, ':');
      string bhv  = context_str;

      m_model->addIPF(ipf_str, community_src);
      result = m_demuxer.getDemuxedResult();

      if(m_gui)
	m_gui->addBehaviorSource(bhv);
    }
      
    if(redraw_needed) {
      m_viewer->resetQuadSet();
      m_viewer->redraw();
    }
    if(m_gui)
      m_gui->updateFields();
  }
  catch (...) {
    m_demuxer_lock.UnLock();
    throw;
  }

  m_demuxer_lock.UnLock();
}

//----------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool FV_MOOSApp::OnStartUp()
{
  return(true);
}

