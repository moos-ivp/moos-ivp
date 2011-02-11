/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_SCALAR.h                                         */
/*    DATE:                                                      */
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
 
#ifndef AOF_SCALAR_HEADER
#define AOF_SCALAR_HEADER

#include "AOF.h"

class AOF_SCALAR: public AOF {
public:
  AOF_SCALAR(IvPDomain g_domain) : AOF(g_domain) 
    {m_scalar_val = 0;};
  ~AOF_SCALAR() {};

public:    
  double evalBox(const IvPBox*) const {return(m_scalar_val);};
  bool   setParam(const std::string& param, double val) 
    {
      if(param == "scalar") {
	m_scalar_val = val;
	return(true);
      }
      return(false);
    };

private:
  double m_scalar_val;
};

#endif









