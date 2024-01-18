/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYArrow.h                                            */
/*    DATE: Jan 23rd 2022 (Support visualization of wind dir)    */
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

#ifndef ARROW_XY_HEADER
#define ARROW_XY_HEADER

#include <string>
#include <vector>
#include "XYObject.h"

class XYArrow : public XYObject {
public:
  XYArrow();
  XYArrow(double x, double y);
  virtual ~XYArrow() {}

  bool   initialize(const std::string&);

  bool   setSuperLength(double super_len);
  void   setBaseCenter(double ctr_x, double ctr_y);
  void   setBaseCenterX(double ctr_x);
  void   setBaseCenterY(double ctr_y);
  bool   setBase(double base_wid, double base_len);
  bool   setBaseWid(double base_wid);
  bool   setBaseLen(double base_len);
  bool   setHead(double head_wid, double head_len);
  bool   setHeadWid(double head_wid);
  bool   setHeadLen(double head_len);
  void   setAngle(double angle);

  bool   resize(double);
  void   modCenterX(double);
  void   modCenterY(double);
  
  void   setPointCache();

  double getCenterX() {return(m_ctr_x);}
  double getCenterY() {return(m_ctr_y);}

  double getHeadCtrX();
  double getHeadCtrY();

  double getMinX();
  double getMaxX();
  double getMinY();
  double getMaxY();
  
  std::vector<double> getBaseVertices();
  std::vector<double> getHeadVertices();

  std::string get_spec(unsigned int vertex_prec=1) const;

  bool set_param(std::string param, std::string value);
  
protected:
  void   init();
  
protected: // Specs
  double m_ctr_x;
  double m_ctr_y;
  double m_base_wid;
  double m_base_len;
  double m_head_wid;
  double m_head_len;
  double m_angle;

 protected: // Cache of drawable points  
  bool   m_cache_set;

  double m_bx1;
  double m_by1;
  double m_bx2;
  double m_by2;
  double m_bx3;
  double m_by3;
  double m_bx4;
  double m_by4;

  double m_hx1;
  double m_hy1;
  double m_hx2;
  double m_hy2;
  double m_hx3;
  double m_hy3;

  double m_xmin;
  double m_xmax;
  double m_ymin;
  double m_ymax;
};

XYArrow stringToArrow(std::string);

#endif



















