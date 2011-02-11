 /*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYFormatUtilsVector.cpp                              */
/*    DATE: Oct 18, 2010 In Waterloo                             */
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

#include <iostream>m
#include <vector>
#include <stdlib.h>
#include "XYFormatUtilsVector.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: string2Vector
//

XYVector string2Vector(string str)
{
  if(strContains(str, "xpos=") && strContains(str, "ypos="))
    return(stringPairs2Vector(str));
  else
    return(stringShort2Vector(str));
}

//---------------------------------------------------------------
// Procedure: stringPairs2Vector
//

XYVector stringPairs2Vector(string str)
{
  cout << "str::::" << str << endl;
  XYVector null_vector;
  XYVector new_vector;

  str = tolower(stripBlankEnds(str));
  vector<string> mvector = parseString(str, ',');
  unsigned int i, vsize = mvector.size();
  
  // Below are the mandatory parameters - check they are set.
  double xpos     = 0;
  double ypos     = 0;
  double mag      = 0;
  double ang      = 0;
  bool   xpos_set = false;
  bool   ypos_set = false;
  bool   mag_set  = false;
  bool   ang_set  = false;

  for(i=0; i<vsize; i++) {
    vector<string> svector = parseString(mvector[i], '=');
    if(svector.size() != 2)
      return(null_vector);
    string param = stripBlankEnds(svector[0]);
    string value = stripBlankEnds(svector[1]);
    double dval  = atof(value.c_str());
    if((param == "xpos") && isNumber(value)) {
      xpos_set = true;
      xpos = dval;
    }
    else if((param == "ypos") && isNumber(value)) {
      ypos_set = true;
      ypos = dval;
    }
    else if((param == "ang") && isNumber(value)) {
      ang_set = true;
      ang = dval;
    }
    else if((param == "mag") && isNumber(value)) {
      mag_set = true;
      mag = dval;
    }
    else if((param == "vertex_size") && isNumber(value))
      new_vector.set_vertex_size(dval);
    else if(param == "label")
      new_vector.set_label(value);
    else if(param == "label_color")
      new_vector.set_label_color(value);
    else if(param == "vertex_color")
      new_vector.set_vertex_color(value);
    else if(param == "source")
      new_vector.set_source(value);
    else if(param == "type")
      new_vector.set_type(value);
    else if(param == "active")
      new_vector.set_active(tolower(value) == "true");
  }

  cout << "  xpos:" << xpos << endl;
  cout << "  ypos:" << ypos << endl;
  cout << "  mag:" << mag << endl;
  cout << "  ang:" << ang << endl;

  if(!xpos_set || !ypos_set || !mag_set || !ang_set)
    return(null_vector);
  
  new_vector.setPosition(xpos, ypos);
  new_vector.setVectorMA(mag, ang);
  
  return(new_vector);
}

//---------------------------------------------------------------
// Procedure: stringShort2Vector
//

XYVector stringShort2Vector(string str)
{
  XYVector null_vector;
  XYVector new_vector;

  // Below are the mandatory parameters - check they are set.
  double xpos     = 0;
  double ypos     = 0;
  double mag      = 0;
  double ang      = 0;
  bool   xpos_set = false;
  bool   ypos_set = false;
  bool   mag_set  = false;
  bool   ang_set  = false;

  str = tolower(stripBlankEnds(str));
  vector<string> mvector = parseString(str, ':');
  unsigned int i, vsize = mvector.size();
  
  for(i=0; i<vsize; i++) {
    mvector[i] = stripBlankEnds(mvector[i]);
    string left = tolower(stripBlankEnds(biteString(mvector[i], ',')));
    string rest = stripBlankEnds(mvector[i]);
    double dval = atof(rest.c_str());

    if(left == "label") 
      new_vector.set_label(rest);
    else if(left == "label_color") 
      new_vector.set_label_color(rest);
    else if(left == "type") 
      new_vector.set_type(rest);
    else if(left == "source") 
      new_vector.set_source(rest);
    else if(left == "time") 
      new_vector.set_time(dval);
    else if(left == "vertex_color") 
      new_vector.set_vertex_color(rest);
    else if(left == "active") 
      new_vector.set_active(tolower(rest)=="true");
    else if((left == "vertex_size") && (dval >= 0))
      new_vector.set_vertex_size(dval);
    else if((left == "xpos") && isNumber(rest)) {
      xpos_set = true;
      xpos = dval;
    }
    else if((left == "ypos") && isNumber(rest)) {
      ypos_set = true;
      ypos = dval;
    }
    else if((left == "mag") && isNumber(rest)) {
      mag_set = true;
      mag = dval;
    }
    else if((left == "ang") && isNumber(rest)) {
      ang_set = true;
      ang = dval;
    }
  }

  if(!xpos_set || !ypos_set || !mag_set || !ang_set)
    return(null_vector);
  
  new_vector.setPosition(xpos, ypos);
  new_vector.setVectorMA(mag, ang);
  
  return(new_vector);
}
