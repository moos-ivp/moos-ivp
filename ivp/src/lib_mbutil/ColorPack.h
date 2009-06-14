/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ColorPack.h                                          */
/*    DATE: May 28th 2009                                        */
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

#ifndef COLOR_PACK_HEADER
#define COLOR_PACK_HEADER

#include <vector>
#include <string>
#include "MBUtils.h"
#include "ColorParse.h"

class ColorPack
{
public:
  // Constructor #1
  ColorPack() {
    m_color_vector = std::vector<double>(3,0);
    m_set = false;
    m_visible = true;
  };

  // Constructor #2
  ColorPack(const std::string& s) {
    if(tolower(s) == "invisible") {
      m_visible = false;
      m_set = true;
      m_color_vector = std::vector<double>(3,0);
    } 
    else {
      m_color_vector=colorParse(s);
      m_set = true;
      m_visible = true;
      if(m_color_vector.size() != 3) {
	m_color_vector = std::vector<double>(3,0);
	m_set = false;
      }
    }
  };

  // Constructor #3
  ColorPack(std::vector<double> v) {
    m_visible = true;
    m_set = false;
    if(v.size()==3) {
      m_color_vector = v;
      m_set = true;
    }
    else 
      m_color_vector = std::vector<double>(3,0);
  };
  
  // Constructor #4
  ColorPack(double r, double g, double b) {
    m_color_vector.push_back(r);
    m_color_vector.push_back(g);
    m_color_vector.push_back(b);
    m_set = true;
    m_visible = true;
  };

  // Destructor
  ~ColorPack() {};

  // setColor
  void setColor(const std::string& s) {
    if(tolower(s) == "invisible") {
      m_visible = false;
      m_set = true;
      m_color_vector = std::vector<double>(3,0);
    } 
    else if(isColor(s)) {
      m_color_vector=colorParse(s);
      m_set = true;
      m_visible = true;
    }
  };

  // clear
  void clear() {
    m_color_vector = std::vector<double>(3,0);
    m_set = false;
  };

  double red() const {return(m_color_vector[0]);};
  double grn() const {return(m_color_vector[1]);};
  double blu() const {return(m_color_vector[2]);};
  bool   set() const {return(m_set);};
  bool   visible() const {return(m_visible);};

  std::string str() const {
    if(!m_visible)
      return("invisible");
    std::string rstr = doubleToString(m_color_vector[0],3) + ",";
    rstr += doubleToString(m_color_vector[1],3) + ",";
    rstr += doubleToString(m_color_vector[2],3);
    return(rstr);
  }

protected:
  std::vector<double> m_color_vector;
  bool                m_set;
  bool                m_visible;
};
#endif


