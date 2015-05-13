/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_AvoidObstacles.h                                 */
/*    DATE: Aug 2nd, 2006                                        */
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
 
#ifndef AOF_AVOID_OBSTACLES_HEADER
#define AOF_AVOID_OBSTACLES_HEADER

#include "AOF.h"
#include "XYPolygon.h"

class IvPDomain;
class AOF_AvoidObstacles: public AOF {
public:
  AOF_AvoidObstacles(IvPDomain);
  ~AOF_AvoidObstacles() {}

public: // virtual functions
  double evalBox(const IvPBox*) const; 
  bool   setParam(const std::string&, double);
  bool   setParam(const std::string&, const std::string&);
  void   addObstacle(const XYPolygon&);
  bool   initialize();
  bool   postInitialize();

  unsigned int obstaclesInRange();
  unsigned int pertObstacleCount();
  unsigned int size() {return(m_obstacles_orig.size());}
  
  bool      ownshipInObstacle(bool=false);
  bool      isObstaclePert(unsigned int ix);
  double    distToObstaclesBuff();
  double    distToObstaclesOrig();
 
  bool      bearingMinMaxToBufferPoly(double& bmin, double& bmax);

  XYPolygon getObstacleOrig(unsigned int ix);
  XYPolygon getObstacleBuff(unsigned int ix);

  std::string getDebugMsg() {return(m_debug_msg);}

 protected: // Initialization Utilities
  bool   ownshipInObstacle(unsigned int ix, bool=false);
  void   bufferBackOff(double osx, double osy);
  void   applyBuffer();

 protected: // Evaluation Utilities
  double evalAuxObstacles(const IvPBox*) const;
  double evalAuxCtrPoints(const IvPBox*) const;

  bool   polyIsSmall(const XYPolygon&, double) const;

 private: // Config variables
  double m_osx;
  double m_osy;
  double m_osh;
  double m_activation_dist;
  double m_allowable_ttc;
  double m_buffer_dist;

  bool   m_osx_set;
  bool   m_osy_set;
  double m_osh_set;
  bool   m_allowable_ttc_set;
  bool   m_activation_dist_set;
  bool   m_buffer_dist_set;

  double m_present_heading_influence;

 private: // State variables
  int    m_crs_ix;  // Index of "course" variable in IvPDomain
  int    m_spd_ix;  // Index of "speed"  variable in IvPDomain
  
  // A vector over the number of obstacles
  std::vector<XYPolygon> m_obstacles_orig;
  std::vector<XYPolygon> m_obstacles_buff;
  std::vector<bool>      m_obstacles_pert;

  std::string            m_debug_msg;
  
  // A vector over 360 (typically) heading values
  std::vector<double>    m_cache_distance;
};

#endif












