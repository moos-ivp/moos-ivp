/************************************************************/
/*   NAME: Michael Benjamin                                 */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA */
/*   FILE: Manifest.h                                       */
/*   DATE: Nov 30th, 2012                                   */
/************************************************************/
 
#ifndef MANIFEST_HEADER
#define MANIFEST_HEADER

#include <string>
#include <vector>

class Manifest {
public:
  Manifest() {};
  virtual ~Manifest() {};

  void setAppName(std::string s)        {m_app_name=s;};
  void setDocTitle(std::string s)       {m_doc_title=s;};
  void setDocURL(std::string s)         {m_doc_url=s;};
  void setDistro(std::string s)         {m_distro=s;};
  void setPrimaryKeyword(std::string s) {m_primary_keyword=s;};
  void setDate(std::string s)           {m_date=s;};
  void setContactEmail(std::string s)   {m_contact_email=s;};

  void addLibDependency(std::string s)  {m_lib_dependencies.push_back(s);};
  void addTreeDependency(std::string s) {m_tree_dependencies.push_back(s);};
  void addSynopsisLine(std::string s)   {m_synopsis.push_back(s);};
  void addHelpLine(std::string s)       {m_help.push_back(s);};
  void addConfigLine(std::string s)     {m_config.push_back(s);};
  void addInterfaceLine(std::string s)  {m_interface.push_back(s);};
  void addAuthor(std::string s)         {m_authors.push_back(s);};
  void addKeyWord(std::string s)        {m_keywords.push_back(s);};
  void addRevisionDate(std::string s)   {m_revision_dates.push_back(s);};
  void addNote(std::string s)           {m_notes.push_back(s);};

  std::string getAppName()        const {return(m_app_name);};
  std::string getDocTitle()       const {return(m_doc_title);};
  std::string getDocURL()         const {return(m_doc_url);};
  std::string getDistro()         const {return(m_distro);};
  std::string getPrimaryKeyword() const {return(m_primary_keyword);};
  std::string getDate()           const {return(m_date);};
  
  std::vector<std::string> getLibDependencies()  const {return(m_lib_dependencies);};
  std::vector<std::string> getTreeDependencies() const {return(m_tree_dependencies);};
  std::vector<std::string> getSynopsis()         const {return(m_synopsis);};
  std::vector<std::string> getHelp()             const {return(m_help);};
  std::vector<std::string> getConfig()           const {return(m_config);};
  std::vector<std::string> getInterface()        const {return(m_interface);};
  std::vector<std::string> getNotes()            const {return(m_notes);};
  std::vector<std::string> getAuthors()          const {return(m_authors);};
  std::vector<std::string> getKeywords()         const {return(m_keywords);};
  std::vector<std::string> getRevisionDates()    const {return(m_revision_dates);};

  std::string getSynopsisStr() const;

  unsigned int missingMandatoryCnt() const;
  unsigned int missingOptionalCnt() const;

  std::vector<std::string> missingMandatory() const;
  std::vector<std::string> missingOptional() const;

  bool valid() const;

  void print() const;

 protected:
  std::string  m_app_name;
  std::string  m_doc_title;
  std::string  m_doc_url;
  std::string  m_distro;
  std::string  m_primary_keyword;
  std::string  m_date;
  std::string  m_contact_email;

  std::vector<std::string>  m_lib_dependencies;
  std::vector<std::string>  m_tree_dependencies;
  std::vector<std::string>  m_authors;
  std::vector<std::string>  m_keywords;
  std::vector<std::string>  m_synopsis;
  std::vector<std::string>  m_help;
  std::vector<std::string>  m_config;
  std::vector<std::string>  m_interface;
  std::vector<std::string>  m_revision_dates;
  std::vector<std::string>  m_notes;
};

#endif
