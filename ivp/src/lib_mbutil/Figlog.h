/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Figlog.h                                             */
/*    DATE: Dec 16th 2010                                        */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef FIGLOG_HEADER
#define FIGLOG_HEADER

#include <string>
#include <vector>

class Figlog
{
public:
  Figlog() {}
  ~Figlog() {}

  // Setters
  void addMessage(std::string s) {m_messages.push_back(s);}
  void addWarning(std::string s) {m_warnings.push_back(s);}
  void addError(std::string s)   {m_errors.push_back(s);}

  void augMessage(std::string);
  void augWarning(std::string);
  void augError(std::string);

  void setLabel(std::string s) {m_label=s;}

  // Getters
  unsigned int messages() const  {return(m_messages.size());}
  unsigned int warnings() const  {return(m_warnings.size());}
  unsigned int errors() const    {return(m_errors.size());}
  
  std::vector<std::string> getMessages() const {return(m_messages);}
  std::vector<std::string> getWarnings() const {return(m_warnings);}
  std::vector<std::string> getErrors()   const {return(m_errors);}

  std::string getMessage(unsigned int);
  std::string getWarning(unsigned int);
  std::string getError(unsigned int);

  std::string getLabel() {return(m_label);}

  // Actions
  void clear();
  void clearMessages()  {m_messages.clear();}
  void clearWarnings()  {m_warnings.clear();}
  void clearErrors()    {m_errors.clear();}
  void print() const;


 protected:
  std::vector<std::string> m_messages;
  std::vector<std::string> m_warnings;
  std::vector<std::string> m_errors;

  std::string m_label;
};

#endif 









