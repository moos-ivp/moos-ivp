/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PokeDB.cpp                                           */
/*    DATE: May 9th 2008                                         */
/*          Motivated by Matt Grund's uMOOSPoke App              */
/*****************************************************************/

#include "PokeDB.h"

using namespace std;

//------------------------------------------------------------
// Procedure: Iterate()

bool PokeDB::Iterate()
{
  if((MOOSTime() - m_start_time) > 0.5)
    exit(0);
  
  return(true);
}

//------------------------------------------------------------
// Procedure: OnStartUp()

bool PokeDB::OnStartUp()
{
  CMOOSApp::OnStartUp();
  
  m_start_time = MOOSTime();

  return(true);
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool PokeDB::OnConnectToServer()
{
  int vsize = m_varname.size();

  for(int i=0; i<vsize; i++) {
    if(m_valtype[i] == "double") 
      m_Comms.Notify(m_varname[i], m_dvalue[i], MOOSTime());
    else 
      m_Comms.Notify(m_varname[i], m_svalue[i], MOOSTime());
  }  

  return(true);
}

//------------------------------------------------------------
// Procedure: setPokeDouble

void PokeDB::setPokeDouble(const string& varname, double value)
{
  m_varname.push_back(varname);
  m_dvalue.push_back(value);
  m_svalue.push_back("");
  m_valtype.push_back("double");
}

//------------------------------------------------------------
// Procedure: setPokeString

void PokeDB::setPokeString(const string& varname, const string& value)
{
  m_varname.push_back(varname);
  m_dvalue.push_back(0);
  m_svalue.push_back(value);
  m_valtype.push_back("string");
}
