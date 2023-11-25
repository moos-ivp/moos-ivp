/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: Obstacle.h                                           */
/*    DATE: May 29, 2020                                         */
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

#ifndef MANAGED_OBSTACLE_HEADER
#define MANAGED_OBSTACLE_HEADER

#include "XYPolygon.h"
#include "XYPoint.h"
#include <list>
#include <vector>

class Obstacle
{
public:
  Obstacle();
  ~Obstacle() {};

  bool addPoint(XYPoint);
  bool setPoly(XYPolygon);

  bool pruneByAge(double max_time, double curr_time);
  
  void setRange(double);
  void setDuration(double v)     {m_duration=v;}
  void setTStamp(double v)       {m_tstamp=v;}
  void setMaxPts(unsigned int v) {m_max_points=v;}
  void setChanged(bool v=true)   {m_changed=v;}
  void incUpdatesTotal()         {m_updates_total++;}
  
  unsigned int size() const            {return(m_points.size());}
  unsigned int getPtsTotal() const     {return(m_pts_total);}
  XYPolygon    getPoly() const         {return(m_polygon);}
  double       getRange() const        {return(m_range);}
  bool         hasChanged() const      {return(m_changed);}
  double       getDuration() const     {return(m_duration);}
  double       getTStamp() const       {return(m_tstamp);}
  unsigned int getUpdatesTotal() const {return(m_updates_total);}
  unsigned int getMaxPoints() const    {return(m_max_points);}
  double       getMinRange() const     {return(m_min_range);}
  
  double       getTimeToLive(double curr_time) const;

  bool         isGiven() const;

  std::string  getInfo(double curr_time=0) const;
  
  std::vector<XYPoint> getPoints() const;
  
protected: // set externally
  std::list<XYPoint> m_points;
  XYPolygon          m_polygon;
  double             m_range;
  double             m_duration;
  double             m_tstamp;
  unsigned int       m_max_points;
  
private:  // set internally
  unsigned int   m_pts_total;
  bool           m_changed;  
  unsigned int   m_updates_total;
  double         m_min_range;
  std::string    m_poly_spec;
};

#endif 

