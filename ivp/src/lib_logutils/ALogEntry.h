/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ALogEntry.h                                          */
/*    DATE: Aug 12th, 2009                                       */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef ALOG_ENTRY_HEADER
#define ALOG_ENTRY_HEADER

#include <string>

class ALogEntry
{
public:
  ALogEntry() {m_timestamp=0; m_dval=0; m_isnum=false;};
  ~ALogEntry() {};

  void set(double timestamp, const std::string& varname, 
	   const std::string& source, const std::string& sval)
  {
    m_timestamp = timestamp;
    m_varname   = varname;
    m_source    = source;
    m_sval      = sval;
    m_dval      = 0;
    m_isnum     = false;
  };
    
  void set(double timestamp, const std::string& varname, 
	   const std::string& source, double dval)
  {
    m_timestamp = timestamp;
    m_varname   = varname;
    m_source    = source;
    m_sval      = "";
    m_dval      = dval;
    m_isnum     = true;
  };
    
  void setStatus(const std::string& s) {m_status=s;};

  double      getTimeStamp() const {return(m_timestamp);};
  std::string getVarName() const   {return(m_varname);};
  std::string getSource() const    {return(m_source);};
  std::string getStringVal() const {return(m_sval);};
  double      getDoubleVal() const {return(m_dval);};
  bool        isNumerical() const  {return(m_isnum);};
  std::string getStatus() const    {return(m_status);};

  void        skewBackward(double v) {m_timestamp -= v;};
  void        skewForward(double v)  {m_timestamp += v;};

protected:
  double      m_timestamp;
  std::string m_varname;
  std::string m_source;
  std::string m_sval;
  double      m_dval;
  bool        m_isnum;

  // An optional status string. The empty string indicates the entry
  // is a normal entry. "invalid" means the entry is not normal. "eof"
  // could indicate that a the entry is the tail of normal entries.
  std::string  m_status; 
};
#endif 








