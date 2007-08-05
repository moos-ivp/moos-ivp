/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TermCommand.cpp                                      */
/*    DATE: June 26th 2007                                       */
/*****************************************************************/

#include <iostream>
#include <iterator>
#include <string.h>
#include "TermCommand.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------
// Constructor

TermCommand::TermCommand()
{
  m_iteration = 0;
}

//------------------------------------------------------------
// Procedure: OnNewMail

bool TermCommand::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::reverse_iterator p;
  
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;
  }
  
  return(true);
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool TermCommand::OnConnectToServer()
{
  // register for variables here
  // possibly look at the mission file?
  // m_MissionReader.GetConfigurationParam("Name", <string>);
  // m_Comms.Register("VARNAME", is_float(int));
  
  return(true);
}

//------------------------------------------------------------
// Procedure: Iterate

bool TermCommand::Iterate()
{
  if(m_iteration==0)
    handleCharInput('\n');

  //printMapping();
  m_iteration++;
  return(true);
}

//------------------------------------------------------------
// Procedure: OnStartUp

bool TermCommand::OnStartUp()
{
  CMOOSApp::OnStartUp();
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
    
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string sLine     = *p;
    string sVarName  = MOOSChomp(sLine, "=");
    sVarName = toupper(sVarName);
    sLine    = stripBlankEnds(sLine);

    int len  = sLine.length();
    
    if(MOOSStrCmp(sVarName, "CMD")) {
      addCommand(sLine);
    }
  }
  
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: addCommand()

void TermCommand::addCommand(string cmd_str)
{
  cmd_str = findReplace(cmd_str, "-->", " ");
  cmd_str = stripBlankEnds(cmd_str);

  vector<string> vector_a = chompString(cmd_str, ' ');

  cout << "CMD_STR: " << cmd_str << endl;
  cout << "vector_a[0]: " << vector_a[0] << endl;
  cout << "vector_a[1]: " << vector_a[1] << endl;

  if(vector_a.size() != 2)
    return;
  string var_key = stripBlankEnds(vector_a[0]);
  
  // Don't allow duplicate keys
  for(int i=0; i<m_var_key.size(); i++)
    if(var_key == m_var_key[i])
      return;

  vector_a[1] = stripBlankEnds(vector_a[1]);
  vector<string> vector_b = chompString(vector_a[1], ' ');
  if(vector_b.size() != 2)
    return;

  string var_name = stripBlankEnds(vector_b[0]);
  string var_val  = stripBlankEnds(vector_b[1]);
  
  string var_type = "double";
  if(isQuoted(var_val)) {
    var_val  = stripQuotes(var_val);
    var_type = "string";
  }
  if(!isNumber(var_val))
    var_type = "string";
      
  cout << "Key: " << var_key << " VAR: " << var_name << " VAL: " << var_val << endl;


  m_var_key.push_back(var_key);
  m_var_name.push_back(var_name);
  m_var_type.push_back(var_type);
  m_var_val.push_back(var_val);
}


//------------------------------------------------------------
// Procedure: registerVariables()

void TermCommand::registerVariables()
{
}


//------------------------------------------------------------
// Procedure: getPartialKeyMatches
//      Note: Rreturn the entries where the cmd buffer matches 
//            the first N characters of the key. 

vector<int> TermCommand::getPartialKeyMatches()
{
  vector<int> rvector;
  
  int buff_len = m_cmd_buffer.length();
  
  int vsize = m_var_key.size();
  for(int i=0; i<vsize; i++) {
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
  int vsize = m_var_key.size();
  for(int i=0; i<vsize; i++) {
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
  m_iteration++;

  for(int j=0; j<5; j++)
    printf("\n");

  printf("  %-12s", "Key");
  printf("%-22s", "VarName");
  printf("VarValue\n");

  printf("  %-12s", "-----------");
  printf("%-22s", "---------------");
  printf(" -------------- (%d)\n", m_iteration);

  int vsize = m_var_name.size();
  for(int i=vsize-1; i>=0; i--) {
    printf("  %-12s ", m_var_key[i].c_str());
    
    printf("%-22s", m_var_name[i].c_str());
    
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
}


//------------------------------------------------------------
// Procedure: postCommand

void TermCommand::postCommand(int ix)
{
  if((ix < 0) || (ix >= m_var_key.size()))
    return;

  if(m_var_type[ix] == "string") 
    m_Comms.Notify(m_var_name[ix], m_var_val[ix]);
  else {
    double dval = atof(m_var_val[ix].c_str());
    m_Comms.Notify(m_var_name[ix], dval);
  }
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

void TermCommand::handleCharInput(char c)
{
  if((c==32)||(c==65)||(c==66)||(c==67)||(c==68)||(c==27)||(c==91))
    return;
  
  for(int k=0; k<100; k++)
    printf("\n");
  
  if(c==9)
    tabExpand();

  // Special case the DELETE key - reduce buff string at end
  if(c==127) {
    int buff_len = m_cmd_buffer.length();
    if(buff_len >= 1)
      m_cmd_buffer.assign(m_cmd_buffer.c_str(), buff_len-1);
  } 
  else
    if((c!=10) && (c!=9))
      m_cmd_buffer += c;
  
  //cout << "Saw char: " << (int)(c) << endl;

  int res = getFullKeyMatch();
  if((res != -1) && (c==10)) {
    postCommand(res);
    m_cmd_buffer = "";
    res = -1;
  }

  printf("  %-24s", "Key");
  printf(" %-32s", "VarName");
  printf("VarValue\n");

  printf("  %-24s", "-----------");
  printf("%-32s", "---------------");
  printf(" --------------\n");

  vector<int> completions = getPartialKeyMatches();
  int vsize = completions.size();
  
  if(vsize > 1) {
    for(int j=vsize-1; j>=0; j--) {
      int i = completions[j];
      printf("  %-24s ", m_var_key[i].c_str());
      printf("%-32s", m_var_name[i].c_str());
      if(m_var_type[i] == "string") {
	if(m_var_val[i] != "n/a") {
	  printf("\"%-15s\"", m_var_val[i].c_str());
	}
	else
	  printf("n/a");
      }
      else if(m_var_type[i] == "double")
	printf("%-15s", m_var_val[i].c_str());
      printf("\n");		
    }
    printf("\n");		
  }

  if(res != -1) {
    printf("  %-24s ", m_var_key[res].c_str());
    printf("%-32s", m_var_name[res].c_str());
    if(m_var_type[res] == "string") {
      if(m_var_val[res] != "n/a") {
	printf("\"%-15s\"", m_var_val[res].c_str());
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

  cout << "> " << m_cmd_buffer << flush;

}

