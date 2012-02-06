/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CBA_Model.h                                          */
/*    DATE: Aug 17th, 2011 (Waterloo)                            */
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

#ifndef CBBA_MODEL_HEADER
#define CBBA_MODEL_HEADER

#include <vector>
#include <string>
#include "Bundle.h"
#include "Path.h"
#include "Figlog.h"
#include "VarDataPair.h"

class CBA_Model
{
 public:
  CBA_Model();
  virtual ~CBA_Model() {};

  bool  setParam(std::string param, std::string value, unsigned int=0);
  bool  handleMsg(std::string key, double dval, std::string sval, 
		  double mtime, bool isdouble, bool isstring, 
		  std::string source);
  void  setCurrTime(double);
  bool  verbose() {return(m_verbose);};
  void  iterate();
  void  print() const;


  void addMessage(const std::string& varname, const std::string& value);
  void addMessage(const std::string& varname, double value);

  std::vector<VarDataPair> getMessages(bool clear=true);

  Figlog getFigLog() {return(m_figlog);};

 protected: 

  std::vector<VarDataPair> m_messages;

  unsigned int  m_iterations;
  unsigned int  m_max_tasks;

  double   m_curr_time;

  bool     m_verbose;
  Bundle   m_bundle;
  Path     m_path;

  Figlog   m_figlog;
};

#endif 

