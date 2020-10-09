/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYPatternBlock.h                                     */
/*    DATE: April 30th 2009                                      */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#ifndef XY_PATTERN_BLOCK_HEADER
#define XY_PATTERN_BLOCK_HEADER

#include <string>
#include <vector>
#include "XYPoint.h"
#include "XYSegList.h"

class XYPatternBlock {
public:
  XYPatternBlock();
  ~XYPatternBlock() {}

 public: // Interface for setting parameters
  bool  setParam(std::string, std::string);
  bool  setParam(std::string, double);
  void  addIDPoint(const XYPoint&);
  void  setCoreWidth(bool v=true) {m_core_wid=v;}
  void  setAutoDrop(bool v=true) {m_auto_drop=v;}
  
 public:  // Interface for building components
  void  buildCompositeSegList(double osx, double osy);

 public: // Interface for getting information
  double distanceToClosestEntry(double osx, double osy, bool=true);
  double distanceToCrossAxis(double osx, double osy) const;

  std::vector<XYPoint>   getLanePoints() const {return(m_lane_points);}
  std::vector<XYSegList> getLaneSegments() const {return(m_lane_segments);}

  XYSegList getCompositeSegList() {return(m_composite_seglist);}

 protected:
  int   setLanePoints();
  int   setLaneSegments();
  int   setCompositeSegList(double osx, double osy);
  void  idPointCenter(double& x, double& y) const;

 protected: // Parameters set by the caller.
  double  m_blocklen;
  double  m_blockwid;
  double  m_swathwid;
  double  m_angle;

  bool    m_core_wid;
  bool    m_auto_drop;
  
  // The pblock may contain several id points, but at least one.
  std::vector<XYPoint>   m_id_points;

 protected: // Intermediate results
  // First stage is to produce the lane points
  std::vector<XYPoint>   m_lane_points;

  // Next stage is to produce te lane segments
  std::vector<XYSegList> m_lane_segments;

  // Final stage is to produce the single composite seglist
  XYSegList m_composite_seglist;
};

#endif
