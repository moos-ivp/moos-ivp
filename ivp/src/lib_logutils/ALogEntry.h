/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ALogEntry.h                                          */
/*    DATE: Aug 12th, 2009                                       */
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

#ifndef ALOG_ENTRY_HEADER
#define ALOG_ENTRY_HEADER

#include <string>

class ALogEntry
{
public:
  ALogEntry() {m_timestamp=0; m_dval=0; m_isnum=false;}
  ~ALogEntry() {}

  // Setters / Modifiers
  void set(double timestamp, const std::string& varname, 
	   const std::string& source, 
	   const std::string& srcaux,
	   const std::string& sval);

  void set(double timestamp, const std::string& varname, 
	   const std::string& source, 
	   const std::string& srcaux,
	   double dval);

  void setTimeStamp(double v)           {m_timestamp = v;}
  void setDVal(double v)                {m_dval = v;}
  void setIsNum()                       {m_isnum = true;}
  void setVarName(const std::string& s) {m_varname = s;}
  void setSource(const std::string& s)  {m_source = s;}
  void setSrcAux(const std::string& s)  {m_srcaux = s;}
  void setNode(const std::string& s)    {m_node = s;}
  void setStatus(const std::string& s)  {m_status = s;}
  void setRawLine(const std::string& s) {m_raw_line = s;}
  void skewBackward(double v)           {m_timestamp -= v;}
  void skewForward(double v)            {m_timestamp += v;}


  // Getters / Analyzers
  double      time() const         {return(m_timestamp);}
  double      getTimeStamp() const {return(m_timestamp);}
  std::string getVarName() const   {return(m_varname);}
  std::string getSource() const    {return(m_source);}
  std::string getSrcAux() const    {return(m_srcaux);}
  std::string getNode() const      {return(m_node);}
  std::string getStringVal() const {return(m_sval);}
  double      getDoubleVal() const {return(m_dval);}
  bool        isNumerical() const  {return(m_isnum);}
  std::string getRawLine() const   {return(m_raw_line);}
  std::string getStatus() const    {return(m_status);}
  bool        isNull() const       {return(m_status=="null");}

  std::string getSummary() const;
  
  bool        tokenField(const std::string& field, double& value) const;


protected:
  double      m_timestamp;
  std::string m_varname;
  std::string m_source;
  std::string m_srcaux;
  std::string m_node;
  std::string m_sval;
  double      m_dval;
  bool        m_isnum;
  std::string m_raw_line;

  // An optional status string. The empty string indicates the entry
  // is a normal entry. "invalid" means the entry is not normal. "eof"
  // could indicate that a the entry is the tail of normal entries.
  std::string  m_status; 
};

bool operator< (const ALogEntry& one, const ALogEntry& two);
bool operator== (const ALogEntry& one, const ALogEntry& two);
bool operator!= (const ALogEntry& one, const ALogEntry& two);

#endif 









