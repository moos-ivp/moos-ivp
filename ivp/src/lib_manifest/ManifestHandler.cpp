/************************************************************/
/*   NAME: Michael Benjamin                                 */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA */
/*   FILE: ManifestHandler.cpp                              */
/*   DATE: Dec 17th, 2012                                   */
/*   DATE: Dec 17th, 2017 (Development resumed 5 yrs later) */
/************************************************************/

#include <cstdio>
#include <cctype>
#include <iostream>
#include <sstream>
#include "Manifest.h"
#include "ManifestHandler.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: genAllManifestPages()

void ManifestHandler::genAllManifestPages() 
{
  if(m_verbose) {
    cout << "******************************************************" << endl;
    cout << "        BEGIN Generating Module Pages                 " << endl;
    cout << "******************************************************" << endl;
  }

  for(unsigned int i=0; i<m_manifest_set.size(); i++) {
    Manifest manifest = m_manifest_set.getManifestByIndex(i);
    genManifestModulePage(manifest);
  }
}

//----------------------------------------------------------
// Procedure: genGroupManifestPages()

void ManifestHandler::genGroupManifestPages() 
{
  if(m_verbose) {
    cout << "******************************************************" << endl;
    cout << "        BEGIN Generating Group Pages                  " << endl;
    cout << "******************************************************" << endl;
  }

  vector<string> all_groups = m_manifest_set.getAllGroups();
  
  for(unsigned int i=0; i<all_groups.size(); i++) {
    string group_name = all_groups[i];
    if(m_verbose)
      cout << "Handling Group: " << group_name << endl;
    genManifestGroupPage(group_name);
  }
  genManifestGroupPage("All");
}

//----------------------------------------------------------
// Procedure: genManifestModulePage()

bool ManifestHandler::genManifestModulePage(Manifest manifest) 
{
  if(!manifest.valid()) {
    cout << "Invalid Manifest in getManifestModulePage()" << endl;
    cout << "     Module Name: " << manifest.getModuleName() << endl;
    return(false);
  }

  string mtype = tolower(manifest.getType());
  
  string module_name = manifest.getModuleName();
  module_name = findReplace(module_name, " ", "");
  module_name = findReplace(module_name, "_", "");
  if(module_name.length() > 0)
    module_name[0] = toupper(module_name[0]);
  string filename = "Manifest." + module_name;
  if(!okFileToWrite(filename)) {
    cout << "Unable to write to file [" << filename << "]" << endl;
    return(false);
  }
  
  // Part 1: Generate the wiki lines in human readable format.
  //         Everything stored in a stringstream.

  stringstream ss;
  if(mtype == "library")
    ss << "!!Library: %color=#ff7f00%'''";
  else if(mtype == "behavior")
    ss << "!!Behavior: %color=#ff7f00%'''";
  else
    ss << "!!Application: %color=#ff7f00%'''";
  ss << manifest.getModuleName();
  ss << "'''%%" << endl;

  ss << "%0a----%0a";
  //ss << "[[<<]]" << endl;
  ss << whiteSpace(3);

  ss << "!!!!Brief Synopsis: " << endl;
  string thumb = manifest.getThumb();
  if(!strEnds(thumb, "."))
    thumb += ".";
  ss << thumb << endl << endl;

  ss << "!!!!Authors:" << endl;
  vector<string> authors = manifest.getAuthors();
  for(unsigned int i=0; i<authors.size(); i++) 
    ss << "* " << authors[i] << endl;
  ss << endl;

  vector<string> groups = manifest.getGroups();
  if(groups.size() != 0) {
    ss << "!!!!Groups:" << endl;
    ss << "* ";
    for(unsigned int i=0; i<groups.size(); i++) {
      string page = findReplace(groups[i], " ", "");
      page = findReplace(groups[i], ".", "p");
      ss << "[[ManifestGrp." << page << " | ";
      ss << groups[i];
      ss << "]]";
      if(i < (groups.size()-1))
	ss << ", ";
      else
	ss << endl << endl;
    }
  }

  if(mtype != "library") {
    ss << "!!!!Dependencies:" << endl;
    ss << "* ";
    vector<string> libdeps = manifest.getLibDepends();
    for(unsigned int i=0; i<libdeps.size(); i++) {
      ss << applyLinks(libdeps[i]);
      if(i < (libdeps.size()-1))
	ss << ", ";
      else
	ss << "." << endl;
    }
  }
  else {
    vector<string> appdeps = manifest.getAppDepends();
    if(appdeps.size() > 0) {
      ss << "!!!!Apps dependent on this library: (";
      ss << appdeps.size() << ")" << endl;
      ss << "* ";
      for(unsigned int i=0; i<appdeps.size(); i++) {
	ss << applyLinks(appdeps[i]);
	if(i < (appdeps.size()-1))
	  ss << ", ";
	else
	  ss << "." << endl;
      }
    }
  }
  
  ss << "!!!!Distribution:" << endl;
  string distro = manifest.getDistro();
#if 0
  if(strEnds(distro, ".org")) {
    if(!strBegins(distro, "www."))
      distro = "www." + distro;
    if(!strBegins(distro, "http://"))
      distro = "http://" + distro;
    distro = "[[" + distro + " | " + distro + "]]";
  }
#endif
  ss << "* " << distro << endl << endl;


  string doc_url = manifest.getDocURL();
  if(doc_url != "") {
    ss << "!!!!Documentation:" << endl;
    ss << "* " << doc_url << endl;
  }
    
  vector<string> kwords = manifest.getKeyWords();
  if(kwords.size() > 0) {
    ss << "!!!!Keywords:" << endl;
    for(unsigned int i=0; i<kwords.size(); i++) 
      ss << "* " << kwords[i] << endl;
    ss << endl;
  }
    
  ss << "!!!!Function: " << endl;

  vector<string> synopsis = manifest.getSynopsis();
  for(unsigned int i=0; i<synopsis.size(); i++)
    synopsis[i] = stripBlankEnds(synopsis[i]);

  for(unsigned int i=0; i<synopsis.size(); i++) {
    if(synopsis[i] == "")
      ss << "[[<<]]";
    else
      ss << synopsis[i];
  }


  ss << "%0a----%0a";
  string loc_str = uintToCommaString(manifest.getLinesOfCode());
  if(loc_str != "0") {
    ss << "!!!!Lines of Code: " << endl;
    ss << "* " << loc_str; 
    if(mtype != "library") {
      string loc_total_str = uintToCommaString(manifest.getLinesOfCodeTot());
      ss << " (with libraries: " << loc_total_str << ")";
    }
    ss <<  endl;
  }
    
  string foc_str = uintToCommaString(manifest.getFilesOfCode());
  if(foc_str != "0") {
    ss << "!!!!Files of Code: " << endl;
    ss << "* " << foc_str;
    if(mtype != "library") {
      string foc_total_str = uintToCommaString(manifest.getFilesOfCodeTot());
      ss << " (with libraries: " << foc_total_str << ")";
    }
    ss <<  endl;
  }
  
  string wyr_str = doubleToStringX(manifest.getWorkYears(),2);
  if(wyr_str != "0") {
    ss << "!!!!Work Years: " << endl;
    ss << "* " << wyr_str;
    if(mtype != "library") {
      string wyr_total_str = doubleToStringX(manifest.getWorkYearsTot(),2);
      ss << " (with libraries: " << wyr_total_str << ")";
    }
    ss <<  endl;
  }
  ss << endl;

  // Part 2: Convert the stringstream into a single string in raw format.
  string human_text = ss.str();
  string wiki_text = findReplace(human_text, "\n", "%0a%0a");

  FILE* f = fopen(filename.c_str(), "w");
  if(f) {
    fprintf(f, "version=pmwiki-2.2.14 ordered=1 urlencoded=1\n");
    fprintf(f, "text=%s\n", wiki_text.c_str());
    fclose(f);
  }
  
  return(true);
}

//----------------------------------------------------------
// Procedure: genManifestGroupPage()

bool ManifestHandler::genManifestGroupPage(string grp_name) 
{
  //==========================================================
  // Part 1: Get the ManifestSet based on given group name  G1
  //==========================================================
  ManifestSet mset = m_manifest_set;
  if(grp_name != "All") 
    mset = m_manifest_set.getManifestSetByGroup(grp_name);
  
  if(mset.size() == 0) {
    cout << "Invalid Manifest in getManifestGroupPage(): " << grp_name << endl;
    return(false);
  }

  
  //==========================================================
  // Part 2: Set the filename and check it is a valid file  G2
  //==========================================================
  string gname = findReplace(grp_name, ".", "p");
  gname = findReplace(gname, " ", "");
  if(gname.length() > 0)
    gname[0] = toupper(gname[0]);
  string filename = "ManifestGrp." + gname;
  if(!okFileToWrite(filename)) {
    cout << "Unable to write to file [" << filename << "]" << endl;
    return(false);
  }
  
  //==========================================================
  // GROUP Page - Part 3A: Begin to set the Wiki Content   G3A
  //==========================================================
  stringstream ss;
  ss << "!!Group: %color=#ff7f00%'''";
  ss << grp_name;
  ss << "'''%%" << endl;

  ss << "%0a----%0a";
  ss << whiteSpace(3);

  //==========================================================
  // GROUP Page - Part 3B: Handle the Group Description    G3B
  //==========================================================
  vector<string> synopsis = m_manifest_set.getGroupSynopsis(grp_name);
  ss << "!!!!Group Description: (" << uintToString(mset.size());
  ss << " modules)" << endl;
  for(unsigned int i=0; i<synopsis.size(); i++)
    synopsis[i] = stripBlankEnds(synopsis[i]);

  for(unsigned int i=0; i<synopsis.size(); i++) {
    if(synopsis[i] == "")
      ss << endl;
    else
      ss << synopsis[i];
  }
  ss << endl << endl;

  //==========================================================
  // GROUP Page - Part 3C: Handle the Documentation Link   G3C
  //==========================================================
  string doc_url = m_manifest_set.getGroupDocURL(grp_name);
  if(doc_url != "") {
    ss << "!!!!Documentation:" << endl;
    ss << "* " << doc_url << endl << endl;
  }

  //==========================================================
  // GROUP Page - Part 4:    Handle Modules                 G4
  //==========================================================

  //==========================================================
  // GROUP Page - Part 4A:   Handle Application            G4A
  //==========================================================
  ss << "!!!!Modules:" << endl;
  for(unsigned int i=0; i<mset.size(); i++) {
    Manifest manifest = mset.getManifestByIndex(i);
    string mtype = tolower(manifest.getType());
    if((mtype=="group") || (mtype=="library") || (mtype=="behavior"))
      continue;

    string modname = manifest.getModuleName();
    modname = findReplace(modname, " ", "");
    string modnamex = findReplace(modname, "_", "");
    if(modnamex.length() > 0)
      modnamex[0] = toupper(modnamex[0]);
    modname = "[[Manifest." + modnamex + " | " + modname + "]]";
    
    string thumb = manifest.getThumb();
    if(!strEnds(thumb, "."))
      thumb += ".";
    ss << "* " << "%color=#0033bb%'''" << modname << ":'''%%";
    ss << " " << thumb << "%0a%0a";
  }

  //==========================================================
  // GROUP Page - Part 4B:   Handle Behaviors              G4B
  //==========================================================
  for(unsigned int i=0; i<mset.size(); i++) {
    Manifest manifest = mset.getManifestByIndex(i);
    string mtype = tolower(manifest.getType());
    if(mtype != "behavior")
      continue;

    string modname = manifest.getModuleName();
    modname = findReplace(modname, " ", "");
    string modnamex = findReplace(modname, "_", "");
    modname = "[[Manifest." + modnamex + " | " + modname + "]]";
    
    string thumb = manifest.getThumb();
    if(!strEnds(thumb, "."))
      thumb += ".";
    ss << "* " << "%color=#0033bb%'''" << modname << ":'''%%";
    ss << " " << thumb << "%0a%0a";
  }

  //==========================================================
  // GROUP Page - Part 4C:   Handle Lbraries               G4C
  //==========================================================
  bool any_libraries = false;
  for(unsigned int i=0; i<mset.size(); i++) {
    Manifest manifest = mset.getManifestByIndex(i);
    string mtype = tolower(manifest.getType());
    if(mtype != "library")
      continue;

    if(!any_libraries)
      ss << "%0a----%0a";
    any_libraries = true;
    
    string modname = manifest.getModuleName();
    modname = findReplace(modname, " ", "");
    string modnamex = findReplace(modname, "_", "");
    modname = "[[Manifest." + modnamex + " | " + modname + "]]";
    
    string thumb = manifest.getThumb();
    if(!strEnds(thumb, "."))
      thumb += ".";
    ss << "* " << "%color=#0033bb%'''" << modname << ":'''%%";
    ss << " " << thumb << "%0a%0a";
  }

  ss << "%0a----%0a";
  string loc_str = uintToCommaString(mset.getAllLinesOfCode());
  if(loc_str != "0") 
    ss << "!!!!Total Lines of Code: " << loc_str << endl;
  string foc_str = uintToCommaString(mset.getAllFilesOfCode());
  if(foc_str != "0") 
    ss << "!!!!Total Files of Code: " << foc_str << endl;
  string wyr_str = doubleToString(mset.getAllWorkYears(),2);
  if(wyr_str != "0") 
    ss << "!!!!Total Work Years: " << wyr_str << endl;

    

  
  // Part 2: Convert the stringstream into a single string in raw format.
  string human_text = ss.str();
  string wiki_text = findReplace(human_text, "\n", "%0a%0a");

  FILE* f = fopen(filename.c_str(), "w");
  if(f) {
    fprintf(f, "version=pmwiki-2.2.69 ordered=1 urlencoded=1\n");
    fprintf(f, "text=%s\n", wiki_text.c_str());
    fclose(f);
  }

  return(true);
}

//----------------------------------------------------------
// Procedure: applyLinks()

string ManifestHandler::applyLinks(string module_name) 
{
  unsigned int mset_size = m_manifest_set.size();
  for(unsigned int i=0; i<mset_size; i++) {
    Manifest manifest = m_manifest_set.getManifestByIndex(i);
    string module_name_i = manifest.getModuleName();
    if(module_name_i == module_name) {
      string namex = findReplace(module_name, " ", "");
      namex = findReplace(namex, "_", "");
      namex = findReplace(namex, ".", "p");
      
      string module_link = "[[Manifest." + namex + " | " + module_name + "]]";
      module_name = findReplace(module_name, module_name_i, module_link);
    }
  }
  return(module_name);
}


//----------------------------------------------------------------- 
// Procedure: whiteSpace()

string ManifestHandler::whiteSpace(unsigned int pixels, string wiki_base)
{
  stringstream ss;

  if(pixels > 0) {
    ss << "%0a%25height=" << pixels;
    ss << "px%25Path:/" << wiki_base << "/images/white.png%0a";
  }
  return(ss.str());
}

