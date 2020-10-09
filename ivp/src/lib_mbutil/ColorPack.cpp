/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ColorPack.cpp                                        */
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
  // Handle special case first
  if((str == "invisible") || (str == "empty") || (str == "off")) {
    m_visible = false;
    m_set = true;
    m_color_vector = std::vector<double>(3,0);
    return;
  } 

  m_color_vector=colorParse(str);

  // Detect if the color provided was a recognized color. Bad colors
  // will return a vector of zeros, which is "black". If we get a 
  // vector of zeros and the named color was not "black" we infer that
  // the given string does not name a known color.
  // Want to avoid isColor() call since this is expensive.
  if((m_color_vector[0] == 0) && 
     (m_color_vector[1] == 0) && 
     (m_color_vector[2] == 0) &&
     tolower(str) != "black") 
    return;

  // Otherwise all is good.
  m_set = true;
  m_visible = true;
  m_color_string = str;
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
  if((str == "invisible") || (str == "empty")) {
    m_visible = false;
    m_set = true;
    m_color_vector = vector<double>(3,0);
    return;
  } 

  vector<double> result_vector = colorParse(str);
  // Detect if the color provided was a recognized color. Bad colors
  // will return a vector of zeros, which is "black". If we get a 
  // vector of zeros and the named color was not "black" we infer that
  // the given string does not name a known color.
  // Want to avoid isColor() call since this is expensive.
  if((result_vector[0] == 0) && 
     (result_vector[1] == 0) && 
     (result_vector[2] == 0) &&
     tolower(str) != "black") 
    return;

  // Otherwise all is good!
  m_color_vector = result_vector;
  m_set = true;
  m_visible = true;
  m_color_string = str;
}

//----------------------------------------------------------------
// Procedure: shade
//  Examples: 0.05 makes things a bit lighter
//            -0.05 makes things a bit darker

void ColorPack::shade(double pct)
{
  unsigned int i, vsize = m_color_vector.size();
  for(i=0; i<vsize; i++) {
    m_color_vector[i] *= (1+pct);
    if(m_color_vector[i] > 1)
      m_color_vector[i] = 1;
    else if(m_color_vector[i] < 0)
      m_color_vector[i] = 0;
  }
}

//----------------------------------------------------------------
// Procedure: moregray
//      Note: Argument range: [0,1]
//  Examples: 0 leaves things alone
//            1 turns the color gray r=0.5, g=0.5, b=0.5

void ColorPack::moregray(double pct)
{
  pct = vclip(pct, 0, 1);
  unsigned int i, vsize = m_color_vector.size();
  for(i=0; i<vsize; i++) {
    double delta = (m_color_vector[i] - 0.5) * pct;
    m_color_vector[i] -= delta;
    if(m_color_vector[i] > 1)  
      m_color_vector[i] = 1;
    else if(m_color_vector[i] < 0)
      m_color_vector[i] = 0;
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









