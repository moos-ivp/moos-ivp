/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYSeglr.h                                            */
/*    DATE: Apr 27, 2015                                         */
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
 
#ifndef XY_SEGLR_HEADER
#define XY_SEGLR_HEADER

#include <string>
#include "XYObject.h"
#include "XYSegList.h"
#include "Seglr.h"

class XYSeglr : public XYObject {
public:
  XYSeglr(double ray_angle=0);
  XYSeglr(double x0, double y0);
  XYSeglr(double x0, double y0, double ray_angle);
  XYSeglr(double x0, double y0, double x1, double y1);
  virtual ~XYSeglr() {};

public: // Setters
  void addVertex(double x, double y);
  void setVertex(double x, double y, unsigned int index);
  void setRayAngle(double angle);

  void setRayLen(double);
  void setHeadSize(double);
  void setCacheStemCPA(double);
  void setCacheCPA(double);
  void setCacheCPAPoint(XYPoint);
  
public: // Getters (for settable vals)
  double getVX(unsigned int ix) const;
  double getVY(unsigned int ix) const;
  double getRayAngle() const {return(m_ray_angle);}
  double getRayLen() const   {return(m_raylen);}
  double getHeadSize() const {return(m_headsz);}
  double getCacheCPA() const {return(m_cpa);}
  double getCacheStemCPA() const {return(m_cpa_stem);}
  XYPoint getCacheCPAPoint() const {return(m_cpa_pt);}

public:  // Modifiers
  void translateTo(double x, double y);
  void reflect();
  
public: // Getters (analyzers)
  bool   valid() const;
  double getMinX() const;
  double getMaxX() const;
  double getMinY() const;
  double getMaxY() const;
  double getRayBaseX() const;
  double getRayBaseY() const;
  double getAvgX() const {return((getMaxX()-getMinX())/2);};
  double getAvgY() const {return((getMaxY()-getMinY())/2);};

  XYSegList getBaseSegList() const;
  unsigned int size() const {return(m_vx.size());};
  
  std::string get_spec(int vertex_precision=1) const;
  std::string get_spec_ix() const;

protected:
  void init();
  
protected:
  std::vector<double> m_vx;
  std::vector<double> m_vy;
  double              m_ray_angle;

  double m_raylen;  // length of the rendered ray in meters
  double m_headsz;  // Size of the rendered head in meters

  double  m_cpa;      // An optionally cached value
  XYPoint m_cpa_pt;   // An optionally cached value
  double  m_cpa_stem; // An optionally cached value
};

XYSeglr string2Seglr(const std::string&);

#endif





