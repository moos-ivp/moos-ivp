/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ColorPack.h                                          */
/*    DATE: May 28th 2009                                        */
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

#ifndef COLOR_PACK_HEADER
#define COLOR_PACK_HEADER

#include <vector>
#include <string>
#include "MBUtils.h"
#include "ColorParse.h"

class ColorPack
{
public:
  // Constructors/Destructor
  ColorPack();
  ColorPack(std::string);
  ColorPack(std::vector<double>);
  ColorPack(double r, double g, double b);
  virtual ~ColorPack() {}

  // setColor
  void setColor(std::string s);

  // clear
  void clear() {
    m_color_vector = std::vector<double>(3,0);
    m_set = false;
  }

  void shade(double pct);
  void moregray(double pct);

  double red() const {return(m_color_vector[0]);}
  double grn() const {return(m_color_vector[1]);}
  double blu() const {return(m_color_vector[2]);}
  bool   set() const {return(m_set);}
  bool   visible() const {return(m_visible);}

  std::string str(char sep=',') const;

protected:
  std::vector<double> m_color_vector;
  bool                m_set;
  bool                m_visible;
  std::string         m_color_string;
};
#endif












