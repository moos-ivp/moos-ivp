/************************************************************/
/*   NAME: Michael Benjamin                                 */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA */
/*   FILE: Manifest.h                                       */
/*   DATE: Nov 30th, 2012                                   */
/************************************************************/

#include <iostream> 
#include "Manifest.h"
#include "MBUtils.h"

using namespace std;

//----------------------------------------------------------
// Procedure: print()

void Manifest::print() const
{
  cout << "AppName:  " << m_app_name  << endl;
  cout << "DocTitle: " << m_doc_title << endl;
  cout << "DocURL:   " << m_doc_url   << endl;
  cout << "Distro:   " << m_distro    << endl;
  cout << "KeyWord:  " << m_primary_keyword << endl;

  cout << "Lib Dependencies:" << endl;
  for(unsigned int i=0; i<m_lib_dependencies.size(); i++) 
    cout << "  " << m_lib_dependencies[i] << endl;

  cout << "Tree Dependencies:" << endl;
  for(unsigned int i=0; i<m_tree_dependencies.size(); i++) 
    cout << "  " << m_tree_dependencies[i] << endl;

  cout << "Authors:" << endl;
  for(unsigned int i=0; i<m_authors.size(); i++) 
    cout << "  " << m_authors[i] << endl;
  
  cout << "Keywords: " << endl;
  for(unsigned int i=0; i<m_keywords.size(); i++) 
    cout << "  " << m_keywords[i] << endl;

  cout << "Synopsis:" << endl;
  for(unsigned int i=0; i<m_synopsis.size(); i++) 
    cout << "  " << m_synopsis[i] << endl;
  
  cout << "Help:" << endl;
  for(unsigned int i=0; i<m_help.size(); i++) 
    cout << "  " << m_help[i] << endl;
  
  cout << "Config" << endl;
  for(unsigned int i=0; i<m_config.size(); i++)
    cout << "  " << m_config[i] << endl;
  
  cout << "Interface" << endl;
  for(unsigned int i=0; i<m_interface.size(); i++)
    cout << "  " << m_interface[i] << endl;

  cout << "Notes" << endl;
  for(unsigned int i=0; i<m_notes.size(); i++)
    cout << "  " << m_notes[i] << endl;    
}

//----------------------------------------------------------
// Procedure: valid()

bool Manifest::valid() const
{
  return(missingMandatoryCnt() == 0);
}

//----------------------------------------------------------
// Procedure: missingMandatoryCnt()

unsigned int Manifest::missingMandatoryCnt() const
{
  vector<string> missing_mandatory = missingMandatory();
  return(missing_mandatory.size());
}


//----------------------------------------------------------
// Procedure: missingMandatory()
//      Note: Mandatory fields:
//            1. App Name
//            2. Distro (where to find the app)
//            3. Primary keyword
//            4. Library dependencies (or "None")
//            5. Synopsis
//            6. Command-line help
//            7. Configuration example
//            8. Interface description
//            9. One or more authors

vector<string> Manifest::missingMandatory() const
{
  vector<string> rvector;

  if(m_app_name == "")            
    rvector.push_back("app_name");
  if(m_distro == "")                 
    rvector.push_back("distro");
  if(m_primary_keyword == "")        
    rvector.push_back("primary_keyword");
  
  if(m_lib_dependencies.size() == 0) 
    rvector.push_back("lib_dependencies");
  if(m_synopsis.size() == 0)  
    rvector.push_back("synopsis");
  if(m_help.size() == 0)      
    rvector.push_back("help");
  if(m_config.size() == 0)    
    rvector.push_back("config");
  if(m_interface.size() == 0) 
    rvector.push_back("interface");

  if(m_authors.size() == 0)   
    rvector.push_back("authors");
  
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
//            1. doc_title
//            2. doc_url
//            3. date
//            4. contact_email
//            5. tree_dependencies
//            6. keywords
//            7. revision_dates
//            8. notes

vector<string> Manifest::missingOptional() const
{
  vector<string> rvector;

  if(m_doc_title == "")               
    rvector.push_back("doc_title");
  if(m_doc_url == "")                 
    rvector.push_back("doc_url");
  if(m_date == "")   
    rvector.push_back("date");
  if(m_contact_email == "")   
    rvector.push_back("contact_email");
  
  if(m_tree_dependencies.size() == 0) 
    rvector.push_back("tree_dependencies");
  if(m_keywords.size() == 0)       
    rvector.push_back("keywords");
  if(m_revision_dates.size() == 0) 
    rvector.push_back("revision_dates");
  if(m_notes.size() == 0)
    rvector.push_back("notes");

  return(rvector);
}
