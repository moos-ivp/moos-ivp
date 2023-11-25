/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYTextBox.cpp                                        */
/*    DATE: August 31st, 2023                                    */
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

#include <cstdlib>
#include <cstring>
#include "XYTextBox.h"
#include "MBUtils.h"
#include "GeomUtils.h"

using namespace std;

//---------------------------------------------------------------
// Constructor()

XYTextBox::XYTextBox()
{
  clear();
}

//---------------------------------------------------------------
// Constructor()

XYTextBox::XYTextBox(double x, double y, string label)
{
  clear();
  setX(x);
  setY(y);
  set_label(label);
}

//---------------------------------------------------------------
// Procedure: clear()

void XYTextBox::clear()
{
  XYObject::clear();

  m_x = 0; 
  m_y = 0; 
  m_x_set = false; 
  m_y_set = false; 
  m_msgs.clear();
  m_fsize = 10;
}

//---------------------------------------------------------------
// Procedure: setXY()

void XYTextBox::setXY(double x, double y)
{
  m_x = x; 
  m_y = x; 
  m_x_set = true;
  m_y_set = true; 
}

//---------------------------------------------------------------
// Procedure: setFSize()

void XYTextBox::setFSize(int fsize)
{
  if((fsize==9)  || (fsize==10) || (fsize==12) || (fsize==14) ||
     (fsize==16) || (fsize==18) || (fsize==20) || (fsize==24))
    m_fsize = fsize;
}

//---------------------------------------------------------------
// Procedure: setMColor()

void XYTextBox::setMColor(string colorstr)
{
  if(!isColor(colorstr))
    return;
  m_mcolor = colorstr;
}

//---------------------------------------------------------------
// Procedure: setMsg()

void XYTextBox::setMsg(string msg)
{
  m_msgs.clear();
  m_msgs.push_back(msg);
}

//---------------------------------------------------------------
// Procedure: addMsg()

void XYTextBox::addMsg(string msg)
{
  m_msgs.push_back(msg);
}

//---------------------------------------------------------------
// Procedure: applySnap()

void XYTextBox::applySnap(double snapval)
{
  m_x = snapToStep(m_x, snapval);
  m_y = snapToStep(m_y, snapval);
}

//---------------------------------------------------------------
// Procedure: valid()

bool XYTextBox::valid() const
{
  if(!m_x_set || !m_y_set)
    return(false);
  return(true);
}

//---------------------------------------------------------------
// Procedure: getMsg()

string XYTextBox::getMsg(unsigned int ix) const
{
  if(ix >= m_msgs.size())
    return("");
  return(m_msgs[ix]);
}

//---------------------------------------------------------------
// Procedure: get_spec()

string XYTextBox::get_spec(string param) const
{
  string spec;

  spec += "x="  + doubleToStringX(m_x, 2);
  spec += ",y=" + doubleToStringX(m_y, 2);

  for(unsigned int i=0; i<m_msgs.size(); i++)
    spec += ",msg=\"" + m_msgs[i] + "\"";

  spec += ",fsize=" + intToString(m_fsize);

  if(isColor(m_mcolor))
    spec += ",mcolor=" + m_mcolor;
  
  string obj_spec = XYObject::get_spec(param);
  if(obj_spec != "")
    spec += ("," + obj_spec);

  
  return(spec);
}

//---------------------------------------------------------------
// Procedure: get_spec_inactive()
//   Purpose: In cases where we know the point spec is created
//            simply to "erase" a previous point with the same
//            label, just generate a concise spec with a trivial
//            coordinates.

string XYTextBox::get_spec_inactive() const
{
  string spec = "x=0,y=0,msg=null,active=false";
  if(m_label != "")
    spec += ",label=" + m_label; 
  
  return(spec);
}


//-------------------------------------------------------------
// Procedure: stringToTextBox()

XYTextBox stringToTextBox(string str)
{
  XYTextBox null_tbox;
  XYTextBox tbox;

  vector<string> svector = parseStringQ(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    double dval = atof(value.c_str());
    double ival = atoi(value.c_str());
    
    if(param == "x")
      tbox.setX(dval);
    else if(param == "y")
      tbox.setY(dval);
    else if(param == "msg") {
      if(isQuoted(value))
	value = stripQuotes(value);
      tbox.addMsg(value);
    }
    else if(param == "mcolor")
      tbox.setMColor(value);
    else if(param == "fsize")
      tbox.setFSize(ival);
    else if(isNumber(param))
      tbox.setFSize(ival);
    else if(isColor(param))
      tbox.setMColor(param);      
    else {
      if(!tbox.set_param(param, value))
	return(null_tbox);
    }
  }

  if(!tbox.valid())
    return(null_tbox);

  // If there is no text, AND duration was not explicitly specified,
  // it is assumed to mean that this textbox is meant to be "erased",
  // disappear from the screen. By adding a short duration, it also
  // ensures the text can be removed from memory by a consumer, e.g.,
  // pMarineViewer.
  if(!tbox.duration_set() && (tbox.size() == 0))
    tbox.set_duration(1);

  return(tbox);
}

