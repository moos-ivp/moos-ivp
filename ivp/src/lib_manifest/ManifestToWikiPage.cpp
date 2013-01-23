/************************************************************/
/*   NAME: Michael Benjamin                                 */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA */
/*   FILE: ManifestToWikiPage.cpp                           */
/*   DATE: Dec 17th, 2012                                   */
/************************************************************/

#include <iostream>
#include <sstream>
#include "Manifest.h"
#include "ManifestToWikiPage.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: generate

bool ManifestToWikiPage::generate_wikilines() 
{
  // Part 1: Generate the wiki lines in human readable format.
  //         Everything stored in a stringstream.

  stringstream ss;
  ss << "!!Application: %color=#ff7f00%'''";
  ss << m_manifest.getAppName();
  ss << "'''%%" << endl;

  ss << "[[<<]]" << endl;
  ss << "!!!!Function: " << endl;

  vector<string> synopsis = m_manifest.getSynopsis();
  for(unsigned int i=0; i<synopsis.size(); i++)
    synopsis[i] = stripBlankEnds(synopsis[i]);

  for(unsigned int i=0; i<synopsis.size(); i++) {
    if(synopsis[i] == "")
      ss << endl;
    else
      ss << synopsis[i];
  }
  ss << endl;
  ss << endl;

  ss << "!!!!Authors:" << endl;
  vector<string> authors = m_manifest.getAuthors();
  for(unsigned int i=0; i<authors.size(); i++) 
    ss << "* " << authors[i] << endl;
  ss << endl;

  ss << "!!!!Dependencies:" << endl;
  ss << "* ";
  vector<string> libdeps = m_manifest.getLibDependencies();
  for(unsigned int i=0; i<libdeps.size(); i++) {
    ss << libdeps[i];
    if(i < (libdeps.size()-1))
      ss << ", ";
    else
      ss << endl << endl;
  }
  
  ss << "!!!!Distribution:" << endl;
  ss << "* (:include MOOSIvPOrg :)" << endl << endl;

  ss << "!!!!Documentation:" << endl;
  ss << "* (:include Reference.ToolsTR :)" << endl << endl;

  ss << "!!!!Keywords:" << endl;
  ss << "* " << m_manifest.getPrimaryKeyword() << endl;
  vector<string> kwords = m_manifest.getKeywords();
  for(unsigned int i=0; i<kwords.size(); i++) 
    ss << "* " << kwords[i] << endl;
  ss << endl;
    
  ss << "(:include IvP.ModulesShort:)" << endl;

  // Part 2: Convert the stringstream into a single string in raw format.

  string human_text = ss.str();
  string wiki_text = findReplace(human_text, "\n", "%0a%0a");


  // Part 3: Dump the target file contents
  cout << "version=pmwiki-2.2.14 ordered=1 urlencoded=1" << endl;
  
  cout << "text=" << wiki_text << endl;

  return(true);
}

