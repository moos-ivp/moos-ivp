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
 
#ifndef MANIFEST_HEADER
#define MANIFEST_HEADER

#include <string>
#include <vector>

class Manifest {
public:
  Manifest();
  virtual ~Manifest() {};

  // Setters
  void setModuleName(std::string s)      {m_module_name=s;};
  void setDocURL(std::string s)          {m_doc_url=s;};
  void setType(std::string s)            {m_type=s;};
  void setThumb(std::string s)           {m_thumb=s;};
  void setDistro(std::string s)          {m_distro=s;};
  void setDownload(std::string s)        {m_download=s;};
  void setContactEmail(std::string s)    {m_contact_email=s;};
  void setLicense(std::string s)         {m_license=s;};
  bool setDate(std::string s);
  void setLinesOfCode(unsigned int v)    {m_lines_of_code=v;}
  void setFilesOfCode(unsigned int v)    {m_files_of_code=v;}
  void setLinesOfCodeTot(unsigned int v) {m_lines_of_code_total=v;}
  void setFilesOfCodeTot(unsigned int v) {m_files_of_code_total=v;}
  void setWorkYears(double v)            {m_work_years=v;}
  void setWorkYearsTot(double v)         {m_work_years_total=v;}
  
  
  void addSynopsisLine(std::string s)   {m_synopsis.push_back(s);};
  void addModDate(std::string s)        {m_mod_dates.push_back(s);};
  void addLibDependency(std::string);
  void addAppDepender(std::string);
  void addAuthor(std::string);
  void addOrg(std::string);
  void addGroup(std::string);          

  // Getters
  std::string getModuleName()   const {return(m_module_name);};
  std::string getType()         const {return(m_type);};
  std::string getThumb()        const {return(m_thumb);};
  std::string getDocURL()       const {return(m_doc_url);};
  std::string getDistro()       const {return(m_distro);};
  std::string getDownload()     const {return(m_download);};
  std::string getDate()         const {return(m_date);};
  std::string getContactEmail() const {return(m_contact_email);};
  std::string getLicense()      const {return(m_license);};
  unsigned int getLinesOfCode() const {return(m_lines_of_code);}
  unsigned int getFilesOfCode() const {return(m_files_of_code);}
  unsigned int getLinesOfCodeTot() const {return(m_lines_of_code_total);}
  unsigned int getFilesOfCodeTot() const {return(m_files_of_code_total);}
  double getWorkYears()         const {return(m_work_years);}
  double getWorkYearsTot()      const {return(m_work_years_total);}

  
  std::vector<std::string> getLibDepends() const {return(m_lib_dependencies);};
  std::vector<std::string> getAppDepends() const {return(m_app_dependers);};
  std::vector<std::string> getAuthors()    const {return(m_authors);};
  std::vector<std::string> getOrgs()       const {return(m_organizations);};
  std::vector<std::string> getGroups()     const {return(m_groups);};
  std::vector<std::string> getSynopsis()   const {return(m_synopsis);};
  std::vector<std::string> getModDates()   const {return(m_mod_dates);};

  std::string getSynopsisStr() const;
  unsigned int getNumDate() const {return(m_num_date);}
  
  // Overload Operators
  bool operator< (const Manifest&) const;
  
  // Analysis/Utilties
  bool hasKey(std::string) const;
  bool hasAuthor(std::string) const;
  bool hasDependency(std::string) const;
  bool hasOrg(std::string) const;
  bool hasGroup(std::string) const;

  unsigned int missingMandatoryCnt() const;
  unsigned int missingOptionalCnt() const;

  std::vector<std::string> missingMandatory() const;
  std::vector<std::string> missingOptional() const;

  bool valid() const;
  void print() const;
  void printTerse() const;

 protected:
  std::string  m_module_name;
  std::string  m_doc_url;
  std::string  m_type;
  std::string  m_thumb;
  std::string  m_distro;
  std::string  m_download;
  std::string  m_date;
  std::string  m_contact_email;
  std::string  m_license;

  std::vector<std::string>  m_lib_dependencies;
  std::vector<std::string>  m_app_dependers;
  std::vector<std::string>  m_authors;
  std::vector<std::string>  m_organizations;
  std::vector<std::string>  m_groups;
  std::vector<std::string>  m_synopsis;
  std::vector<std::string>  m_mod_dates;

  unsigned int m_num_date;

  unsigned int m_lines_of_code;
  unsigned int m_files_of_code;
  unsigned int m_lines_of_code_total;
  unsigned int m_files_of_code_total;
  double m_work_years;
  double m_work_years_total;

};

#endif

