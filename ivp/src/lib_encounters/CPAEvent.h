/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CPAEvent.h                                           */
/*    DATE: Dec 21st 2015                                        */
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

#ifndef CPA_EVENT_HEADER
#define CPA_EVENT_HEADER

#include <string>

class CPAEvent
{
 public:
  CPAEvent();
  CPAEvent(std::string v1, std::string v2, double cpa=0);
  CPAEvent(std::string spec);
  ~CPAEvent() {};

  void setVName1(std::string s) {m_vname1=s;}
  void setVName2(std::string s) {m_vname2=s;}
  void setCPA(double v)         {m_cpa=v;}
  void setEFF(double v)         {m_eff=v;}
  void setX(double v)           {m_x=v;}
  void setY(double v)           {m_y=v;}
  void setID(int id)            {m_id=id;}
  void setAlpha(double v)       {m_alpha=v;}
  void setBeta(double v)        {m_beta=v;}
  void setPType(std::string s)  {m_ptype=s;}
  void setXType(std::string s)  {m_xtype=s;}
  
  std::string getVName1() const {return(m_vname1);}
  std::string getVName2() const {return(m_vname2);}
  double      getCPA() const    {return(m_cpa);}
  double      getEFF() const    {return(m_eff);}
  double      getX() const      {return(m_x);}
  double      getY() const      {return(m_y);}
  int         getID() const     {return(m_id);}
  double      getAlpha() const  {return(m_alpha);}
  double      getBeta() const   {return(m_beta);}
  std::string getPType() const  {return(m_ptype);}
  std::string getXType() const  {return(m_xtype);}
  
  std::string getSpec() const;
  
 protected: 
  std::string  m_vname1;  // The two vehicles involved
  std::string  m_vname2; 
  double       m_cpa;     // The core info: cpa distance
  double       m_eff;     // Efficiency perhaps filled later
  double       m_x;       // (x,y) midpt at time of cpa
  double       m_y;
  int          m_id;

  double       m_alpha;   // relbng of v1 to v2
  double       m_beta;    // relbng of v2 to v1
  std::string  m_ptype;   // passing type, e.g. port:port
  std::string  m_xtype;   // crossing type, e.g. aft:fore
};

#endif 





