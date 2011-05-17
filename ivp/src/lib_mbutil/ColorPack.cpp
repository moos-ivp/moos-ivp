/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: ColorPack.cpp                                        */
/*    DATE: May 28th 2009                                        */
/*          May 15th 2011                                        */
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

#include "ColorPack.h"

using namespace std;

//----------------------------------------------------------------
// Constructor #1

ColorPack::ColorPack() 
{
  // By default the color is BLACK (0,0,0)
  m_color_vector = std::vector<double>(3,0);
  m_set = false;
  m_visible = true;
  m_color_string = "black";
}

//----------------------------------------------------------------
// Constructor #2

ColorPack::ColorPack(string str) 
{
  str = tolower(str);

  if(str == "invisible") {
    m_visible = false;
    m_set = true;
    m_color_vector = std::vector<double>(3,0);
    return;
  } 

  if(isColor(str)) {
    m_color_vector=colorParse(str);
    m_set = true;
    m_visible = true;
    m_color_string = str;
    return;
  }

  m_color_vector = std::vector<double>(3,0);
  m_set = false;
}

//----------------------------------------------------------------
// Constructor #3

ColorPack::ColorPack(vector<double> vect) 
{
  m_visible = true;
  m_set = false;
  if(vect.size()==3) {
    m_color_vector = vect;
    m_set = true;
  }
  else 
    m_color_vector = std::vector<double>(3,0);
}


//----------------------------------------------------------------
// Constructor #4

ColorPack::ColorPack(double r, double g, double b) 
{
  m_color_vector.push_back(r);
  m_color_vector.push_back(g);
  m_color_vector.push_back(b);
  m_set = true;
  m_visible = true;
}

//----------------------------------------------------------------
// Procedure: setColor()

void ColorPack::setColor(string str)
{
  str = tolower(str);
  if(str == "invisible") {
    m_visible = false;
    m_set = true;
    m_color_vector = std::vector<double>(3,0);
  } 
  else if(isColor(str)) {
    m_color_vector=colorParse(str);
    m_set = true;
    m_visible = true;
    m_color_string = str;
  }
}

//----------------------------------------------------------------
// Procedure: str(char)

string ColorPack::str(char separator) const
{
  if(!m_visible)
    return("invisible");
  if(m_color_string != "")
    return(m_color_string);
  string rstr = doubleToStringX(m_color_vector[0],3);
  rstr += separator + doubleToStringX(m_color_vector[1],3);
  rstr += separator + doubleToStringX(m_color_vector[2],3);
  return(rstr);
}
