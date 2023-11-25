/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ManifestSet.cpp                                      */
/*    DATE: December 17th, 2017                                  */
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

#include <algorithm>
#include <iostream>
#include <map>
#include "ManifestSet.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

ManifestSet::ManifestSet()
{
}

//-----------------------------------------------------------
// Procedure: addManifest

bool ManifestSet::addManifest(Manifest new_manifest)
{
#if 0
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    if(new_manifest.getModuleName() == m_manifests[i].getModuleName())
      return(false);
  }
#endif

  m_manifests.push_back(new_manifest);
  return(true);
}

//-----------------------------------------------------------
// Procedure: setLinesOfCode()

void ManifestSet::setLinesOfCode(string module, unsigned int loc)
{
  m_map_loc[module] = loc;
}

//-----------------------------------------------------------
// Procedure: setFilesOfCode()

void ManifestSet::setFilesOfCode(string module, unsigned int foc)
{
  m_map_foc[module] = foc;
  //cout << "*************************************************" << endl;
  //cout << "Setting FILES OF CODE: " << foc << endl;
  //cout << "*************************************************" << endl;
}

//-----------------------------------------------------------
// Procedure: setWorkYears()

void ManifestSet::setWorkYears(string module, double wyr)
{
  m_map_wyr[module] = wyr;
}

//-----------------------------------------------------------
// Procedure: getManifestByModule

Manifest ManifestSet::getManifestByModule(string module) const
{
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    if(module == m_manifests[i].getModuleName())
      return(m_manifests[i]);
  }
  
  Manifest null_manifest;
  return(null_manifest);
}

//-----------------------------------------------------------
// Procedure: getManifestByIndex()

Manifest ManifestSet::getManifestByIndex(unsigned int index) const
{
  if(index >= m_manifests.size()) {
    Manifest null_manifest;
    return(null_manifest);
  }
  
  return(m_manifests[index]);
}


//-----------------------------------------------------------
// Procedure: getManifestSetByAuthor()

ManifestSet ManifestSet::getManifestSetByAuthor(string author) const
{
  ManifestSet mset;

  for(unsigned int i=0; i<m_manifests.size(); i++) {
    if(m_manifests[i].hasAuthor(author))
      mset.addManifest(m_manifests[i]);
  }
  
  return(mset);
}

//-----------------------------------------------------------
// Procedure: getManifestSetByOrg()

ManifestSet ManifestSet::getManifestSetByOrg(string org) const
{
  ManifestSet mset;

  for(unsigned int i=0; i<m_manifests.size(); i++) {
    if(m_manifests[i].hasOrg(org))
      mset.addManifest(m_manifests[i]);
  }
  
  return(mset);
}

//-----------------------------------------------------------
// Procedure: getManifestSetByGroup()

ManifestSet ManifestSet::getManifestSetByGroup(string group) const
{
  ManifestSet mset;

  for(unsigned int i=0; i<m_manifests.size(); i++) {
    if(m_manifests[i].hasGroup(group))
      mset.addManifest(m_manifests[i]);
  }
  
  return(mset);
}

//-----------------------------------------------------------
// Procedure: getManifestSetByDistro()

ManifestSet ManifestSet::getManifestSetByDistro(string distro) const
{
  ManifestSet mset;

  distro = tolower(distro);
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    if(tolower(m_manifests[i].getDistro()) == distro)
      mset.addManifest(m_manifests[i]);
  }
  
  return(mset);
}

//-----------------------------------------------------------
// Procedure: getManifestSetByDependency()

ManifestSet ManifestSet::getManifestSetByDependency(string dep) const
{
  ManifestSet mset;

  for(unsigned int i=0; i<m_manifests.size(); i++) {
    if(m_manifests[i].hasDependency(dep))
      mset.addManifest(m_manifests[i]);
  }
  
  return(mset);
}

//-----------------------------------------------------------
// Procedure: getManifestSetByType()

ManifestSet ManifestSet::getManifestSetByType(string mtype) const
{
  ManifestSet mset;

  for(unsigned int i=0; i<m_manifests.size(); i++) {
    if(m_manifests[i].getType() == mtype)
      mset.addManifest(m_manifests[i]);
  }
  
  return(mset);
}

//-----------------------------------------------------------
// Procedure: getAllAuthors()

vector<string> ManifestSet::getAllAuthors() const
{
  set<string> author_set;
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    vector<string> authors = m_manifests[i].getAuthors();
    for(unsigned j=0; j<authors.size(); j++) {
      author_set.insert(authors[j]);
    }
  }

  vector<string> rvector; 
  set<string>::iterator p;
  for(p=author_set.begin(); p!=author_set.end(); p++)
    rvector.push_back(*p);

  return(rvector);
}

//-----------------------------------------------------------
// Procedure: getAllAuthorsX()

vector<string> ManifestSet::getAllAuthorsX() const
{
  map<string, unsigned int> author_map;
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    vector<string> authors = m_manifests[i].getAuthors();
    for(unsigned j=0; j<authors.size(); j++) {
      author_map[authors[j]]++;
    }
  }

  vector<string> rvector; 
  map<string, unsigned int>::iterator p;
  for(p=author_map.begin(); p!=author_map.end(); p++) {
    string author = p->first + " (" + uintToString(p->second) + ")";
    rvector.push_back(author);
  }

  return(rvector);
}

//-----------------------------------------------------------
// Procedure: getAllOrgs()

vector<string> ManifestSet::getAllOrgs() const
{
  set<string> org_set;
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    vector<string> orgs = m_manifests[i].getOrgs();
    for(unsigned j=0; j<orgs.size(); j++) {
      org_set.insert(orgs[j]);
    }
  }

  vector<string> rvector; 
  set<string>::iterator p;
  for(p=org_set.begin(); p!=org_set.end(); p++)
    rvector.push_back(*p);

  return(rvector);
}

//-----------------------------------------------------------
// Procedure: getAllGroups()

vector<string> ManifestSet::getAllGroups() const
{
  set<string> group_set;
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    vector<string> groups = m_manifests[i].getGroups();
    for(unsigned j=0; j<groups.size(); j++) {
      group_set.insert(groups[j]);
    }
  }

  vector<string> rvector; 
  set<string>::iterator p;
  for(p=group_set.begin(); p!=group_set.end(); p++)
    rvector.push_back(*p);

  return(rvector);
}

//-----------------------------------------------------------
// Procedure: getAllDistros()

vector<string> ManifestSet::getAllDistros() const
{
  set<string> distro_set;
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    string distro = m_manifests[i].getDistro();
    distro_set.insert(distro);
  }

  vector<string> rvector; 
  set<string>::iterator p;
  for(p=distro_set.begin(); p!=distro_set.end(); p++)
    rvector.push_back(*p);

  return(rvector);
}

//-----------------------------------------------------------
// Procedure: getAllDependencies()

vector<string> ManifestSet::getAllDependencies() const
{
  set<string> dep_set;
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    vector<string> deps = m_manifests[i].getLibDepends();
    for(unsigned j=0; j<deps.size(); j++) {
      dep_set.insert(deps[j]);
    }
  }

  vector<string> rvector; 
  set<string>::iterator p;
  for(p=dep_set.begin(); p!=dep_set.end(); p++)
    rvector.push_back(*p);

  return(rvector);
}

//-----------------------------------------------------------
// Procedure: getAllTypes()

vector<string> ManifestSet::getAllTypes() const
{
  set<string> type_set;
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    string mtype = m_manifests[i].getType();
    type_set.insert(mtype);
  }

  vector<string> rvector; 
  set<string>::iterator p;
  for(p=type_set.begin(); p!=type_set.end(); p++)
    rvector.push_back(*p);

  return(rvector);
}

//-----------------------------------------------------------
// Procedure: getGroupDocURL()

string ManifestSet::getGroupDocURL(string grp_name) const
{
  string result;
  
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    if(tolower(m_manifests[i].getType()) == "group")
      if(m_manifests[i].hasGroup(grp_name))
	result = m_manifests[i].getDocURL();
  }

  return(result);
}

//-----------------------------------------------------------
// Procedure: getGroupDistro()

string ManifestSet::getGroupDistro(string grp_name) const
{
  string result;
  
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    if(tolower(m_manifests[i].getType()) == "group")
      if(m_manifests[i].hasGroup(grp_name))
	result = m_manifests[i].getDistro();
  }

  return(result);
}

//-----------------------------------------------------------
// Procedure: getGroupDownload()

string ManifestSet::getGroupDownload(string grp_name) const
{
  string result;
  
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    if(tolower(m_manifests[i].getType()) == "group")
      if(m_manifests[i].hasGroup(grp_name))
	result = m_manifests[i].getDownload();
  }

  return(result);
}

//-----------------------------------------------------------
// Procedure: getGroupSynopsis()

vector<string> ManifestSet::getGroupSynopsis(string grp_name) const
{
  vector<string> result;
  
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    if(tolower(m_manifests[i].getType()) == "group")
      if(m_manifests[i].hasGroup(grp_name))
	result = m_manifests[i].getSynopsis();
  }

  return(result);
}

//-----------------------------------------------------------
// Procedure: orderNewToOld()

void ManifestSet::orderNewToOld()
{
  std::sort(m_manifests.begin(), m_manifests.end());
}


//-----------------------------------------------------------
// Procedure: orderOldToNew()

void ManifestSet::orderOldToNew()
{
  //std::reverse_sort(m_manifests.begin(), m_manifests.end());
}

//-----------------------------------------------------------
// Procedure: associateDependers()

void ManifestSet::associateDependers()
{
  //cout << "*********************************************************" << endl;
  //cout << "    In associateDependers                                " << endl;
  //cout << "*********************************************************" << endl;
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    //cout << "Type: " << tolower(m_manifests[i].getType()) << endl;
    if(tolower(m_manifests[i].getType()) == "library") {
      string libname = m_manifests[i].getModuleName();
      //cout << "   libname:" << libname << endl;
      for(unsigned int j=0; j<m_manifests.size(); j++) {
	if(m_manifests[j].hasDependency(libname))
	  m_manifests[i].addAppDepender(m_manifests[j].getModuleName());
      }
    }
  }
}

//-----------------------------------------------------------
// Procedure: associateLinesOfCode()

void ManifestSet::associateLinesOfCode()
{
  //cout << "*********************************************************" << endl;
  //cout << "    In associateLinesOfCode                              " << endl;
  //cout << "*********************************************************" << endl;
  
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    string modname = m_manifests[i].getModuleName();
    unsigned int loc = m_map_loc[modname];
    unsigned int foc = m_map_foc[modname];
    double       wyr = m_map_wyr[modname];
    m_manifests[i].setLinesOfCode(loc);
    m_manifests[i].setFilesOfCode(foc);
    m_manifests[i].setWorkYears(wyr);

    unsigned int loc_total = loc;
    unsigned int foc_total = foc;
    double       wyr_total = wyr;
    vector<string> dependencies = m_manifests[i].getLibDepends();
    for(unsigned int j=0; j<dependencies.size(); j++) {
      string dep = dependencies[j];
      loc_total += m_map_loc[dep];
      foc_total += m_map_foc[dep];
      wyr_total += m_map_wyr[dep];
    }
    m_manifests[i].setLinesOfCodeTot(loc_total);
    m_manifests[i].setFilesOfCodeTot(foc_total);
    m_manifests[i].setWorkYearsTot(wyr_total);
  }

  //cout << "*********************************************************" << endl;
  //cout << "  DONE associateLinesOfCode                              " << endl;
  //cout << "*********************************************************" << endl;  
}

//-----------------------------------------------------------
// Procedure: getAllLinesOfCode()

unsigned int ManifestSet::getAllLinesOfCode() const
{
  unsigned int total = 0;
  for(unsigned int i=0; i<m_manifests.size(); i++) 
    total += m_manifests[i].getLinesOfCode();

  return(total);
}

//-----------------------------------------------------------
// Procedure: getAllFilesOfCode()

unsigned int ManifestSet::getAllFilesOfCode() const
{
  unsigned int total = 0;
  for(unsigned int i=0; i<m_manifests.size(); i++) 
    total += m_manifests[i].getFilesOfCode();

  return(total);
}

//-----------------------------------------------------------
// Procedure: getAllWorkYears

double ManifestSet::getAllWorkYears() const
{
  double total = 0;
  for(unsigned int i=0; i<m_manifests.size(); i++) 
    total += m_manifests[i].getWorkYears();

  return(total);
}

//-----------------------------------------------------------
// Procedure: containsLibrary()

bool ManifestSet::containsLibrary(string library) const
{
  string name1 = tolower(library); 
  name1 = findReplace(name1, "_", "");
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    string name2 = tolower(m_manifests[i].getModuleName());
    name2 = findReplace(name2, "_", "");
    string type2 = tolower(m_manifests[i].getType());
    if((name1 == name2) && (type2 == "library"))
      return(true);
  }

  return(false);
}

//-----------------------------------------------------------
// Procedure: size()

unsigned int ManifestSet::size() const
{
  unsigned int count = 0;
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    if(tolower(m_manifests[i].getType()) != "group")
      count++;
  }
  return(count);
}

//-----------------------------------------------------------
// Procedure: print()

void ManifestSet::print() const
{
  cout << "Total Manifests: " << m_manifests.size() << endl;
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    cout << "==================================================" << endl;
    m_manifests[i].print();
  }    
}

//-----------------------------------------------------------
// Procedure: printTerse()

void ManifestSet::printTerse() const
{
  cout << "Total Manifests: " << m_manifests.size() << endl;
  cout << "==================================================" << endl;
  for(unsigned int i=0; i<m_manifests.size(); i++) {
    m_manifests[i].printTerse();
  }
    
}


