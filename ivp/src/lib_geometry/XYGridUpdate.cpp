/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYGridUpdate.cpp                                     */
/*    DATE: Mar 18th 2022                                        */
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

#include <cmath>
#include <cstdlib>
#include "XYGridUpdate.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Constructor()

XYGridUpdate::XYGridUpdate(string grid_name)
{
  m_grid_name = grid_name;

  // By default the update type is delta
  m_update_type_delta   = true;
  m_update_type_replace = false;
  m_update_type_average = false;
}

//-------------------------------------------------------------
// Procedure: setUpdateTypeDelta()

void XYGridUpdate::setUpdateTypeDelta()
{
  m_update_type_delta   = true;
  m_update_type_replace = false;
  m_update_type_average = false;
}
//-------------------------------------------------------------
// Procedure: setUpdateTypeReplace()

void XYGridUpdate::setUpdateTypeReplace()
{
  m_update_type_delta   = false;
  m_update_type_replace = true;
  m_update_type_average = false;
}


//-------------------------------------------------------------
// Procedure: setUpdateTypeAverage()

void XYGridUpdate::setUpdateTypeAverage()
{
  m_update_type_delta   = false;
  m_update_type_replace = false;
  m_update_type_average = true;
}

//-------------------------------------------------------------
// Procedure: valid()

bool XYGridUpdate::valid() const
{
  if(m_grid_name == "")
    return(false);

  if(size() == 0)
    return(false);

  return(true);
}

//-------------------------------------------------------------
// Procedure: addUpdate()

bool XYGridUpdate::addUpdate(unsigned int ix, string var, double val)
{
  m_cell_ix.push_back(ix);
  m_cell_var.push_back(var);
  m_cell_val.push_back(val);

  return(true);
}

//-------------------------------------------------------------
// Procedure: getCellIX()

unsigned int XYGridUpdate::getCellIX(unsigned int ix) const
{
  if(ix >= m_cell_ix.size())
    return(0);

  return(m_cell_ix[ix]);
}

//-------------------------------------------------------------
// Procedure: getCellVar()

string XYGridUpdate::getCellVar(unsigned int ix) const
{
  if(ix >= m_cell_var.size())
    return("");

  return(m_cell_var[ix]);
}

//-------------------------------------------------------------
// Procedure: getCellVal()

double XYGridUpdate::getCellVal(unsigned int ix) const
{
  if(ix >= m_cell_val.size())
    return(0);

  return(m_cell_val[ix]);
}

//-------------------------------------------------------------
// Procedure: get_spec()
//    Format: label @ delta @ ix,var,val : ix,var,val : ... : ix,var,val 
//   Example: psg @ 23,depth,12 : 23,temp,45 : 47,depth,9 : 47,temp,43

string XYGridUpdate::get_spec() const
{
  string spec;
  if(!valid())
    return("");
  
  spec += m_grid_name + "@";
  if(m_update_type_replace)
    spec += "replace@";
  else if(m_update_type_average)
    spec += "avg@";
  
  for(unsigned int i=0; i<m_cell_ix.size(); i++) {
    if(i!=0)
      spec += ":";
    spec += uintToString(m_cell_ix[i]) + ",";
    if(m_cell_var[i] != "")
      spec += m_cell_var[i] + ",";
    spec += doubleToStringX(m_cell_val[i],4);
  }
  
  return(spec);
}

//-------------------------------------------------------------
// Procedure: stringToGridUpdate()

XYGridUpdate stringToGridUpdate(string str)
{
  XYGridUpdate null_update;
  XYGridUpdate grid_update;

  string grid_name = biteStringX(str, '@');
  grid_update.setGridName(grid_name);

  if(strContains(str, '@')) {
    string update_type = biteStringX(str, '@');
    if(update_type == "delta")
      grid_update.setUpdateTypeDelta();
    else if(update_type == "avg")
      grid_update.setUpdateTypeAverage();
    else if(update_type == "replace")
      grid_update.setUpdateTypeReplace();
    else
      return(null_update);
  }

  while(str != "") {
    string update = biteStringX(str, ':');
    vector<string> svector = parseString(update, ',');
    // In cases where no cell_var specified, assumed to be first
    if(svector.size() == 2) {
      unsigned int ix = atoi(svector[0].c_str());
      double cell_val = atof(svector[1].c_str());
      grid_update.addUpdate(ix, "", cell_val);
    }
    // cell_var is indeed specified
    else if(svector.size() == 3) {
      unsigned int ix = atoi(svector[0].c_str());
      string cell_var = svector[1];
      double cell_val = atof(svector[2].c_str());
      grid_update.addUpdate(ix, cell_var, cell_val);
    }
    else
      return(null_update);
    
  }
  
  if(!grid_update.valid())
    return(null_update);
    
  return(grid_update);
}



