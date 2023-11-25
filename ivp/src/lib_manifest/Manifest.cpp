/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Manifest.h                                           */
/*    DATE: Nov 30th, 2012                                       */
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

#include <cstdlib> 
#include <iostream> 
#include "Manifest.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------
// Constructor

Manifest::Manifest()
{
  m_num_date = 0;

  m_lines_of_code = 0;
  m_files_of_code = 0;
  m_work_years    = 0;

  m_lines_of_code_total = 0;
  m_files_of_code_total = 0;
  m_work_years_total    = 0;
}

//----------------------------------------------------------
// Procedure: setDate()
//    Format: Expected format: YYMMDD

bool Manifest::setDate(string sdate)
{
  if(sdate.length() != 6)
    return(false);
  if(!isNumber(sdate))
    return(false);

  string ystr = sdate.substr(0,2);
  string mstr = sdate.substr(2,2);
  string dstr = sdate.substr(4,2);

  int yval = atoi(ystr.c_str());
  int mval = atoi(mstr.c_str());
  int dval = atoi(dstr.c_str());
  if(yval < 0)
    return(false);
  if((mval > 12) || (mval < 0))
    return(false);
  if((dval > 31) || (dval < 0))
    return(false);

  m_date = sdate;
  m_num_date = (unsigned int)(atoi(sdate.c_str()));
  return(true);
}

//----------------------------------------------------------
// Procedure: addLibDependency()
//      Note: Handle multiple comma-separated deps on one line

void Manifest::addLibDependency(string dep_line)
{
  vector<string> deps = parseString(dep_line, ',');
  for(unsigned int i=0; i<deps.size(); i++) {
    string dep = stripBlankEnds(deps[i]);
    if(!vectorContains(m_lib_dependencies, dep))
      m_lib_dependencies.push_back(dep);
  }
}

//----------------------------------------------------------
// Procedure: addAppDepender()
//      Note: Handle multiple comma-separated deps on one line

void Manifest::addAppDepender(string dep_line)
{
  vector<string> deps = parseString(dep_line, ',');
  for(unsigned int i=0; i<deps.size(); i++) {
    string dep = stripBlankEnds(deps[i]);
    if(!vectorContains(m_app_dependers, dep))
      m_app_dependers.push_back(dep);
  }
}

//----------------------------------------------------------
// Procedure: addAuthor()
//      Note: Handle multiple comma-separated authors on one line

void Manifest::addAuthor(string author_line)
{
  vector<string> authors = parseString(author_line, ',');
  for(unsigned int i=0; i<authors.size(); i++) {
    string author = stripBlankEnds(authors[i]);
    if(!vectorContains(m_authors, author))
      m_authors.push_back(author);
  }
}

//----------------------------------------------------------
// Procedure: addOrg()
//      Note: Handle multiple comma-separated orgs on one line

void Manifest::addOrg(string org_line)
{
  vector<string> orgs = parseString(org_line, ',');
  for(unsigned int i=0; i<orgs.size(); i++) {
    string org = stripBlankEnds(orgs[i]);
    if(!vectorContains(m_organizations, org))
      m_organizations.push_back(org);
  }
}

//----------------------------------------------------------
// Procedure: addGroup()
//      Note: Handle multiple comma-separated groups on one line

void Manifest::addGroup(string group_line)
{
  vector<string> groups = parseString(group_line, ',');
  for(unsigned int i=0; i<groups.size(); i++) {
    string group = stripBlankEnds(groups[i]);
    if(!vectorContains(m_groups, group))
      m_groups.push_back(group);
  }
}

//----------------------------------------------------------
// Procedure: getSynopsisStr()

string Manifest::getSynopsisStr() const
{
  string result;
  for(unsigned int i=0; i<m_synopsis.size(); i++)
    result += " " + stripBlankEnds(m_synopsis[i]);

  return(result);
}

//----------------------------------------------------------
// Procedure: operator<

bool Manifest::operator<(const Manifest& other_manifest) const
{
  if(m_num_date < other_manifest.getNumDate())
    return(true);

  return(false);
}

//----------------------------------------------------------
// Procedure: valid()

bool Manifest::valid() const
{
  return(missingMandatoryCnt() == 0);
}

//----------------------------------------------------------
// Procedure: hasAuthor()

bool Manifest::hasAuthor(string author) const
{
  author = tolower(author);
  for(unsigned int i=0; i<m_authors.size(); i++) {
    string authori = m_authors[i];
    if(author == authori)
      return(true);
    if(strContains(author, authori))
      return(true);
    if(strContains(authori, author))
      return(true);
  }
  return(false);
}

//----------------------------------------------------------
// Procedure: hasDependency

bool Manifest::hasDependency(string dependency) const
{
  dependency = tolower(dependency);
  for(unsigned int i=0; i<m_lib_dependencies.size(); i++) {
    string dependencyi = tolower(m_lib_dependencies[i]);
    if(dependency == dependencyi)
      return(true);
  }
  return(false);
}

//----------------------------------------------------------
// Procedure: hasOrg()

bool Manifest::hasOrg(string org) const
{
  org = tolower(org);
  for(unsigned int i=0; i<m_organizations.size(); i++) {
    if(org == m_organizations[i])
      return(true);
  }
  return(false);
}

//----------------------------------------------------------
// Procedure: hasGroup()

bool Manifest::hasGroup(string group) const
{
  group = tolower(group);
  for(unsigned int i=0; i<m_groups.size(); i++) {
    if(group == tolower(m_groups[i]))
      return(true);
  }
  return(false);
}

//----------------------------------------------------------
// procedure: missingMandatoryCnt()

unsigned int Manifest::missingMandatoryCnt() const
{
  vector<string> missing_mandatory = missingMandatory();
  return(missing_mandatory.size());
}


//----------------------------------------------------------
// Procedure: missingMandatory()
//      Note: Mandatory fields:
//            1. Module Name
//            2. Distro (where to find the module)
//            3. Library dependencies (or "None")
//            4. Synopsis
//            5. One or more authors

vector<string> Manifest::missingMandatory() const
{
  vector<string> rvector;

  if(m_module_name == "")            
    rvector.push_back("module_name");

  string typex = tolower(m_type);
  if((typex != "group") && (m_thumb == ""))
    rvector.push_back("thumb");

  if(m_contact_email == "")   
    rvector.push_back("contact");

  return(rvector);
}

//----------------------------------------------------------
// Procedure: missingOptional()

unsigned Manifest::missingOptionalCnt() const
{
  vector<string> m_missing_optional = missingOptional();
  return(m_missing_optional.size());
}

//----------------------------------------------------------
// Procedure: missingOptional()
//      Note: Optional fields:
//            1. doc_url
//            2. borndate
//            3. contact

vector<string> Manifest::missingOptional() const
{
  vector<string> rvector;

  if(m_distro == "")                 
    rvector.push_back("distro");

  if(m_synopsis.size() == 0)  
    rvector.push_back("synopsis");

  //=============================================================
  // All further checks are not flagged for modules of type group
  if(tolower(m_type) == "group")
    return(rvector);  

  if(m_doc_url == "")                 
    rvector.push_back("doc_url");
  if(m_date == "")   
    rvector.push_back("borndate");
  if(m_authors.size() == 0)   
    rvector.push_back("authors");
  
  string typex = tolower(m_type);
  if((typex != "library") && (typex != "group") && (typex != "behavior")) {
    if(m_lib_dependencies.size() == 0) 
      rvector.push_back("depends");
  }
    

  return(rvector);
}


//----------------------------------------------------------
// Procedure: print()

void Manifest::print() const
{
  cout << "Module   = " << m_module_name   << endl;

  if(m_doc_url != "")
    cout << "doc_url  = " << m_doc_url       << endl;

  if(m_type != "") 
    cout << "type     = " << m_type          << endl;

  if(m_thumb != "") 
    cout << "thumb    = " << m_thumb         << endl;

  if(m_distro != "")
    cout << "distro   = " << m_distro        << endl;

  if(m_date != "")
    cout << "date     = " << m_date          << endl;

  if(m_contact_email != "")
    cout << "contact  = " << m_contact_email << endl;

  if(m_license != "")
    cout << "license  = " << m_license       << endl;

  if(m_lines_of_code > 0)
    cout << "lines_of_code  = " << m_lines_of_code  << endl;

  if(m_lines_of_code_total > 0)
    cout << "lines_of_code_total  = " << m_lines_of_code_total  << endl;

  if(m_files_of_code > 0)
    cout << "files_of_code  = " << m_files_of_code  << endl;

  if(m_files_of_code_total > 0)
    cout << "files_of_code_total  = " << m_files_of_code_total  << endl;

  if(m_work_years > 0)
    cout << "work_years  = " << m_work_years  << endl;

  if(m_work_years_total > 0)
    cout << "work_years_total = " << m_work_years_total  << endl;

  //=========================================================
  if(m_lib_dependencies.size() != 0) {
    cout << "depends  = ";
    for(unsigned int i=0; i<m_lib_dependencies.size(); i++) {
      if(i != 0)
	cout << ", ";
      cout << m_lib_dependencies[i];
    }
    cout << endl;
  }

  //=========================================================
  if(m_app_dependers.size() != 0) {
    cout << "dependers = ";
    for(unsigned int i=0; i<m_app_dependers.size(); i++) {
      if(i != 0)
	cout << ", ";
      cout << m_app_dependers[i];
    }
    cout << endl;
  }
    
  //=========================================================
  if(m_authors.size() != 0) {
    cout << "author   = ";
    for(unsigned int i=0; i<m_authors.size(); i++) {
      if(i != 0)
	cout << ", ";
      cout << m_authors[i];
    }
    cout << endl;
  }

  //=========================================================
  if(m_organizations.size() != 0) {
    cout << "org      = ";
    for(unsigned int i=0; i<m_organizations.size(); i++) {
      if(i != 0)
	cout << ", ";
      cout << m_organizations[i];
    }
    cout << endl;
  }
  
  //=========================================================
  if(m_groups.size() != 0) {
    cout << "group    = ";
    for(unsigned int i=0; i<m_groups.size(); i++) {
      if(i != 0)
	cout << ", ";
      cout << m_groups[i];
    }
    cout << endl;
  }
    
  //=========================================================
  if(m_synopsis.size() != 0) {
    cout << "synopsis = ";
    for(unsigned int i=0; i<m_synopsis.size(); i++) { 
      if(i != 0)
	cout << ", ";
      cout << m_synopsis[i];
    }
    cout << endl;
  }

}

//----------------------------------------------------------
// Procedure: printTerse()

void Manifest::printTerse() const
{
  cout << "* " << m_module_name << " (" << m_distro << ") ";
  if(m_synopsis.size() != 0)
    cout << m_synopsis[0].substr(0, 45) << endl;
}


