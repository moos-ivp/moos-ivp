/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MOOSAppWrapper.cpp                                   */
/*    DATE: June 14th 2012                                       */
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
#include "MOOSAppWrapper.h"

using namespace std;

//------------------------------------------------------------------
// Procedure: Iterate()

bool MOOSAppWrapper::Iterate()
{
  if(!m_pseudo_moos_app)
    return(false);
  return(m_pseudo_moos_app->Iterate());
}

//------------------------------------------------------------------
// Procedure: OnNewMail

bool MOOSAppWrapper::OnNewMail(MOOSMSG_LIST& NewMail)
{
  if(!m_pseudo_moos_app)
    return(false);

  list<AppMsg> app_messages;
  
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    AppMsg app_msg = MOOSMsg2AppMsg(*p);
    app_messages.push_back(app_msg);
  }
  
  return(m_pseudo_moos_app->OnNewMail(app_messages));
}

//------------------------------------------------------------------
// Procedure: OnStartUp()

bool MOOSAppWrapper::OnStartUp()
{
  if(!m_pseudo_moos_app)
    return(false);
  return(m_pseudo_moos_app->OnStartUp());
}

//------------------------------------------------------------------
// Procedure: OnConnectToServer()

bool MOOSAppWrapper::OnConnectToServer()
{
  if(!m_pseudo_moos_app)
    return(false);
  return(m_pseudo_moos_app->OnConnectToServer());
}

//------------------------------------------------------------------
// Procedure: SetMOOSApp()

void MOOSAppWrapper::SetMOOSApp(PseudoMOOSApp *app)
{
  if(!app)
    return;

  m_pseudo_moos_app = app;
  m_pseudo_moos_app->m_owner = this;
  m_pseudo_moos_app->GetConfiguration = &MOOSAppWrapper::GetConfiguration;

  m_pseudo_moos_app->m_Comms.NotifyStr    = &MOOSAppWrapper::NotifyStr;
  m_pseudo_moos_app->m_Comms.NotifyStrAux = &MOOSAppWrapper::NotifyStrAux;
  m_pseudo_moos_app->m_Comms.NotifyDbl    = &MOOSAppWrapper::NotifyDbl;
  m_pseudo_moos_app->m_Comms.NotifyDblAux = &MOOSAppWrapper::NotifyDblAux;
  m_pseudo_moos_app->m_Comms.NotifyPtr    = &MOOSAppWrapper::NotifyPtr;
  m_pseudo_moos_app->m_Comms.NotifyPtrAux = &MOOSAppWrapper::NotifyPtrAux;
}

//------------------------------------------------------------------
// Procedure: MOOSMsg2AppMsg

AppMsg MOOSAppWrapper::MOOSMsg2AppMsg(const CMOOSMsg& msg)
{
  if(msg.IsString()) {
    AppMsg app_msg(msg.GetKey(), msg.GetString(), msg.GetTime()); 
    app_msg.SetCommunity(msg.GetCommunity());
    app_msg.SetSource(msg.GetSource());
    app_msg.SetSourceAux(msg.GetSourceAux());
    if(msg.IsDataType(MOOS_BINARY_STRING))
      app_msg.MarkAsBinary();
    return(app_msg);
  }
  else {
    AppMsg app_msg(msg.GetKey(), msg.GetDouble(), msg.GetTime()); 
    app_msg.SetCommunity(msg.GetCommunity());
    app_msg.SetSource(msg.GetSource());
    app_msg.SetSourceAux(msg.GetSourceAux());
    return(app_msg);
  }
}
 
//=================================================================
//                           Call Backs
//=================================================================
 
//-----------------------------------------------------------------
// Procedure: GetConfiguration()
 
void MOOSAppWrapper::GetConfiguration(list<string>& configs)
{
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), configs);
}

//-----------------------------------------------------------------
// Procedure: Notify Callbacks
 
void MOOSAppWrapper::NotifyStr(const string& varname, const string& strval,
			       double time)
{
  m_Comms.Notify(varname, strval, time);
}

//-----------------------------------------------------------------
void MOOSAppWrapper::NotifyStrAux(const string& varname, const string& strval,
				  const string& srcaux,  double time)
{
  m_Comms.Notify(varname, strval, srcaux, time);
}

//-----------------------------------------------------------------
void MOOSAppWrapper::NotifyDbl(const string& varname, double dval,
			       double time)
{
  m_Comms.Notify(varname, dval, time);
}

//-----------------------------------------------------------------
void MOOSAppWrapper::NotifyDblAux(const string& varname, double dval,
				  const string& srcaux,  double time)
{
  m_Comms.Notify(varname, dval, srcaux, time);
}

//-----------------------------------------------------------------
void MOOSAppWrapper::NotifyPtr(const string& varname,  void* ptr_data, 
			       unsigned int size_data, double time)
{
  m_Comms.Notify(varname, ptr_data, size_data, time);
}

//-----------------------------------------------------------------
void MOOSAppWrapper::NotifyPtrAux(const string& varname,  void* ptr_data,
				  unsigned int size_data, const string& srcaux,  
				  double time)
{
  m_Comms.Notify(varname, ptr_data, size_data, srcaux, time);
}

