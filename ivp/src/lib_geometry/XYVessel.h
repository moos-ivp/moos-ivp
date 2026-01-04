/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYVessel.h                                           */
/*    DATE: November 7th 2025                                      */
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

#ifndef XY_VESSEL_HEADER
#define XY_VESSEL_HEADER

#include <string>
#include "XYObject.h"

class XYVessel : public XYObject {
public:
  XYVessel(double x=0, double y=0, double hdg=0, double len=0);
  virtual ~XYVessel() {}

  void   setXY(double, double);
  void   setX(double v)        {setXY(v, m_y);}
  void   setY(double v)        {setXY(m_x, v);}
  void   setLen(double v);
  void   setHdg(double v);
  
  void   modX(double v)        {setX(m_x+v);}
  void   modY(double v)        {setY(m_y+v);}
  void   modLen(double v)      {setLen(m_len+v);}

  double getX() const     {return(m_x);}
  double getY() const     {return(m_y);}
  double getLen() const   {return(m_len);}
  double getHdg() const   {return(m_hdg);}

  std::string get_spec(std::string s="") const;

protected:
  double   m_x;
  double   m_y;
  double   m_len;
  double   m_hdg;
};

XYVessel stringToVessel(std::string);
#endif
