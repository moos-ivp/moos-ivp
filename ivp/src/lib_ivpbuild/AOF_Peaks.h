/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_PEAKS.h                                          */
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
 
#ifndef AOF_PEAKS_HEADER
#define AOF_PEAKS_HEADER

#include <vector>
#include <string>
#include "AOF.h"
#include "IvPDomain.h"

class AOF_Peaks: public AOF {
public:
  AOF_Peaks(IvPDomain domain) : AOF(domain) {};
  virtual ~AOF_Peaks() {};
  
public: // virtuals defined
  double evalPoint(const std::vector<double>& point) const;
  bool   setParam(const std::string&, const std::string&);

private:
  std::vector<double>  m_xpos;
  std::vector<double>  m_ypos;
  std::vector<double>  m_gradient_dist;
  std::vector<double>  m_range;
};

#endif









