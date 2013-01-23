/************************************************************/
/*   NAME: Michael Benjamin                                 */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA */
/*   FILE: Populator_Manifest.cpp                           */
/*   DATE: Dec 14th, 2012 (Sandy Hook)                      */
/************************************************************/

#include <iostream>
#include "FileBuffer.h" 
#include "Manifest.h"
#include "Populator_Manifest.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: populate(string)

bool Populator_Manifest::populate(string filename) 
{
  vector<string> lines = fileBuffer(filename);

  string mode = "top";

  bool all_ok = true;
  
  unsigned int i, vsize = lines.size();

  if(vsize == 0)
    all_ok = false;

  for(i=0; i<vsize; i++) {
    string orig = lines[i];
    string line = stripBlankEnds(lines[i]);
    string lline = tolower(line);

    if(lline == "end_help")
      mode = "top";
    else if(lline == "end_config")
      mode = "top";
    else if(lline == "end_interface")
      mode = "top";
    else if(lline == "begin_help")
      mode = "help";
    else if(lline == "begin_synopsis")
      mode = "synopsis";
    else if(lline == "begin_config")
      mode = "config";
    else if(lline == "begin_interface")
      mode = "interface";
    else {  // Not a mode switch line
      if(mode == "help") 
	m_manifest.addHelpLine(orig);
      else if(mode == "synopsis") 
	m_manifest.addSynopsisLine(orig);
      else if(mode == "config") 
	m_manifest.addConfigLine(orig);
      else if(mode == "interface") 
	m_manifest.addInterfaceLine(orig);
      else { // Not one of the verbatim modes
	string param = tolower(biteStringX(line, '='));
	string value = line;

	if(param == "app_name")
	  m_manifest.setAppName(value);
	else if(param == "doc_title")
	  m_manifest.setDocTitle(value);
	else if(param == "doc_url")
	  m_manifest.setDocURL(value);
	else if(param == "distro")
	  m_manifest.setDistro(value);
	else if(param == "primary_keyword")
	  m_manifest.setPrimaryKeyword(value);
	else if(param == "date")
	  m_manifest.setDate(value);
	else if(param == "lib_dependencies")
	  m_manifest.addLibDependency(value);
	else if(param == "tree_dependencies")
	  m_manifest.addTreeDependency(value);
	else if(param == "authors")
	  m_manifest.addAuthor(value);
	else if(param == "keywords")
	  m_manifest.addKeyWord(value);
	else if(param == "revision_dates")
	  m_manifest.addRevisionDate(value);
	else if(param == "note")
	  m_manifest.addNote(value);
	else if ((!strBegins(line, "//")) && (line != "")) {
	  cout << "Unhandled line [" << i+1 << "]: " << orig << endl;
	  all_ok = false;
	}
      }
    }
  }

  return(all_ok);
}

