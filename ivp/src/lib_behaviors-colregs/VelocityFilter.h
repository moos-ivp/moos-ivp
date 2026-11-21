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
  bool   setMinSpd(double v);
  bool   setMaxSpd(double v);
  bool   setMaxDiscount(double v);

  void   setEnabled(bool v) {m_enabled=v;}
  bool   setSpdCN(double v);
  
 public: // Getters
  double getMinSpd() const      {return(m_min_spd);}
  double getMaxSpd() const      {return(m_max_spd);}
  double getMaxDiscount() const {return(m_max_discount);}
  double getSpdCN() const       {return(m_cnv);}
  bool   isSpdRegulated() const {return(m_enabled && valid());}

  double spdRegulate(double) const;
  
  std::string getSpec() const;

  bool valid() const;
  
 private: // config vars
  double   m_min_spd;       // meters/sec
  double   m_max_spd;       // meters/sec
  double   m_max_discount;  // [0,100]
  bool     m_enabled;   
  
 private: // state vars
  double   m_cnv;           // meters/sec
};

VelocityFilter stringToVelocityFilter(std::string);

#endif 
