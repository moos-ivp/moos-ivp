/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: AppGenerator.h                                       */
/*    DATE: Mar 23rd, 2021                                       */
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

#ifndef MOOS_APP_GENERATOR_HEADER
#define MOOS_APP_GENERATOR_HEADER

#include <vector>
#include <string>
#include <set>

class AppGenerator
{
 public:
  AppGenerator();
  virtual ~AppGenerator() {}

  void   setBody(std::string s="") {};
  void   setPrefix(std::string s="") {};
  void   setDate(std::string s="") {};
  void   setName(std::string n="") {};
  void   setOrg(std::string n="") {};
  
  bool   generate();

 protected: // Config variables
  bool        m_appcasting;
  std::string m_comments;

  std::string m_prefix;
  std::string m_body;
  std::string m_name;
  std::string m_date;
  std::string m_org;

};

#endif 









