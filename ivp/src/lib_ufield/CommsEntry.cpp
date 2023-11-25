/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: CommsEntry.cpp                                       */
/*    DATE: Aug 30th 2022                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include "CommsEntry.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

CommsEntry::CommsEntry(double x, double y, double utc,
		       unsigned int id)
{
  m_x   = x;
  m_y   = y;
  m_utc = utc;
  m_id  = id;
}

//------------------------------------------------------------
// Procedure: getSpec()

string CommsEntry::getSpec() const
{
  string str;
  str += "x=" + doubleToStringX(m_x, 3);
  str += ",y=" + doubleToStringX(m_y, 3);
  str += ",utc=" + doubleToStringX(m_utc, 3);
  str += ",id=" + uintToString(m_id);
  return(str);
}

//------------------------------------------------------------
// Procedure: stringToCommsEntry()

CommsEntry stringToCommsEntry(string str)
{
  CommsEntry null_entry;
  
  string x_str, y_str, utc_str, id_str;
  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    if(param == "x")
      x_str = value;
    else if(param == "y")
      y_str = value;
    else if(param == "utc")
      utc_str = value;
    else if(param == "id")
      id_str = value;
    else
      return(null_entry);
  }

  if(!isNumber(x_str) || !isNumber(y_str) ||
     !isNumber(utc_str) || !isNumber(id_str))
    return(null_entry);

  double x_dbl = atof(x_str.c_str());
  double y_dbl = atof(y_str.c_str());
  double utc_dbl = atof(utc_str.c_str());
  unsigned int id_uint = atoi(id_str.c_str());
  
  CommsEntry entry(x_dbl, y_dbl, utc_dbl, id_uint);
  return(entry);
}
			      
  



