/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_Peaks.cpp                                        */
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

#include <stdlib.h>
#include <math.h>
#include "MBUtils.h"
#include "AOF_Peaks.h"
#include "BuildUtils.h"
#include "IO_Utilities.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: setParam
 
bool AOF_Peaks::setParam(const string& param, const string& val)
{
  m_xpos.push_back(tokDoubleParse(val, "xpos", ',', '='));
  m_ypos.push_back(tokDoubleParse(val, "ypos", ',', '='));
  m_range.push_back(tokDoubleParse(val, "range", ',', '='));
  m_gradient_dist.push_back(tokDoubleParse(val, "gradient", ',', '='));
  return(true);
}

//----------------------------------------------------------------
// Procedure: evalPoint

double AOF_Peaks::evalPoint(const vector<double>& point) const
{
  if(point.size() != 2)
    return(0);
  
  double return_value = 0;

  for(unsigned int i; i<m_xpos.size(); i++) {
    
    double xdist = (point[0] - m_xpos[i]);
    double ydist = (point[1] - m_ypos[i]);

    double distToCent = sqrt((xdist*xdist)+(ydist*ydist));

    
    double fudge =  (sqrt(0.5) - (0.5 * 0.5));
    double ratio = distToCent / m_gradient_dist[i]; 
    
    if(distToCent < (m_gradient_dist[i] / 2.0)) {
      ratio *= ratio;
      ratio = 1 - ratio;
      ratio = (ratio - fudge) / (1.0 - fudge);
    }
    else if(distToCent < m_gradient_dist[i]) {
      ratio = sqrt(ratio);
      ratio = (1 - ratio) + fudge;
      ratio = (ratio - fudge) / (1.0 - fudge);
    }
    else
      ratio = 0;
    
    return_value += (ratio * m_range[i]);
  }

  return(return_value);
}









