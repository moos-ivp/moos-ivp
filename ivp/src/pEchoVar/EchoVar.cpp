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

#include <cstring>
#include <iterator>
#include "EchoVar.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool EchoVar::OnNewMail(MOOSMSG_LIST &NewMail)
{
  unsigned int i, vsize = m_var_source.size();
  unsigned int j, fsize = m_eflippers.size();

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
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

    for(j=0; j<fsize; j++) {
      if(key == m_eflippers[j].getSourceVar()) {
	string flip_result = m_eflippers[j].flip(sdata);
	if(flip_result != "")
	  m_Comms.Notify(m_eflippers[j].getDestVar(), flip_result);
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
  iteration++;
  m_Comms.Notify("ECHO_ITER", iteration);
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
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    
    list<string>::reverse_iterator p;
    for(p=sParams.rbegin(); p!=sParams.rend(); p++) {
      string original_line = *p;
      string sLine  = stripBlankEnds(*p);
      string sKey   = stripBlankEnds(MOOSChomp(sLine, "="));

      cout << "sKey: [" << sKey << "]" << endl;
      cout << "  sLine: [" << sLine << "]" << endl;

      sKey = tolower(sKey);
      if(!strncmp(sKey.c_str(), "flip", 4)) {
	bool ok = handleFlipEntry(sKey, sLine);
	if(!ok) {
	  cout << "Probem with " << original_line << endl;
	  return(false);
	}
      }
      else if(sKey == "echo") {
	sLine = findReplace(sLine, "->", ">");
	vector<string> svector = parseString(sLine, '>');
	if(svector.size() != 2)
	  return(false);
	string sLeft  = stripBlankEnds(svector[0]);
	string sRight = stripBlankEnds(svector[1]);
	bool ok = addMapping(sLeft, sRight);
	if(!ok) { 
	  cout << "Probem with " << original_line << endl;
	  return(false);
	}
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

  unsigned int i, vsize = m_eflippers.size();
  for(i=0; i<vsize; i++) {
    m_eflippers[i].print();
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
  
  for(unsigned int j=0; j<m_eflippers.size(); j++)
    m_Comms.Register(m_eflippers[j].getSourceVar(), 0);
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

//-----------------------------------------------------------------
// Procedure: handleFlipEntry

bool EchoVar::handleFlipEntry(string sKey, string sLine)
{
  vector<string> svector = parseString(sKey, ':');
  unsigned int vsize = svector.size();
  if(vsize != 2) 
    return(false);
  string tag = stripBlankEnds(svector[0]);
  string key = stripBlankEnds(svector[1]);
  if(tolower(tag) != "flip") {
    cout << "probhere" << endl;
    return(false);
  }

  // Determine the index of the given flipper reference
  int index = -1;
  unsigned int i, esize = m_eflippers.size();
  for(i=0; i<esize; i++) {
    if(key == m_eflippers[i].getKey())
      index = i;
  }
  
  // If pre-existing EFlipper not found, create a new one
  if(index == -1) {
    EFlipper new_flipper;
    new_flipper.setParam("key", key);
    m_eflippers.push_back(new_flipper);
    index = esize;
  }
    
  if(!strncmp(sLine.c_str(), "source_variable", 15)) {
    string left = biteString(sLine, '=');
    string right = stripBlankEnds(sLine);
    bool ok = m_eflippers[index].setParam("source_variable", right);
    return(ok);
  }
  if(!strncmp(sLine.c_str(), "dest_variable", 13)) {
    string left = biteString(sLine, '=');
    string right = stripBlankEnds(sLine);
    bool ok = m_eflippers[index].setParam("dest_variable", right);
    return(ok);
  }
  if(!strncmp(sLine.c_str(), "source_separator", 16)) {
    string left = biteString(sLine, '=');
    string right = stripBlankEnds(sLine);
    bool ok = m_eflippers[index].setParam("source_separator", right);
    return(ok);
  }
  if(!strncmp(sLine.c_str(), "dest_separator", 14)) {
    string left = biteString(sLine, '=');
    string right = stripBlankEnds(sLine);
    bool ok = m_eflippers[index].setParam("dest_separator", right);
    return(ok);
  }
  if(!strncmp(sLine.c_str(), "filter", 6)) {
    string left = biteString(sLine, '=');
    string right = stripBlankEnds(sLine);
    bool ok = m_eflippers[index].setParam("filter", right);
    return(ok);
  }
  if(strContains(sLine, "==")) {
    bool ok = m_eflippers[index].setParam("filter", sLine);
    return(ok);
  }
  if(!strncmp(sLine.c_str(), "component", 9)) {
    string left = biteString(sLine, '=');
    string right = stripBlankEnds(sLine);
    bool ok = m_eflippers[index].setParam("component", right);
    return(ok);
  }
  if(strContains(sLine, "->")) {
    bool ok = m_eflippers[index].setParam("component", sLine);
    return(ok);
  }

  return(false);
}


