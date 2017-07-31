/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: RandomVariable.h                                     */
/*    DATE: Dec 18th 2009                                        */
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

#include <string>

class RandomVariable 
{
 public:
  RandomVariable();
  virtual ~RandomVariable() {}

  virtual bool setParam(std::string, double);
  virtual void reset() {}
  
  virtual std::string getStringSummary() const;
  virtual std::string getParams() const {return("");}
  
 public:
  void   setVarName(std::string str)  {m_varname=str;} 
  void   setKeyName(std::string str)  {m_keyname=str;}
  void   setType(std::string str)     {m_type=str;}

  std::string getVarName() const      {return(m_varname);}
  std::string getKeyName() const      {return(m_keyname);}
  std::string getType() const         {return(m_type);}
  
  double      getMinVal() const       {return(m_min_val);}
  double      getMaxVal() const       {return(m_max_val);}
  double      getValue() const        {return(m_value);}

  std::string getStringValue() const;

 protected: // Configuration Parameters
  std::string m_varname;
  std::string m_keyname;
  std::string m_type;

  double      m_min_val;
  double      m_max_val;

 protected: // State Variables
  double      m_value;
  std::string m_value_str;

};

#endif 









