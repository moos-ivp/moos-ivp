/************************************************************/
/*   NAME: Michael Benjamin                                 */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA */
/*   FILE: ManifestTester.cpp                               */
/*   DATE: Jan 9th, 2018                                    */
/************************************************************/

#include <cstdio>
#include <cctype>
#include <iostream>
#include <sstream>
#include "Manifest.h"
#include "ManifestTester.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------
// Constructor

ManifestTester::ManifestTester() 
{
  m_verbose  = false;
  m_warnings = false;
  m_depends  = false;
  m_stats    = false;
}

//----------------------------------------------------------
// Procedure: setIgnoreLibs()

void ManifestTester::setIgnoreLibs(string libs)
{
  libs = stripBlankEnds(libs);
  if(libs == "")
    return;
  
  vector<string> svector = parseString(libs, ':');
  m_ignore_libs = svector;
}

//----------------------------------------------------------
// Procedure: test()

bool ManifestTester::test() 
{
  bool ok1 = testForDuplicates();
  bool ok2 = testForErrors();
  
  bool ok3 = true;
  if(m_warnings)
    ok3 = testForWarnings();

  bool ok4 = true;
  if(m_depends)
    ok4 = testDependencies();

  bool ok5 = true;
  if(m_stats)
    ok5 = testStatistics();

  return(ok1 && ok2 && ok3 && ok4 && ok5);
}

//----------------------------------------------------------
// Procedure: testForDuplicates()

bool ManifestTester::testForDuplicates() 
{
  unsigned int msize = m_manifest_set.size();
  if(msize < 2) {
    cout << "Phase 2: All manifests have a unique module name:  PASS" << endl;
    return(true);
  }

  vector<string> duplicate_modules;
  
  for(unsigned int i=0; i<msize-1; i++) {
    Manifest manifest_i = m_manifest_set.getManifestByIndex(i);
    string module_name_i = manifest_i.getModuleName();

    for(unsigned int j=i+1; j<msize; j++) {
      Manifest manifest_j = m_manifest_set.getManifestByIndex(j);
      string module_name_j = manifest_j.getModuleName();
      if(module_name_i == module_name_j)
	duplicate_modules.push_back(module_name_i);
    }
  }

  if(duplicate_modules.size() == 0) {
    cout << "Phase 2: All manifests have a unique module name:  PASS" << endl;
    return(true);
  }

 
  cout << "Phase 2: All manifests have a unique module name:  FAIL" << endl;
  if(!m_verbose)
    return(false);
  
  for(unsigned int i=0; i<duplicate_modules.size(); i++) 
    cout << "  [" << duplicate_modules[i] << "] found more than once. "<< endl;

  return(false);
}


//----------------------------------------------------------
// Procedure: testForErrors()

bool ManifestTester::testForErrors() 
{
  unsigned int msize = m_manifest_set.size();
  if(msize == 0) {
    cout << "Phase 3: All manifests have all required fields:   FAIL" << endl;
    if(m_verbose)
      cout << "         Empty Manifest Set                        " << endl;
    return(false);
  }

  map<string, vector<string> > failed_manifests;
  for(unsigned int i=0; i<msize; i++) {
    Manifest manifest = m_manifest_set.getManifestByIndex(i);
    unsigned int errcnt = manifest.missingMandatoryCnt();
    if(errcnt != 0) {
      string module_name = manifest.getModuleName();

      vector<string> svector = manifest.missingMandatory();
      for(unsigned int j=0; j<svector.size(); j++) 
	failed_manifests[module_name].push_back(svector[j]);

    }
  }

  if(failed_manifests.size() == 0) {
    cout << "Phase 3: All manifests have all required fields:   PASS" << endl;
    if(m_verbose)
      cout << "         " << msize << " manifests checked. " << endl;
    return(true);
  }

  cout << "Phase 3: All manifests have all required fields:   FAIL" << endl;
  if(!m_verbose)
    return(false);
  
  map<string, vector<string> >::iterator p;
  for(p=failed_manifests.begin(); p!= failed_manifests.end(); p++) {
    
    cout << "         Module[" << p->first << "] missing fields: " << endl;
    cout << "            " << flush;
    vector<string> missing = p->second;
    for(unsigned int i=0; i<missing.size(); i++) {
      if(i > 0)
	cout << ", ";
      cout << missing[i];
    }
    cout << endl;
  }
  
  return(false);
}


//----------------------------------------------------------
// Procedure: testForWarnings()

bool ManifestTester::testForWarnings() 
{
  map<string, vector<string> > manifest_warnings;

  unsigned int msize = m_manifest_set.size();
  for(unsigned int i=0; i<msize; i++) {
    Manifest manifest = m_manifest_set.getManifestByIndex(i);
    unsigned int warning_count = manifest.missingOptionalCnt();
    if(warning_count != 0) {
      string module_name = manifest.getModuleName();

      vector<string> svector = manifest.missingOptional();
      for(unsigned int j=0; j<svector.size(); j++) 
	manifest_warnings[module_name].push_back(svector[j]);

    }
  }

  if(manifest_warnings.size() == 0) {
    cout << "Phase 4: All manifests have all optional fields:   PASS" << endl;
    if(m_verbose)
      cout << "         " << msize << " manifests checked.          " << endl;
    return(true);
  }

  cout << "Phase 4: All manifests have all optional fields:   FAIL" << endl;
  if(!m_verbose)
    return(false);
  
  map<string, vector<string> >::iterator p;
  for(p=manifest_warnings.begin(); p!= manifest_warnings.end(); p++) {
    
    cout << "         Module[" << p->first << "] missing optional fields: "<<endl;
    cout << "            " << flush;
    vector<string> missing = p->second;
    for(unsigned int i=0; i<missing.size(); i++) {
      if(i > 0)
	cout << ", ";
      cout << missing[i];
    }
    cout << endl;
  }
  
  return(false);
}


//----------------------------------------------------------
// Procedure: testDependencies()

bool ManifestTester::testDependencies() 
{
  map<string, vector<string> > depend_warnings;

  unsigned int msize = m_manifest_set.size();
  for(unsigned int i=0; i<msize; i++) {
    Manifest manifest = m_manifest_set.getManifestByIndex(i);
    string module_name = manifest.getModuleName();

    vector<string> dependencies = manifest.getLibDepends();
    for(unsigned int j=0; j<dependencies.size(); j++) {
      string lib_dependency = dependencies[j];
      if(!vectorContains(m_ignore_libs, lib_dependency)) {
	if(!m_manifest_set.containsLibrary(lib_dependency))
	  depend_warnings[module_name].push_back(dependencies[j]);
      }
    }
  }

  if(depend_warnings.size() == 0) {
    cout << "Phase 5: All manifest dependency libs are known:   PASS" << endl;
    if(m_verbose) 
      cout << "         " << msize << " manifests checked.            " << endl;
    return(true);
  }
  
  cout << "Phase 5: All manifest dependency libs are known:   FAIL" << endl;
  if(!m_verbose)
    return(false);
  
  map<string, vector<string> >::iterator p;
  for(p=depend_warnings.begin(); p!= depend_warnings.end(); p++) {
    
    cout << "         Module[" << p->first << "] Unknown dependencies: "<<endl;
    cout << "            " << flush;
    vector<string> missing = p->second;
    for(unsigned int i=0; i<missing.size(); i++) {
      if(i > 0)
	cout << ", ";
      cout << missing[i];
    }
    cout << endl;
  }
  
  return(false);
}

//----------------------------------------------------------
// Procedure: testStatistics()

bool ManifestTester::testStatistics() 
{
  map<string, vector<string> > loc_warnings;

  unsigned int msize = m_manifest_set.size();
  for(unsigned int i=0; i<msize; i++) {
    Manifest manifest = m_manifest_set.getManifestByIndex(i);
    string module_name = manifest.getModuleName();

    string mtype = tolower(manifest.getType());
    if((mtype == "behavior") || (mtype == "group"))
      continue;
	
    if(manifest.getLinesOfCode() == 0)
      loc_warnings[module_name].push_back("lines_of_code");
    if(manifest.getFilesOfCode() == 0)
      loc_warnings[module_name].push_back("files_of_code");
    if(manifest.getWorkYears() == 0)
      loc_warnings[module_name].push_back("work_years");
  }


  if(loc_warnings.size() == 0) {
    cout << "Phase 6: All manifests have lines-of-code stats:   PASS" << endl;
    if(m_verbose) 
      cout << "         " << msize << " manifests checked.          " << endl;
    return(true);
  }

  cout << "Phase 6: All manifests have lines-of-code stats:   FAIL" << endl;
  
  if(!m_verbose)
    return(false);

  map<string, vector<string> >::iterator p;
  for(p=loc_warnings.begin(); p!= loc_warnings.end(); p++) {
    
    cout << "         Module[" << p->first << "] Missing statistics: "<<endl;
    cout << "            " << flush;
    vector<string> missing = p->second;
    for(unsigned int i=0; i<missing.size(); i++) {
      if(i > 0)
	cout << ", ";
      cout << missing[i];
    }
    cout << endl;
  }
  return(false);
}


