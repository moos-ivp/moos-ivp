/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYWedge.h                                            */
/*    DATE: Sep 10th 2015                                        */
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

#ifndef XY_WEDGE_HEADER
#define XY_WEDGE_HEADER

#include <string>
#include "XYObject.h"

class XYWedge : public XYObject {
public:
  XYWedge();
  virtual ~XYWedge() {}

  void   setX(double);
  void   setY(double);
  bool   setRadLow(double);
  bool   setRadHigh(double);
  void   setLangle(double);
  void   setHangle(double);

  double getX() const       {return(m_x);}
  double getY() const       {return(m_y);}
  double getLangle() const  {return(m_langle);}
  double getHangle() const  {return(m_hangle);}
  double getRadLow() const  {return(m_radlow);}
  double getRadHigh() const {return(m_radhgh);}

  double getMinX() const    {return(m_xmin);}
  double getMaxX() const    {return(m_xmax);}
  double getMinY() const    {return(m_ymin);}
  double getMaxY() const    {return(m_ymax);}
  
  std::vector<double> getPointCache() const {return(m_pt_cache);}

  std::string get_spec();

  bool   isValid() const;
  bool   initialize(double degrees_per_pt=1);

 protected: // Config variables
  double m_x;    
  double m_y;
  double m_langle;
  double m_hangle;
  double m_radlow;
  double m_radhgh;

 protected: // State variables

  bool   m_x_set;
  bool   m_y_set;
  bool   m_langle_set;
  bool   m_hangle_set;
  bool   m_radlow_set;
  bool   m_radhgh_set;

 protected: // State variables (cached values)
  double m_xmin;
  double m_xmax;
  double m_ymin;
  double m_ymax;

  double m_llx;   // low  angle low  radius
  double m_lly; 

  double m_lhx;   // low  angle high radius
  double m_lhy; 

  double m_hlx;   // high angle low  radius
  double m_hly; 

  double m_hhx;   // high angle high radius
  double m_hhy;

  bool   m_initialized;

  std::vector<double> m_pt_cache;
};
#endif


















