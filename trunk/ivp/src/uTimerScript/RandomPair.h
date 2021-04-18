/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: RandomPair.h                                         */
/*    DATE: Nov 22nd 2016                                        */
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

#ifndef RANDOM_PAIR_HEADER
#define RANDOM_PAIR_HEADER

#include <string>

class RandomPair 
{
 public:
  RandomPair();
  virtual ~RandomPair() {}

  virtual bool setParam(std::string, double);
  virtual bool setParam(std::string, std::string);
  virtual void reset() {}
  
  virtual std::string getStringSummary() const;
  virtual std::string getParams() const {return("");}
  
 public:
  void   setVarName1(std::string str)  {m_varname1=str;} 
  void   setVarName2(std::string str)  {m_varname2=str;} 
  void   setKeyName(std::string str)   {m_keyname=str;}
  void   setType(std::string str)      {m_type=str;}

  std::string getVarName1() const      {return(m_varname1);}
  std::string getVarName2() const      {return(m_varname2);}
  std::string getKeyName() const       {return(m_keyname);}
  std::string getType() const          {return(m_type);}
  
  double      getValue1() const        {return(m_value1);}
  double      getValue2() const        {return(m_value2);}

  std::string getStringValue1() const;
  std::string getStringValue2() const;

 protected: // Configuration Parameters
  std::string m_varname1;
  std::string m_varname2;
  std::string m_keyname;
  std::string m_type;

 protected: // State Variables
  double      m_value1;
  double      m_value2;

  std::string m_value_str1;
  std::string m_value_str2;

};

#endif 









