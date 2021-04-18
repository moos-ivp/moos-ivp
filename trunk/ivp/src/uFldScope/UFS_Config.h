/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: UFS_Config.h                                         */
/*    DATE: Nov 23rd 2011                                        */
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

#ifndef UFS_CONFIG_HEADER
#define UFS_CONFIG_HEADER

#include <string>

class UFS_Config
{
 public:
  UFS_Config(const std::string& s="");
  virtual ~UFS_Config() {}

  // Setters
  void init(const std::string&);

  // Analysis
  bool valid() const;

  // Getters
  std::string getMOOSVar() const   {return(m_var);}
  std::string getKey() const       {return(m_key);}
  std::string getField() const     {return(m_fld);}
  std::string getAlias() const     {return(m_alias);}
  std::string getColor() const     {return(m_color);}
  std::string getXColor() const    {return(m_xcolor);}
  double      getXThresh() const   {return(m_xthresh);}

 protected:
  std::string m_var;      // MOOS Variable   
  std::string m_key;      // Vehicle name key (default = "name")
  std::string m_fld;      // Field to find and report
  std::string m_alias;    // Alias for the field used in report
  std::string m_color;    // Color reported under normal situations
  std::string m_xcolor;   // Color reported when new info is late
  double      m_xthresh;  // Time after which report is considered late.
};

#endif 








