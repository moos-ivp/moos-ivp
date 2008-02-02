/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Quadratic.h                                      */
/*    DATE: Feb 2nd 2008                                         */
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
 
#ifndef AOF_QUADRATIC_HEADER
#define AOF_QUADRATIC_HEADER

#include "AOF.h"

class AOF_Quadratic: public AOF {
public:
  AOF_Quadratic(IvPDomain g_domain);
  ~AOF_Quadratic() {};

public:    
  double evalBox(const IvPBox*) const;
  bool   setParam(const std::string& param, double val); 
  bool   initialize();
  
private:
  double m_coeff;
  double n_coeff;
  double x_center;
  double y_center;
  int    x_index;
  int    y_index;
};

#endif
