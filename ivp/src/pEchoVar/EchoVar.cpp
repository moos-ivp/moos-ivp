/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: EchoVar.cpp                                          */
/*    DATE: July 22 2006                                         */
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
#include "EchoVar.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool EchoVar::OnNewMail(MOOSMSG_LIST &NewMail)
{
  int i;
  int vsize = m_var_source.size();

  MOOSMSG_LIST::reverse_iterator p;
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;

    string key   = msg.m_sKey;
    string sdata = msg.m_sVal;
    double ddata = msg.m_dfVal;
    char   mtype = msg.m_cDataType;

    for(i=0; i<vsize; i++) {
      if(key == m_var_source[i]) {
	string new_key = m_var_target[i];
	if(mtype == MOOS_DOUBLE)
	  m_Comms.Notify(new_key, ddata);
	else if(mtype == MOOS_STRING)
	  m_Comms.Notify(new_key, sdata);
      }
    }
  }
  return(true);
}

//-----------------------------------------------------------------
// Procedure: OnConnectoToServer()

bool EchoVar::OnConnectToServer()
{
  // register for variables here
  registerVariables();
  return(true);
}

//-----------------------------------------------------------------
// Procedure: Iterate()
//      Note: Happens AppTick times per second

bool EchoVar::Iterate()
{
  return(true);
}


//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool EchoVar::OnStartUp()
{
  CMOOSApp::OnStartUp();
  MOOSTrace("pEchoVar starting....\n");

  list<string> sParams;
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    
    list<string>::iterator p;
    for(p = sParams.begin();p!=sParams.end();p++) {
      string sLine  = stripBlankEnds(*p);
      string sKey   = stripBlankEnds(MOOSChomp(sLine, "="));

      sKey = tolower(sKey);
      if(sKey == "echo") {
	sLine = findReplace(sLine, "->", ">");
	vector<string> svector = parseString(sLine, '>');
	if(svector.size() != 2)
	  return(false);
	string sLeft  = stripBlankEnds(svector[0]);
	string sRight = stripBlankEnds(svector[1]);
	bool ok = addMapping(sLeft, sRight);
	if(!ok) 
	  return(false);
      }
    }
  }

  bool ok = noCycles();
  if(ok)
    MOOSTrace("No Cycles Detected - proceeding with startup.\n");
  else {
    MOOSTrace("A cycle was detected - aborting the startup.\n");
    return(false);
  }

  registerVariables();
  return(true);
}

//-----------------------------------------------------------------
// Procedure: registerVariables()

void EchoVar::registerVariables()
{
  for(unsigned int i=0; i<m_unique_sources.size(); i++)
    m_Comms.Register(m_unique_sources[i], 0);
}

//-----------------------------------------------------------------
// Procedure: addMapping

bool EchoVar::addMapping(string src, string targ)
{
  if((src == "") || (targ == ""))
    return(false);
  
  int  i;
  
  bool new_pair = true;
  bool new_src  = true;
  
  int vsize = m_var_source.size();
  for(i=0; i<vsize; i++) {
    if(m_var_source[i] == src) {
      new_src = false;
      if(m_var_target[i] == targ)
	new_pair = false;
    }
  }
   
  if(new_pair) {
    m_var_source.push_back(src);
    m_var_target.push_back(targ);
  }
  
  if(new_src)
    m_unique_sources.push_back(src);
  
  return(true);
}


//-----------------------------------------------------------------
// Procedure: noCycles

bool EchoVar::noCycles()
{
  int  i, j;
  int  vsize    = m_unique_sources.size(); 
  int  map_size = m_var_source.size();;

  vector<string> key_vector;
  vector<string> new_vector;

  for(i=0; i<vsize; i++) {
    key_vector.clear();
    
    for(j=0; j<map_size; j++)
      if(m_unique_sources[i] == m_var_source[j])
	if(!vectorContains(key_vector, m_var_target[j]))
	  key_vector.push_back(m_var_target[j]);

    new_vector = expand(key_vector);
    
    if(vectorContains(new_vector, m_unique_sources[i]))
      return(false);
  }
  return(true);
}



//-----------------------------------------------------------------
// Procedure: expand

vector<string> EchoVar::expand(vector<string> key_vector)
{
  unsigned int  i, j;
  unsigned int  map_size = m_var_source.size();;

  unsigned int vsize = key_vector.size();
  for(i=0; i<vsize; i++) {
    string key = key_vector[i];
    for(j=0; j<map_size; j++)
      if(key == m_var_source[j])
	if(!vectorContains(key_vector, m_var_target[j]))
	  key_vector.push_back(m_var_target[j]);
  }

  if(key_vector.size() == vsize)
    return(key_vector);
  else
    return(expand(key_vector));

}

