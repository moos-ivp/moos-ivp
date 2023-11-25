/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: LatLonFormatUtils.cpp                                */
/*    DATE: March 19th 2020                                      */
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

#include <iostream>
#include "LatLonFormatUtils.h"
#include "MBUtils.h"

using namespace std;

//--------------------------------------------------------
// Procedure: latDDtoDDMM()
//      Note: Convert: 39.9054895  (double)
//                 to: 3954.3294   (string)
//                     DDMM.MMMMM
//            54.3294 minutes = 0.9054895 degs * 60 minutes
//      Note: Negative latitudes are conveyed with positive
//            values. 23.82530 ==> 2349.518
//            It is assumed caller will handle appropriately
//            In the case of NMEA msgs, an N/S designator is used

string latDDtoDDMM(double gval)
{
  if((gval > 90) || (gval < -90))
    return("");
  if(gval < 0)
    gval = -gval;
  
  int degs = (int)(gval);

  gval = gval - (double)(degs);
  if(gval < 0)
    gval = -gval;

  double mins = gval * 60;

  string str = intToString(degs);
  if(str.length() == 1)
    str = "0" + str;

  if(mins < 10)
    str += "0";
  
  str += doubleToStringX(mins);

  if(!strContains(str, '.'))
    str += ".00";
  
  return(str);
}


//--------------------------------------------------------
// Procedure: lonDDDtoDDMM()
//      Note: Convert: -116.395443 (double)
//                 to: 11623.7267
//                     DDDMM.MMMM                     
//            23.7267 minutes = 0.395443 degs * 60 minutes

string lonDDDtoDDDMM(double gval)
{
  if((gval > 180) || (gval < -180))
    return("");
  if(gval < 0)
    gval = -gval;

  int degs = (int)(gval);

  gval = gval - (double)(degs);
  if(gval < 0)
    gval = -gval;

  double mins = gval * 60;

  string str = intToString(degs);
  if(str.length() == 1)
    str = "0" + str;
  if(str.length() == 2)
    str = "0" + str;

  if(mins < 10)
    str += "0";
  
  str += doubleToStringX(mins);
  if(!strContains(str, '.'))
    str += ".00";

  return(str);
}

//--------------------------------------------------------
// Procedure: latDDMMtoDD()
//      Note: Convert 4849.518 to 
//                    48.8253
//            
//            0.8253 degs = 49.518 minutes / 60 minutes

double latDDMMtoDD(string sval)
{
  if(sval.length() < 2)  
    return(0);

  string sdegs;
  if(sval[0] != '0')
    sdegs += sval[0];
  sdegs += sval[1];

  string smins = sval.substr(2);

  double degs = atof(sdegs.c_str());
  double mins = atof(smins.c_str());

  degs += (mins / 60);

  return(degs);
}


//--------------------------------------------------------
// Procedure: lonDDDMMtoDDD()
//      Note: Convert 04849.518 to 
//                    48.8253
//            
//            0.8253 degs = 49.518 minutes / 60 minutes

double lonDDDMMtoDDD(string sval)
{
  if(sval.length() < 3)  
    return(0);

  string sdegs;
  if(sval[0] != '0')
    sdegs += sval[0];
  if((sdegs != "") || (sval[1] != '0'))
    sdegs += sval[1];
  sdegs += sval[2];

  string smins = sval.substr(3);

  double degs = atof(sdegs.c_str());
  double mins = atof(smins.c_str());

  degs += (mins / 60);

  return(degs);
}


