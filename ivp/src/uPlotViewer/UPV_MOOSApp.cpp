/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: UPV_MOOSApp.cpp                                      */
/*    DATE: May 18th, 2012                                       */
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

#include <iostream>
#include "UPV_MOOSApp.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

UPV_MOOSApp::UPV_MOOSApp() 
{
  m_pending_moos_events = 0;
  m_gui                 = 0; 
}

//----------------------------------------------------------------
// Procedure: OnNewMail

bool UPV_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
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

bool UPV_MOOSApp::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//-------------------------------------------------------------
// Procedure: Iterate
//      Note: virtual overide of base class CMOOSApp member.

bool UPV_MOOSApp::Iterate()
{
  if((!m_gui) || (!m_pending_moos_events))
    return(true);
  
  MOOS_event e;
  e.type="Iterate";
  e.moos_time = MOOSTime();

  m_pending_moos_events->enqueue(e);
  Fl::awake();
  
  return(true);
}

//--------------------------------------------------------
// Procedure: onStartUp()

bool UPV_MOOSApp::OnStartUp()
{
  MOOSTrace("uPlotViewer starting....\n");
  SetAppFreq(4);

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

void UPV_MOOSApp::registerVariables()
{
  unsigned int i, vsize = m_plot_vars.size();
  for(i=0; i<vsize; i++)
    m_Comms.Register(m_plot_vars[i], 0);
}

//----------------------------------------------------------------------
// Procedure: handleNewMail  (OnNewMail)

void UPV_MOOSApp::handleNewMail(const MOOS_event & e)
{
  unsigned int handled_msgs = 0;
  for(size_t i=0; i<e.mail.size(); ++i) {
    CMOOSMsg msg = e.mail[i].msg;
    string key   = msg.GetKey();
    string sval  = msg.GetString();
    string community = msg.GetCommunity();
    string source = msg.GetSource();

    bool handled = false;
    if(vectorContains(m_plot_vars, key)) {
      double plot_val = msg.GetDouble();
      m_gui->viewer()->addValue(key, plot_val);
      handled = true;
    }
    if(handled)
      handled_msgs++;
  }

  if(handled_msgs > 0) {
    if(m_gui) {
      m_gui->updateOutput();
      m_gui->viewer()->redraw();
    }
  }
}


//----------------------------------------------------------------------
// Procedure: handleIterate

void UPV_MOOSApp::handleIterate(const MOOS_event & e) 
{
  if(!m_gui)
    return;
}


//----------------------------------------------------------------------
// Procedure: handleStartUp (OnStartUp)

void UPV_MOOSApp::handleStartUp(const MOOS_event & e) 
{
  if(!m_gui)
    return;

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  STRING_LIST::reverse_iterator p;
  for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
    string line  = *p;
    string param = stripBlankEnds(tolower(biteString(line, '=')));
    string value = stripBlankEnds(line);
    
    // plot_var = var=UTEST_VAL, min=0, max=400, bars=20
    if(param == "plot_var") {
      string var   = tokStringParse(value, "var", ',', '=');
      string min   = tokStringParse(value, "min", ',', '=');
      string max   = tokStringParse(value, "max", ',', '=');
      string bars  = tokStringParse(value, "bars", ',', '=');
      string color = tokStringParse(value, "color", ',', '=');
      if(!strContainsWhite(var) && 
	 isNumber(min) && isNumber(max) && isNumber(bars)) {
	double dmin = atof(min.c_str());
	double dmax = atof(max.c_str());
	int    dbars = atoi(bars.c_str());
	unsigned int udbars = 0;
	if(dbars > 0)
	  udbars = (unsigned int) dbars;
	m_gui->viewer()->addPartition(var, dmin, dmax, udbars, color);
	m_gui->addScopeVar(var);
	m_plot_vars.push_back(var);
      }
    }
  }
  
  m_gui->viewer()->redraw();
  
  registerVariables();
}









