/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: MIT, Cambridge MA                                    */
/*    FILE: VelocityFilter.h                                     */
/*    DATE: Apr 23rd, 2022                                       */
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

#ifndef VELOCITY_FILTER_HEADER
#define VELOCITY_FILTER_HEADER

#include <string>

class VelocityFilter
{
 public:
  VelocityFilter();
  ~VelocityFilter() {}; 

 public: // Setters
  void   setMinSpd(double v)    {m_min_spd=v;}
  void   setMaxSpd(double v)    {m_max_spd=v;}
  void   setMinSpdPct(double v) {m_min_spd_pct=v;}

  void   setSpdOS(double v) {m_osv=v;}
  void   setSpdCN(double v) {m_cnv=v;}
  
 public: // Getters
  double getMinSpd()    const {return(m_min_spd);}
  double getMaxSpd()    const {return(m_max_spd);}
  double getMinSpdPct() const {return(m_min_spd_pct);}
  double getSpdOS()     const {return(m_osv);}
  double getSpdCN()     const {return(m_cnv);}

  double getFilterPct() const;
  
  std::string getSpec() const {return("");}

  bool valid() const;
  
 private: // config vars
  double   m_min_spd;       // meters/sec
  double   m_max_spd;       // meters/sec
  double   m_min_spd_pct;   // range [0,100]

 private: // state vars
  double   m_osv;           // meters/sec
  double   m_cnv;           // meters/sec
};

VelocityFilter stringToVelocityFilter(std::string);

#endif 
