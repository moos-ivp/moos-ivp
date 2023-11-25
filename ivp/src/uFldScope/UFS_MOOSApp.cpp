/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: UFS_MOOSApp.cpp                                      */
/*    DATE: Nov 23rd 2011                                        */
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
#include "UFS_MOOSApp.h"
#include "ColorParse.h"
#include "MBUtils.h"
#include "ACTable.h"

using namespace std;

//---------------------------------------------------------
// Constructor

UFS_MOOSApp::UFS_MOOSApp()
{
  m_total_reports    = 0;
  m_layout_index     = 0;
  m_layout_applied   = false;
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool UFS_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
	
    string moos_var   = msg.GetKey();
    //double dval  = msg.GetDouble();
    //double mtime = msg.GetTime();
    //bool   mdbl  = msg.IsDouble();
    //string msrc  = msg.GetSource();
    
    if(m_map_varkeys.count(moos_var)) {
      addPosting(moos_var, msg.GetString());
    }
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool UFS_MOOSApp::OnConnectToServer()
{
  registerVariables();  
  return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()

bool UFS_MOOSApp::Iterate()
{
  AppCastingMOOSApp::Iterate();
  AppCastingMOOSApp::PostReport();
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool UFS_MOOSApp::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);

  STRING_LIST::reverse_iterator p;
  for(p = sParams.rbegin();p!=sParams.rend();p++) {
    string line  = *p;
    string param = stripBlankEnds(toupper(biteString(line, '=')));
    string value = stripBlankEnds(line);

    if(param == "SCOPE") {
      bool ok = addScope(value);
      if(!ok) 
	MOOSTrace("Bad SCOPE parameter: %s\n", param.c_str());
    }
  }

  // Go through parameters twice so we can ensure that the LAYOUT
  // configurations make sense w.r.t. to the SCOPE configurations.
  for(p = sParams.rbegin();p!=sParams.rend();p++) {
    string line  = *p;
    string param = stripBlankEnds(toupper(biteString(line, '=')));
    string value = stripBlankEnds(line);

    if(param == "LAYOUT") {
      bool ok = addLayout(value);
      if(!ok) 
	MOOSTrace("Bad LAYOUT parameter: %s\n", param.c_str());
    }
  }

  registerVariables();
  return(true);
}

//------------------------------------------------------------
// Procedure: registerVariables

void UFS_MOOSApp::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();

  map<string,string>::iterator p = m_map_varkeys.begin();
  while(p != m_map_varkeys.end()) {
    string moos_var = p->first;
    Register(moos_var, 0);
    p++;
  }
}
  
//------------------------------------------------------------
// Procedure: addScope

bool UFS_MOOSApp::addScope(string config_str)
{
  UFS_Config ufs_config(config_str);

  bool valid = ufs_config.valid();
  if(!valid)
    return(false);

  m_config.push_back(ufs_config);

  string moos_var = ufs_config.getMOOSVar();
  string key = ufs_config.getKey();
  m_map_varkeys[moos_var] = key; 

  return(true);
}

//------------------------------------------------------------
// Procedure: addLayout
//   Example: "Time, Speed, Distance"
//   Example: "Time, Speed"
//   Example: "Speed, Distance"
//
//      Note: A check is performed to see if all the fields in the
//            given layout match one of the SCOPE configurations
//            in either the "fld" or "alias". Return true if so.
//      Note: If the check fails we accept the layout anyway, but 
//            the user may get less than what is expected in the 
//            output since mismatches are just dropped.

bool UFS_MOOSApp::addLayout(string config_str)
{
  // Part 1: Add the new layout after parsing and stripping blank
  // ends from each component
  vector<string> new_layout;
  vector<string> svector = parseString(config_str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++)
    new_layout.push_back(stripBlankEnds(tolower(svector[i])));
  m_layouts.push_back(new_layout);
			 
  // Part 2: Perform an error check on the new layout w.r.t. all
  // the scope configurations currently in effect.
  bool all_match = true;
  for(i=0; i<vsize; i++) {
    string this_field = tolower(new_layout[i]);

    bool jmatch = false;
    unsigned int j, jsize = m_config.size();
    for(j=0; j<jsize; j++) {
      string alias = tolower(m_config[j].getAlias());
      string fld = tolower(m_config[j].getField());
      if((alias == this_field) || (fld == this_field))
	jmatch = true;
    }
    all_match = all_match && jmatch;
  }

  return(all_match);
}

//---------------------------------------------------------
// Procedure: addPosting

bool UFS_MOOSApp::addPosting(string moosvar, string msgval)
{
  if(msgval == "")
    return(false);

  string key = m_map_varkeys[moosvar];
  if(key == "")
    return(false);

  string keyval = tokStringParse(msgval, key, ',', '=');
  
  bool found = false;
  unsigned int i, vsize = m_postings_moosvar.size();
  for(i=0; i<vsize; i++) {
    if((moosvar == m_postings_moosvar[i]) && (keyval == m_postings_keyval[i])) {
      found = true;
      m_postings_msgval[i] = msgval;
    }
  }

  if(!found) {
    m_postings_moosvar.push_back(moosvar);
    m_postings_keyval.push_back(keyval);
    m_postings_msgval.push_back(msgval);
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: getPosting

string UFS_MOOSApp::getPosting(string moosvar, string keyval)
{
  unsigned int i, vsize = m_postings_moosvar.size();
  for(i=0; i<vsize; i++) {
    if((moosvar == m_postings_moosvar[i]) && (keyval == m_postings_keyval[i])) {
      return(m_postings_msgval[i]);
    }
  }
  
  // else if not found
  return("");
}


//------------------------------------------------------------
// Procedure: makeReportRaw
//
//
// VName    Speed  Distance  Battery  DCharges  Range
// -----    -----  --------  -------  --------  -----
// charlie   1.21     410.5      72%     8        132
// berta     1.19     388.2      64%     2        180
// alpha     1.12     412.9      79%     4      11600
//
// m_raw_report:
// VName    Speed  Distance  Battery  DCharges  Range
// charlie   1.21     410.5      72%     8        132
// berta     1.19     388.2      64%     2        180
// alpha     1.12     412.9      79%     4      11600

void UFS_MOOSApp::makeReportRaw()
{
  // Part 1: Get a list of Vehicle Names
  set<string> vnames;

  for(unsigned int i=0; i<m_postings_keyval.size(); i++) {
    vnames.insert(m_postings_keyval[i]);
  }

  // Part 2: Build a the first row of the table containing all the
  // headers.
  vector<string> headers;
  headers.push_back("VName");
  unsigned int h, hsize = m_config.size();
  for(h=0; h<hsize; h++) {
    if(configInLayout(m_config[h])) {
	string hdr = m_config[h].getField();
	string alias = m_config[h].getAlias();
	if(alias != "")
	  hdr = alias;
	headers.push_back(hdr);
    }
  }

  // Part 3: For each Vehicle, For each config entry, fill in value
  //
  // In this loop a vector of vectors will be created. One for each
  // row in the table. This table is stored as a member variable so 
  // must be cleared at the outset.
  m_raw_report.clear();
  m_raw_report.push_back(headers);
  set<string>::iterator p2 = vnames.begin();
  while(p2 != vnames.end()) {
    string vname = *p2;
    // In this loop a vector of length equal to the number of configs
    // will be built. One entry for each column of the table.
    vector<string> row;
    row.push_back(vname);
    unsigned int i, vsize = m_config.size();
    for(i=0; i<vsize; i++) {
      if(configInLayout(m_config[i])) {
	string moos_var  = m_config[i].getMOOSVar();
	string latest    = getPosting(moos_var, vname);
	string fld       = m_config[i].getField();
	string value     = tokStringParse(latest, fld, ',', '=');
	row.push_back(value);
      }
    }
    m_raw_report.push_back(row);
    p2++;
  }
}

//------------------------------------------------------------
// Procedure: outputRawReport
//   Purpose: Purely for debugging.

void UFS_MOOSApp::outputRawReport() const
{
  unsigned int i, rows = m_raw_report.size();
  for(i=0; i<rows; i++) {   
    unsigned j, columns = m_raw_report[i].size();
    for(j=0; j<columns; j++)
      cout << m_raw_report[i][j] << "  ";
    cout << endl;
  }  
}

//------------------------------------------------------------
// Procedure: configInLayout
//   Purpose: Determine if the given Scope Config is to be included
//            in a report based on: 
//            (a) Whether layouts generally are being applied presently.
//            (b) If so, which layout is being applied
//            (c) Whether the config is included in the current layout 

bool UFS_MOOSApp::configInLayout(const UFS_Config& config) const
{
  // Part 1: If layouts are not being applied, the ALL Scope Configs
  // are included in all reports. Just return true.
  if(!m_layout_applied) 
    return(true);

  // Part 2: Check the layout index. If out of bounds, just return
  // true indicating that the current config passes. This will be true
  // when no layouts have been provided by the user.
  if(m_layout_index >= m_layouts.size()) 
    return(true);

  // Part 3: Check the config's "fld" against a layout
  string fld_alias = tolower(config.getAlias());
  if(vectorContains(m_layouts[m_layout_index], fld_alias)) 
    return(true);

  // Part 4: Check the config's "alias" against a layout
  string alias = tolower(config.getAlias());
  if(vectorContains(m_layouts[m_layout_index], alias)) 
    return(true);

  return(false);
}


//---------------------------------------------------------
// Procedure: buildReport()
//      Note: A virtual function of the AppCastingMOOSApp superclass, 
//            conditionally invoked if either a terminal or appcast 
//            report is needed.
//   Example:

// VName           Time  MODE  TotDist  TripDist  Speed  
// =====  =============  ====  =======  ========  =====  
// alpha  1357989896.95  PARK  

bool UFS_MOOSApp::buildReport()
{
  makeReportRaw();
  m_total_reports++;

  // Part 1: First ensure we have a non-empty properly formatted raw table.
  unsigned int i, rows = m_raw_report.size();
  if(rows == 0) {
    cout << "Empty Results" << endl;
    return(false);
  }

  unsigned int j, columns = m_raw_report[0].size();

  ACTable actab(columns, 2);

  // Part 3A: Output the Table Header Labels
  for(j=0; j<columns; j++) 
    actab << m_raw_report[0][j];
  actab.addHeaderLines();

  // Part 4: Output the body of the table
  for(i=1; i<rows; i++) {   
    columns = m_raw_report[i].size();
    for(j=0; j<columns; j++) 
      actab << m_raw_report[i][j];
  }

  m_msgs << actab.getFormattedString();
  return(true);
}








