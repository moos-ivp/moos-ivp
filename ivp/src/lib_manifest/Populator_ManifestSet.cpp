/************************************************************/
/*   NAME: Michael Benjamin                                 */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA */
/*   FILE: Populator_ManifestSet.cpp                        */
/*   DATE: Dec 14th, 2012 (Sandy Hook)                      */
/*   DATE: Dec 17th, 2017 (resumption of development)       */
/************************************************************/

#include <cstdlib>
#include <iostream>
#include "FileBuffer.h" 
#include "Manifest.h"
#include "Populator_ManifestSet.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------
// Constructor

Populator_ManifestSet::Populator_ManifestSet()
{
  m_verbose = false;
  m_exit_on_failure = true;
}

//----------------------------------------------------------
// Procedure: populate()

bool Populator_ManifestSet::populate() 
{
  if(m_manifest_files.size() == 0) {
    cout << "Aborted: An input (.mfs) file must be provided." << endl;
    exit(1);
  }

  for(unsigned int i=0; i<m_manifest_files.size(); i++) {

    bool ok = populateManifest(m_manifest_files[i]);
    if(!ok) {
      cout << "Invalid Manifest file: " << m_manifest_files[i] << endl;
      return(false);
    }
  }
  
  for(unsigned int i=0; i<m_loc_files.size(); i++) {
    bool ok = populateLOC(m_loc_files[i]);
    if(!ok) {
      cout << "Invalid LOC file: " << m_loc_files[i] << endl;      
      return(false);
    }
  }

  m_manifest_set.associateDependers();
  m_manifest_set.associateLinesOfCode();

  return(true);
}


//----------------------------------------------------------
// Procedure: populateLOC(string)

bool Populator_ManifestSet::populateLOC(string filename) 
{
  // Each line is expected to be of the format:
  // <appname>  <lines_of_code>
  // or
  // <appname>  <lines_of_code>  <files_of_code>
  // or
  // <appname>  <lines_of_code>  <files_of_code>  <work_years>

  if(m_verbose)
    cout << "Handling LOC File: " << filename << endl;
  
  vector<string> lines = fileBuffer(filename);
  if(lines.size() == 0)
    return(false);

  for(unsigned int i=0; i<lines.size(); i++) {
    string line = stripBlankEnds(lines[i]);
    if(strBegins(line, "//"))
      continue;
    if(line == "")
      continue;

    vector<string> svector = parseString(line, ',');

    string module_name, loc_str, foc_str, wkyrs_str;
    
    for(unsigned int j=0; j<svector.size(); j++) {
      string param = tolower(biteStringX(svector[j], '='));
      string value = svector[j];

      if(param == "module")
	module_name = value;
      else if((param == "loc") && isNumber(value))
	loc_str = value;
      else if((param == "foc") && isNumber(value))
	foc_str = value;
      else if((param == "wkyrs") && isNumber(value))
	wkyrs_str = value;
      else {
	cout << "Invalid param on line " << i+1 << ": " << param << endl;
	return(false);
      }
    }	

    if(loc_str != "") {
      int loc_int = atoi(loc_str.c_str());
      if(loc_int >= 0)
	m_manifest_set.setLinesOfCode(module_name, loc_int);
    }
    if(foc_str != "") {
      int foc_int = atoi(foc_str.c_str());
      if(foc_int >= 0)
	m_manifest_set.setFilesOfCode(module_name, foc_int);
    }
    if(wkyrs_str != "") {
      double wkyrs_dbl = atof(wkyrs_str.c_str());
      if(wkyrs_dbl >= 0)
	m_manifest_set.setWorkYears(module_name, wkyrs_dbl);
    }
  }
  
  return(true);
}


//----------------------------------------------------------
// Procedure: populateManifest(string)

bool Populator_ManifestSet::populateManifest(string filename) 
{
  if(m_verbose)
    cout << "Handling Manifest File: " << filename << endl;
  
  vector<string> lines = fileBuffer(filename);
  if(lines.size() == 0)
    return(false);

  // Part 1: First Pass through all the lines. Collapse all multi-line
  // cases into a single line for later handling in Part 2.
  vector<string> xlines;
  string curr_line;
  for(unsigned int i=0; i<lines.size(); i++) {
    string line = lines[i];
    if(strBegins(stripBlankEnds(line), "//"))
      continue;
    if(line == "")
      continue;
    if((line.at(0) == ' ') || (line.at(0) == '\t')) {
      curr_line += " " + stripBlankEnds(line);
      continue;
    }
    
    line = stripBlankEnds(line);
    string linekey = getLineKey(line);

    if((linekey != "") && !validLineKey(linekey)) {
      cout << "Invalid line key in File [" << filename << "]" << endl;
      cout << line << endl;
      return(false);
    }

    if(validLineKey(linekey)) {
      if(curr_line != "")
	xlines.push_back(curr_line);
      curr_line = line;
    }
    else 
      curr_line += " " + line;
  }
  if(curr_line != "")
    xlines.push_back(curr_line);

  // Part 2: Second Pass assumes all lines are of param=value format

  bool all_ok = true;
  for(unsigned int i=0; i<xlines.size(); i++) {
    string orig = xlines[i];
    string line = stripBlankEnds(xlines[i]);
    string lline = tolower(line);

    //cout << "   Line [" << i << "]: " << line << endl;

    string param = tolower(biteStringX(line, '='));
    string value = line;
    
    if(param == "module") {
      if(m_curr_manifest.getModuleName() != "") {
	m_manifest_set.addManifest(m_curr_manifest);
	m_curr_manifest = Manifest();
      }
      m_curr_manifest.setModuleName(value);
    }
    else if(param == "type")
      m_curr_manifest.setType(value);
    else if(param == "thumb")
      m_curr_manifest.setThumb(value);
    else if((param == "author") || (param == "authors"))
      m_curr_manifest.addAuthor(value);
    else if(param == "doc_url")
      m_curr_manifest.setDocURL(value);
    else if(param == "contact")
      m_curr_manifest.setContactEmail(value);
    else if(param == "org")
      m_curr_manifest.addOrg(value);
    else if((param == "group") || (param == "groups"))
      m_curr_manifest.addGroup(value);
    else if(param == "synopsis")
      m_curr_manifest.addSynopsisLine(value);
    else if(param == "depends")
      m_curr_manifest.addLibDependency(value);
    else if(param == "borndate") {
      bool ok_date = m_curr_manifest.setDate(value);
      if(!ok_date) {
	cout << "Not ok borndate!! [" << filename << "][" << value << "]" << endl;
	exit(1);
      }
    }
    
    else if(param == "license")
      m_curr_manifest.setLicense(value);
    else if(param == "distro")
      m_curr_manifest.setDistro(value);
    else if(param == "mod_date")
      m_curr_manifest.addModDate(value);
    else {
      cout << "Unhandled line [" << i+1 << "]: " << orig << endl;
      all_ok = false;
    }
  }

  // When done with all lines, there is an "open" current manifest
  // that has yet to be added, so add it here (unless it is empty)
  if(m_curr_manifest.getModuleName() != "") {
    bool unique = m_manifest_set.addManifest(m_curr_manifest);
    if(!unique) {
      cout << "End of manifest file [" << filename << "]";
      return(false);
    }
    m_curr_manifest = Manifest();
    
  }
  else {
    cout << "End of manifest file [" << filename << "]";
    cout << " and still no module name!" << endl;
    return(false);
  }
  
  return(all_ok);
}


//----------------------------------------------------------
// Procedure: getLineKey

string Populator_ManifestSet::getLineKey(string line) const
{
  if(!strContains(line, '='))
    return("");
  
  string front = biteStringX(line, '=');

  // If there is white space in this supposed key, then it is
  // not likely to be a key, but rather a segment of text, e.g.
  // "and then set DEPLOY=true".
  if(strContainsWhite(front))
    return("");


  return(front);
}

//----------------------------------------------------------
// Procedure: validLineKey

bool Populator_ManifestSet::validLineKey(string linekey) const
{
  if(linekey == "module")
    return(true);
  else if(linekey == "type")
    return(true);
  else if(linekey == "author")
    return(true);
  else if(linekey == "contact")
    return(true);
  else if(linekey == "org")
    return(true);
  else if(linekey == "thumb")
    return(true);
  else if(linekey == "synopsis")
    return(true);
  else if(linekey == "depends")
    return(true);
  else if(linekey == "borndate")
    return(true);
  else if(linekey == "doc_url")
    return(true);
  else if(linekey == "license")
    return(true);
  else if(linekey == "group")
    return(true);
  else if(linekey == "distro")
    return(true);
  else if(linekey == "mod_date")
    return(true);

  return(false);
}

