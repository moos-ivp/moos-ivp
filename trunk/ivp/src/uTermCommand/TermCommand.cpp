/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: TermCommand.cpp                                      */
/*    DATE: June 26th 2007                                       */
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

#include <iterator>
#include <cstring>
#include "TermCommand.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------
// Constructor

TermCommand::TermCommand()
{
  m_iteration = 0;
  m_memory_ix = -1;
}

//------------------------------------------------------------
// Procedure: OnNewMail

bool TermCommand::OnNewMail(MOOSMSG_LIST &NewMail)
{
  return(true);
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool TermCommand::OnConnectToServer()
{
  return(true);
}

//------------------------------------------------------------
// Procedure: Iterate

bool TermCommand::Iterate()
{
  m_tc_mutex.Lock();
  if(m_iteration==0)
    handleCharInput('\n');

  m_iteration++;

  m_tc_mutex.UnLock();

  return(true);
}

//------------------------------------------------------------
// Procedure: OnStartUp

bool TermCommand::OnStartUp()
{
  CMOOSApp::OnStartUp();
  
  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(true);
  m_MissionReader.GetConfigurationAndPreserveSpace(GetAppName(), sParams);
    
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine     = *p;

    string sVarName  = MOOSChomp(sLine, "=");
    sVarName = toupper(sVarName);
    sLine    = stripBlankEnds(sLine);

    if(MOOSStrCmp(sVarName, "CMD"))
      addCommand(sLine);
  }
  
  return(true);
}

//------------------------------------------------------------
// Procedure: addCommand()

void TermCommand::addCommand(string cmd_str)
{
  cmd_str = findReplace(cmd_str, "-->", ",");

  string var_key  = tolower(biteStringX(cmd_str,','));
  string var_name = biteStringX(cmd_str,',');
  string var_val  = biteStringX(cmd_str,',');

  if((var_key=="") || (var_name==""))
    return;

  for(unsigned int i=0; i<m_var_key.size(); i++)
    if(var_key == m_var_key[i])
      return;

  string var_type = "double";
  if(isQuoted(var_val)) {
    var_val  = stripQuotes(var_val);
    var_type = "string";
  }
  if(!isNumber(var_val))
    var_type = "string";
      
  m_var_key.push_back(var_key);
  m_var_name.push_back(var_name);
  m_var_type.push_back(var_type);
  m_var_val.push_back(var_val);
}


//------------------------------------------------------------
// Procedure: getPartialKeyMatches
//      Note: Return the entries where the cmd buffer matches 
//            the first N characters of the key. 

vector<int> TermCommand::getPartialKeyMatches()
{
  vector<int> rvector;
  
  int buff_len = m_cmd_buffer.length();
  
  unsigned int i, vsize = m_var_key.size();
  for(i=0; i<vsize; i++) {
      if(!strncmp(m_var_key[i].c_str(), m_cmd_buffer.c_str(), buff_len)) {
	if(m_var_key[i] != m_cmd_buffer)
	  rvector.push_back(i);
      }
  }

  return(rvector);
}

//------------------------------------------------------------
// Procedure: getFullKeyMatch
//      Note: Return index of an entry if it matches perfectly, 
//            or if it matches partially and is the only one.

int TermCommand::getFullKeyMatch()
{
  unsigned int i, vsize = m_var_key.size();
  for(i=0; i<vsize; i++) {
    if(m_var_key[i] == m_cmd_buffer)
      return(i);
  }

  vector<int> rvector = getPartialKeyMatches();
  if(rvector.size()==1)
    return(rvector[0]);
  else
    return(-1);
}

//------------------------------------------------------------
// Procedure: printMapping()

void TermCommand::printMapping()
{
  unsigned int i, vsize = m_var_key.size();
  for(i=0; i<vsize; i++) {
    cout << "[" << i << "]: ";
    cout << m_var_key[i] << ",";
    cout << m_var_name[i] << ",";
    cout << m_var_type[i] << ",";
    cout << m_var_val[i] << endl;
  }
}


//------------------------------------------------------------
// Procedure: postCommand

void TermCommand::postCommand(unsigned int ix)
{
  if(ix >= m_var_key.size())
    return;

  if(m_var_type[ix] == "string") 
    Notify(m_var_name[ix], m_var_val[ix]);
  else {
    double dval = atof(m_var_val[ix].c_str());
    Notify(m_var_name[ix], dval);
  }

  m_cmds_prev.push_back(m_var_key[ix]);
  m_memory_ix = -1;
  m_cmd_partial = "";
}


//------------------------------------------------------------
// Procedure: handleArrow

void TermCommand::handleArrow(char c)
{
  int cache_size = m_cmds_prev.size();
  if(cache_size == 0)
    return;

  if(c==65) { // up-arrow
    m_memory_ix++;
    if(m_memory_ix >= cache_size)
      m_memory_ix = cache_size-1;
  }

  if(c==66) { // down-arrow
    m_memory_ix--;
    if(m_memory_ix <= -1) {
      m_memory_ix = -1;
      m_cmd_buffer = "";
      m_cmd_buffer = m_cmd_partial;
      return;
    }
  }

  int cache_ix = (cache_size - 1) - m_memory_ix;

  // m_cmd_partial = m_cmd_buffer;
  m_cmd_buffer = m_cmds_prev[cache_ix];
}

//------------------------------------------------------------
// Procedure: tabExpand

//
//    01234567890
//    hello_bob
//    hello_bobby
//    hello_bobcat

void TermCommand::tabExpand()
{
  // First get the indexes of partial matches
  vector<int> rvector = getPartialKeyMatches();

  int rsize = rvector.size();
  if(rsize == 0)
    return;

  int  i;
  int max_common_len = m_var_key[rvector[0]].length();
  for(i=1; i<rsize; i++) {
    int ilen = m_var_key[rvector[i]].length();
    if(ilen < max_common_len)
      max_common_len = ilen;
  }
  
  int  len  = m_cmd_buffer.length();
  bool done = (len >= max_common_len);
  while(!done) {
    char next_char = m_var_key[rvector[0]].c_str()[len];
    bool   all_match = true;
    for(i=1; i<rsize; i++) {
      char   ichar = m_var_key[rvector[i]].c_str()[len];
      all_match &= (next_char == ichar);
    }
    if(all_match) {
      len++;
      done = (len >= max_common_len);
    }
    else
      done = true;
  }
  
  m_cmd_buffer.assign(m_var_key[rvector[0]].c_str(), len);
}

//------------------------------------------------------------
// Procedure: handleCharInput
//  ASCII:
//           09 = tab
//           32 = blank
//           65 = up-arrow
//           66 = down-arrow
//           67 = left-arrow
//           68 = right-arrow

void TermCommand::handleCharInput(char c)
{
  if((c==32)||(c==27)) {
    m_tc_mutex.UnLock();
    return;
  }

  for(int k=0; k<100; k++)
    printf("\n");
  
  if((c==65) || (c==66))
    handleArrow(c);

  if(c==9) {
    tabExpand();
    m_cmd_partial = m_cmd_buffer;
  }

  // Special case the DELETE key - reduce buff string at end
  if(c==127) {
    int buff_len = m_cmd_buffer.length();
    if(buff_len >= 1)
      m_cmd_buffer.assign(m_cmd_buffer.c_str(), buff_len-1);
  } 
  else {
    if((c!=10) && (c!=9) && (c!=65) && (c!=66)) {
      m_cmd_buffer += c;
      m_cmd_partial = m_cmd_buffer;
    }
  }

  int res = getFullKeyMatch();
  if((res != -1) && (c==10)) {
    postCommand(res);
    m_cmd_buffer = "";
    res = -1;
  }

  printf("  %-24s", "Cue");
  printf(" %-32s", "VarName");
  printf("VarValue\n");

  printf("  %-24s", "-----------");
  printf("%-32s", "---------------");
  printf(" --------------\n");

  vector<int> completions = getPartialKeyMatches();
  unsigned int vsize = completions.size();
  
  if(vsize > 1) {
    for(int j=vsize-1; j>=0; j--) {
      int i = completions[j];

      printf("  %-24s ", m_var_key[i].c_str());
      printf("%-32s", m_var_name[i].c_str());
      if(m_var_type[i] == "string") {
	if(m_var_val[i] != "n/a") {
	  printf("\"%s\"", m_var_val[i].c_str());
	}
	else
	  printf("n/a");
      }
      else if(m_var_type[i] == "double")
	printf("%s", m_var_val[i].c_str());
      printf("\n");		
    }
    printf("\n");		
  }

  if(res != -1) {
    printf("  %-24s ", m_var_key[res].c_str());
    printf("%-32s", m_var_name[res].c_str());
    if(m_var_type[res] == "string") {
      if(m_var_val[res] != "n/a") {
	printf("\"%s\"", m_var_val[res].c_str());
      }
      else
	printf("n/a");
    }
    else if(m_var_type[res] == "double")
      printf("%-15s", m_var_val[res].c_str());
    printf(" <-- SELECT \n\n");		

  }

  if((vsize==0) && (res==-1))
    printf("\n  NO MATCH\n\n");

  printf("> %s\n", m_cmd_buffer.c_str());
}










