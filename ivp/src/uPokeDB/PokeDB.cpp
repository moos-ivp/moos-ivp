/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PokeDB.cpp                                           */
/*    DATE: May 9th 2008                                         */
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

#include "PokeDB.h"
#include "MBUtils.h"
#include "VarDataPairUtils.h"
#include "ACTable.h"

using namespace std;

extern bool MOOSAPP_OnConnect(void*);
extern bool MOOSAPP_OnDisconnect(void*);

//------------------------------------------------------------
// Constructor()

PokeDB::PokeDB()
{
  m_db_start_time = 0; 
  m_db_time       = 0;
  m_iteration     = 0; 
  m_sServerHost   = "localhost"; 
  m_lServerPort   = 9000;

  m_configure_comms_locally = false;

  m_time_warp_set = false;
  m_priors_reported = false;
  m_values_poked  = false;
  m_poked_reported = false;

  m_use_cache = false;
  
  m_poke_iteration = 0;
}

//------------------------------------------------------------
// Constructor()

PokeDB::PokeDB(string g_server_host, long int g_server_port)
{
  m_db_start_time = 0; 
  m_db_time       = 0;
  m_iteration     = 0; 
  m_sServerHost   = g_server_host; 
  m_lServerPort   = g_server_port;

  m_configure_comms_locally = false;

  m_time_warp_set = false;
  m_priors_reported = false;
  m_values_poked  = false;
  m_poked_reported = false;

  m_poke_iteration = 0;
}

//------------------------------------------------------------
// Procedure: ConfigureComms()
//      Note: Overload the MOOSApp::ConfigureComms implementation
//            which would have grabbed the port/host info from the
//            .moos file instead.

bool PokeDB::ConfigureComms()
{
  //cout << "PokeDB::ConfigureComms:" << endl;
  //cout << "  m_sServerHost: " << M_Sserverhost << endl;
  //cout << "  m_lServErport: " << m_lServerPort << endl;

  if(!m_configure_comms_locally) 
    return(CMOOSApp::ConfigureComms());

  //register a callback for On Connect
  m_Comms.SetOnConnectCallBack(MOOSAPP_OnConnect, this);
  
  //and one for the disconnect callback
  m_Comms.SetOnDisconnectCallBack(MOOSAPP_OnDisconnect, this);
  
  //start the comms client....
  if(m_sMOOSName.empty())
    m_sMOOSName = m_sAppName;
  
  m_nCommsFreq = 10;

  m_Comms.Run(m_sServerHost.c_str(), 
	      m_lServerPort,
	      m_sMOOSName.c_str(), 
	      m_nCommsFreq);
  
  return(true);
}

//------------------------------------------------------------
// Procedure: setTimeWarp()

void PokeDB::setTimeWarp()
{
  if(m_time_warp_set)
    return;

  // We DONT try to auto-set the warp if it has already been set to
  // a value greater than 1.
  if(GetMOOSTimeWarp() > 1) {
    m_time_warp_set = true;
    return;
  }

  if(m_db_time == 0)
    return;

  double curr_time = MOOSTime();
  if(curr_time <= 0)
    return;
  
  double calculated_warp = m_db_time / curr_time;
  if(calculated_warp <= 0)
    return;
  
  SetMOOSTimeWarp(calculated_warp);
  m_db_start_time *= calculated_warp;

  cout << "Calculated Time Warp: " << doubleToString(calculated_warp,2);
  m_time_warp_set = true;
}

//------------------------------------------------------------
// Procedure: Iterate()

bool PokeDB::Iterate()
{
  m_iteration++;

  // Make sure we have a chance to receive and read incoming mail
  // on the poke variables, prior to having their new values poked.
  if((m_iteration == 1) || (m_db_time == 0))
    return(true);

  if(!m_time_warp_set) {
    setTimeWarp();
    return(true);
  }
  
  if(!m_priors_reported) {
    printReport();
    m_priors_reported = true;
    return(true);
  }

  // After the first iteration, poke the all the scheduled values.
  if(!m_values_poked) {
    unsigned int i, vsize = m_varname.size();
    for(i=0; i<vsize; i++) {
      string varval = m_varvalue[i];
      if(strContains(varval, "@MOOSTIME")) {
	double curr_time = MOOSTime();
	string stime = doubleToStringX(curr_time, 2);
	varval = findReplace(varval, "@MOOSTIME", stime);
      }
      if(m_valtype[i] == "double")
	Notify(m_varname[i], atof(varval.c_str()) );
      else 
	Notify(m_varname[i], varval);
    }
    m_values_poked = true;
    m_poke_iteration = m_iteration;

    return(true);
  }
  
  if((m_poke_iteration - m_iteration) >= 2) {
    printReport();
    exit(0);
  }

  return(true);
}


//  UTC Time                       UPTIME
//  000 - 1970
//
//  100 - UTC start of MOOS        0 - UPTIME start of MOOS
//  >500 - UTC start of MOOS*      0 - UPTIME start of MOOS
//  
//  150 - UTC sometime later       50 - UPTIME sometime later
//  >750 - UTC sometime later      250 - UPTIME sometime later


//------------------------------------------------------------
// Procedure: OnNewMail()

bool PokeDB::OnNewMail(MOOSMSG_LIST &NewMail)
{    
  MOOSMSG_LIST::iterator p;

  // First scan the mail for the DB_UPTIME message to get an 
  // up-to-date value of DB uptime *before* handling other vars
  if(m_db_start_time == 0) {
    for(p=NewMail.begin(); p!=NewMail.end(); p++) {
      CMOOSMsg &msg = *p;
      if(msg.GetKey() == "DB_UPTIME") 
	m_db_start_time = MOOSTime() - msg.GetDouble();
      else if(msg.GetKey() == "DB_TIME") 
	m_db_time = msg.GetDouble();
    }
  }
  
  // Update the values of all variables we have registered for.  
  // All variables "values" are stored as strings. We let MOOS
  // tell us the type of the variable, and we keep track of the
  // type locally, just so we can put quotes around string values.
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    updateVariable(msg);
  }
  return(true);
}

//------------------------------------------------------------
// Procedure: OnStartUp()

bool PokeDB::OnStartUp()
{
  CMOOSApp::OnStartUp();

  // Newly added Jan 21, 2024, ability to add a set of pokes
  // from a configuration block. These lines are the "poke cache"
  // This allows auto-starts to a mission by just generically
  // invoking a poke of whatever is configured to be in the cache.
  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
  
  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = true;
    if(param == "poke") {  // poke=DEPLOY=true
      if(m_use_cache)
	handled = setPoke(value);
    }
    else
      handled = false;

    if(!handled)
      cout << "Unhandle uPokeDB config: " << orig << endl;
  }

  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: OnConnectToServer()

bool PokeDB::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: anyVarsReceived()

bool PokeDB::anyVarsReceived() const
{
  for(unsigned int i=0; i<m_varname_recd.size(); i++)
    if(m_varname_recd[i])
      return(true);
  return(false);
}

//------------------------------------------------------------
// Procedure: allVarsReceived()

bool PokeDB::allVarsReceived() const
{
  for(unsigned int i=0; i<m_varname_recd.size(); i++)
    if(!m_varname_recd[i])
      return(false);
  return(true);
}

//------------------------------------------------------------
// Procedure: clearVarsReceived()

void PokeDB::clearVarsReceived()
{
  for(unsigned int i=0; i<m_varname_recd.size(); i++)
    m_varname_recd[i] = false;
}

//------------------------------------------------------------
// Procedure: setPoke()
//   Purpose: Examine a FOO=bar string and separate it into
//            left/right parts and determine string or double

bool PokeDB::setPoke(string pstr)
{
  if(!strContains(pstr, "="))
    return(false);

  if(strContains(pstr, ":=")) {
    vector<string> svector = parseString(pstr, ":=");
    if(svector.size() != 2)
      return(false);
    
    string left  = stripBlankEnds(svector[0]);
    string right = stripBlankEnds(svector[1]);
    setPokeString(left, right);
    return(true);
  }
    
  string left  = biteStringX(pstr, '=');
  string right = pstr;
  if(right == "@UTC")
    right = "@MOOSTIME";
  
  if(isNumber(right))
    setPokeDouble(left, right);
  else
    setPokeString(left, right);

  return(true);
}


//------------------------------------------------------------
// Procedure: setPokeDouble()

void PokeDB::setPokeDouble(string varname, string value)
{
  m_varname.push_back(varname);
  m_valtype.push_back("double");
  m_varvalue.push_back(value);

  m_dvalue_read.push_back("");
  m_svalue_read.push_back("");
  m_source_read.push_back("");
  m_valtype_read.push_back("");
  m_wrtime_read.push_back(0);
  m_varname_recd.push_back(false);
}

//------------------------------------------------------------
// Procedure: setPokeString()

void PokeDB::setPokeString(string varname, string value)
{
  m_varname.push_back(varname);
  m_valtype.push_back("string");
  m_varvalue.push_back(value);

  m_dvalue_read.push_back("");
  m_svalue_read.push_back("");
  m_source_read.push_back("");
  m_valtype_read.push_back("");
  m_wrtime_read.push_back(0);
  m_varname_recd.push_back(false);
}

//------------------------------------------------------------
// Procedure: registerVariables()

void PokeDB::registerVariables()
{
  for(unsigned int i=0; i<m_varname.size(); i++) 
    Register(m_varname[i], 0);

  Register("DB_UPTIME", 0);
  Register("DB_TIME", 0);
}

//------------------------------------------------------------
// Procedure: updateVariable()
//      Note: Will read a MOOS Mail message and grab the fields
//            and update the variable only if its in the vector 
//            of variables vector<string> vars.

void PokeDB::updateVariable(CMOOSMsg &msg)
{
  string varname = msg.GetKey();  
  
  int ix = -1;
  unsigned int index, vsize = m_varname.size();
  for(index=0; index<vsize; index++)
    if(m_varname[index] == varname)
      ix = index;
  if(ix == -1)
    return;

  double vtime = msg.GetTime();
  
  m_source_read[ix] = msg.GetSource();
  m_wrtime_read[ix] = vtime;

  if(msg.IsString()) {
    m_svalue_read[ix]  = msg.GetString();
    m_valtype_read[ix] = "string";
  }      
  else if(msg.IsDouble()) {
    m_dvalue_read[ix]  = doubleToString(msg.GetDouble());
    m_valtype_read[ix] = "double";
  }
}

//------------------------------------------------------------
// Procedure: postFlags()

void PokeDB::postFlags(const vector<VarDataPair>& flags)
{
  for(unsigned int i=0; i<flags.size(); i++) {
    VarDataPair pair = flags[i];
    string moosvar = pair.get_var();
    
    // If posting is a double, just post. No macro expansion
    if(!pair.is_string()) {
      double dval = pair.get_ddata();
      Notify(moosvar, dval);
    }
    // Otherwise if string posting, handle macro expansion
    else {
      string sval = pair.get_sdata();
      Notify(moosvar, sval);
    }
  }
}

//------------------------------------------------------------
// Procedure: printReport()

#if 0
void PokeDB::printReport()
{
  printf("  %-24s", "VarName");
  
  printf("%-12s", "(S)ource");
  printf("%-12s", "(T)ime");
  printf("VarValue\n");
  
  printf("  %-24s", "----------------");
  printf("%-12s", "----------");
  printf("%-12s", "----------");
  printf(" -------------\n");
  
  unsigned int i, vsize = m_varname.size();
  for(i=0; i<vsize; i++) {
    double wrtime_dval = 0;
    string wrtime_sval;
    if(m_wrtime_read[i] > 0) {
      wrtime_dval = m_wrtime_read[i] - m_db_start_time;
      wrtime_dval = wrtime_dval / GetMOOSTimeWarp();
      wrtime_sval = doubleToString(wrtime_dval,2);
    }

    printf("  %-24s ", m_varname[i].c_str());
    printf("%-12s",    m_source_read[i].c_str());
    printf("%-12s",    wrtime_sval.c_str());
    if(m_valtype_read[i] == "string") {
      if(m_svalue_read[i] != "") {
	printf("\"%s\"", m_svalue_read[i].c_str());
      }
      else
	printf("n/a");
    }
    else if(m_valtype_read[i] == "double")
      printf("%s", m_dvalue_read[i].c_str());
    printf("\n");		
  }
}
#endif


//------------------------------------------------------------
// Procedure: printReport()

void PokeDB::printReport()
{
  ACTable actab(4);
  actab << "VarName | Source | Time | VarValue";
  actab.addHeaderLines();
  
  for(unsigned int i=0; i<m_varname.size(); i++) {
    double wrtime_dval = 0;
    string wrtime_sval;
    if(m_wrtime_read[i] > 0) {
      wrtime_dval = m_wrtime_read[i] - m_db_start_time;
      wrtime_dval = wrtime_dval / GetMOOSTimeWarp();
      wrtime_sval = doubleToString(wrtime_dval,2);
    }
    
    string varval="n/a";    
    if(m_valtype_read[i] == "string") {
      if(m_svalue_read[i] != "") {
	varval = "\"" + m_svalue_read[i] + "\"";
      }
    }
    else if(m_valtype_read[i] == "double")
      varval = m_dvalue_read[i];
    
    actab << m_varname[i] << m_source_read[i] << wrtime_sval << varval;
  }

  cout << endl;
  vector<string> lines = actab.getTableOutput();
  for(unsigned int i=0; i<lines.size(); i++)
    cout << lines[i] << endl;
  
}

