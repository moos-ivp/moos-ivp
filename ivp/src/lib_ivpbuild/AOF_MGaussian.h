/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_MGaussian.h                                      */
/*    DATE: June 9th 2008                                        */
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
 
#ifndef AOF_MGAUSSIAN_HEADER
#define AOF_MGAUSSIAN_HEADER

#include <vector>
#include <string>
#include "AOF.h"

class AOF_MGaussian: public AOF {
 public:
  AOF_MGaussian(IvPDomain domain) : AOF(domain) {};
  ~AOF_MGaussian() {};
  
public:
  double evalPoint(const std::vector<double>& point) const;
  bool   setParam(const std::string&, const std::string&);

private:
  std::vector<double>  m_xcent;
  std::vector<double>  m_ycent;
  std::vector<double>  m_sigma;
  std::vector<double>  m_range;
};

#endif









