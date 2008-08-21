/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_AvoidObstacles.h                                 */
/*    DATE: Aug 2nd 2006                                         */
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
 
#ifndef BHV_AVOID_OBSTACLES_HEADER
#define BHV_AVOID_OBSTACLES_HEADER

#include "IvPBehavior.h"
#include "AOF_AvoidObstacles.h"
#include "XYPolygon.h"

class BHV_AvoidObstacles : public IvPBehavior {
public:
  BHV_AvoidObstacles(IvPDomain);
  ~BHV_AvoidObstacles();
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

protected:
  AOF_AvoidObstacles    *m_aof_avoid;

  double  m_allowable_ttc;    // Allowable time to collision
  double  m_activation_dist;  // Outside of which no IPF created
  double  m_buffer_dist;       // Between OS and obstacle(s)
};
#endif




