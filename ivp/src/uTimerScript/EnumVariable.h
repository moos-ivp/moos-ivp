/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: EnumVariable.h                                       */
/*    DATE: Dec 26th 2009                                        */
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

#ifndef RANDOM_VARIABLE_HEADER
#define RANDOM_VARIABLE_HEADER

#include <vector>
#include <string>

class EnumVariable 
{
 public:
  EnumVariable();
  virtual ~EnumVariable() {}

 public:
  void        setVarName(std::string str)   {m_varname=str;} 
  void        setKeyName(std::string str)   {m_keyname=str;}
  void        addElement(std::string, double weight=1);
  std::string reset(std::string key, double timestamp=-1);
  double      getAge(double timestamp);

  std::string getVarName() const {return(m_varname);}
  std::string getKeyName() const {return(m_keyname);}
  std::string getValue() const;

  std::string getStringSummary();

 protected: // Configuration Parameters
  std::string m_varname;
  std::string m_keyname;
  std::vector<std::string> m_elements;
  std::vector<double>      m_weights;

 protected: // State Variables
  double         m_timestamp;
  unsigned int   m_index;
  double         m_total_weight;
};

#endif 










