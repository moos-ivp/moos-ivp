/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: NodeRider.h                                          */
/*    DATE: May 7th 2022                                         */
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

#ifndef NODE_RIDER_HEADER
#define NODE_RIDER_HEADER

#include <string>

class NodeRider
{
public:
  NodeRider();
  ~NodeRider() {};

  bool setVar(std::string);
  bool setPolicyConfig(std::string);
  bool setRiderFld(std::string);

  void setFresh(bool v=true)    {m_fresh=v;}
  void setLastUTC(double utc)   {m_last_utc=utc;}       

  bool updateValue(std::string, double utc);
  
  std::string getVar() const      {return(m_moosvar);}
  std::string getPolicy() const   {return(m_policy);}
  std::string getRiderFld() const {return(m_rider_fld);}
  double getFrequency() const     {return(m_frequency);}

  bool   isFresh() const         {return(m_fresh);}
  double getLastUTC() const      {return(m_last_utc);}

  std::string getCurrVal() const {return(m_curr_value);}
  std::string getSpec() const;

  bool valid() const;
  
 protected:  
  bool setPolicy(std::string);
  bool setFrequency(double);
    
 protected: // config vars
  std::string  m_moosvar;
  std::string  m_policy;
  double       m_frequency;
  std::string  m_rider_fld;

 protected: // state vars
  std::string  m_curr_value;
  double       m_last_utc;
  bool         m_fresh;
  unsigned int m_total_updates;
  
};

#endif 

