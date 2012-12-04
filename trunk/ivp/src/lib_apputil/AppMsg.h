/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AppMsg.h                                             */
/*    DATE: Jun 14th 2012                                        */
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

#ifndef APP_MSG_HEADER
#define APP_MSG_HEADER

#include <string>

// MESSAGE DATA TYPES
#define MSG_DOUBLE        'D'
#define MSG_STRING        'S'
#define MSG_BINARY_STRING 'B'

class AppMsg  
{
public:
  AppMsg();
  AppMsg(const std::string& sKey, double dfVal, double dfTime);
  AppMsg(const std::string& sKey, const std::string& sVal, double dfTime);

  virtual ~AppMsg() {};
  
 public: // Setters
  void SetKey(const std::string& str)        {m_sKey = str;}; 
  void SetString(const std::string& str)     {m_sVal = str;}; 
  void SetDouble(double val)                 {m_dfVal = val;}; 
  void SetTime(double val)                   {m_dfTime = val;}; 
  void SetSource(const std::string& str)     {m_sSrc = str;};
  void SetSourceAux(const std::string& str)  {m_sSrcAux = str;}
  void SetCommunity(const std::string& str)  {m_sOriginatingCommunity = str;};
  void MarkAsBinary()                        {m_cDataType = MSG_BINARY_STRING;};

 public: // Getters
  bool   IsDouble() const          {return(m_cDataType == MSG_DOUBLE);};
  bool   IsString() const          {return(m_cDataType == MSG_STRING);};
  bool   IsBinary() const          {return(m_cDataType == MSG_BINARY_STRING);};
  double GetTime() const           {return(m_dfTime);};
  double GetDouble() const         {return(m_dfVal);};

  std::string GetKey() const       {return m_sKey;};
  std::string GetString() const    {return m_sVal;};
  std::string GetSource() const    {return m_sSrc;};
  std::string GetSourceAux() const {return m_sSrcAux;};
  std::string GetCommunity() const {return m_sOriginatingCommunity;};

 private: // Member variables
  char        m_cDataType;
  double      m_dfTime;
  double      m_dfVal;
  std::string m_sKey;
  std::string m_sVal;
  std::string m_sSrc;
  std::string m_sSrcAux;
  std::string m_sOriginatingCommunity;
};

#endif 

