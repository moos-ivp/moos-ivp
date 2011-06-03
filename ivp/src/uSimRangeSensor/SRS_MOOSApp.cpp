/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SRS_MOOSApp.cpp                                      */
/*    DATE: Feb 1st 2011                                         */
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

#include <iterator>
#include "SRS_MOOSApp.h"
#include "ColorParse.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Procedure: OnNewMail

bool SRS_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  m_model.setCurrTime(MOOSTime());
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
	
    bool ok;
    ok = m_model.handleMsg(msg.GetKey(),msg.GetDouble(), msg.GetString(),
			   msg.GetTime(), msg.IsDouble(), msg.IsString(),
			   msg.GetSource());
    if(!ok) {
      cout << termColor("magenta") << endl;
      cout << "Unhandled Message: " << msg.GetKey() << endl;
      cout << termColor() << flush;
    }
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool SRS_MOOSApp::OnConnectToServer()
{
  RegisterVariables();  
  return(true);
}


//------------------------------------------------------------
// Procedure: RegisterVariables

void SRS_MOOSApp::RegisterVariables()
{
  m_Comms.Register("NODE_REPORT", 0);
  m_Comms.Register("NODE_REPORT_LOCAL", 0);
  m_Comms.Register("RANGE_REQUEST", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool SRS_MOOSApp::Iterate()
{
  m_model.setCurrTime(MOOSTime());
  m_model.iterate();
  postMessages(m_model.getMessages());
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool SRS_MOOSApp::OnStartUp()
{
  cout << termColor("blue");
  MOOSTrace("\nSimulated Range Sensor starting...\n");
  m_model.setCurrTime(MOOSTime());
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
    
  STRING_LIST::iterator p;
  // First Pass
  for(p = sParams.begin(); p!=sParams.end(); p++) {
    string sLine  = *p;
    string param  = stripBlankEnds(MOOSChomp(sLine, "="));
    string value  = stripBlankEnds(sLine);
    m_model.setParam(param, value);
  }
  // Second Pass
  for(p = sParams.begin(); p!=sParams.end(); p++) {
    string sLine  = *p;
    string param  = stripBlankEnds(MOOSChomp(sLine, "="));
    string value  = stripBlankEnds(sLine);
    m_model.setParam(param, value, 2);
  }
  
  RegisterVariables();

  Figlog figlog = m_model.getFigLog();
  figlog.print();

  postMessages(m_model.getVisuals());
  
  m_model.print();
  MOOSTrace("Simulated Range Sensor started. \n\n");
  cout << termColor() << flush;
  return(true);
}


//---------------------------------------------------------
// Procedure: postMessages()

void SRS_MOOSApp::postMessages(vector<VarDataPair> msgs)
{
  unsigned int i, vsize = msgs.size();
  for(i=0; i<vsize; i++) {
    string varname = msgs[i].get_var();
    cout << "+++ " << varname;
    if(msgs[i].is_string()) {
      m_Comms.Notify(varname, msgs[i].get_sdata());
      cout << msgs[i].get_sdata() << endl;
    }
    else {
      m_Comms.Notify(varname, msgs[i].get_ddata());
      cout << msgs[i].get_ddata() << endl;
    }
  }
}

