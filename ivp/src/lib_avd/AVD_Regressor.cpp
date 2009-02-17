/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AVD_Regressor.cpp                                    */
/*    DATE: Feb 15th, 2009                                       */
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

#include <cstdlib>
#include "AVD_Regressor.h"
#include "MBUtils.h"

using namespace std;

//------------------------------------------------------------------
// Constructor

AVD_Regressor::AVD_Regressor()
{
  m_coeff  = 0;
  m_scalar = 0;
}

//------------------------------------------------------------------
// Procedure: addMeasurement

bool AVD_Regressor::addMeasurement(double time, double hdelta)
{
  m_times.push_back(time);
  m_hdeltas.push_back(hdelta);

  return(true);
}

//------------------------------------------------------------------
// Procedure: addMeasurementSet

bool AVD_Regressor::addMeasurementSet(vector<double> times, 
				      vector<double> hdeltas)
{
  unsigned int i, vsize = times.size();
  if(hdeltas.size() != vsize)
    return(false);

  for(i=0; i<vsize; i++) {
    m_times.push_back(times[i]);
    m_hdeltas.push_back(hdeltas[i]);
  }

  return(true);
}

//------------------------------------------------------------------
// Procedure: addMeasurementSet
//   Example: "0.5,1.2: 0.8,2.3: ... :12.2,45.0"
//      Note: If even one of the pairs are not of the right syntax,
//            then none of the pairs are added.

bool AVD_Regressor::addMeasurementSet(string pairs)
{
  vector<double> new_times;
  vector<double> new_hdeltas;

  vector<string> svector = parseString(pairs, ':');
  unsigned int i, vsize = svector.size();
  if(vsize == 0)
    return(false);
  for(i=0; i<vsize; i++) {
    vector<string> ivector = parseString(svector[i], ',');
    unsigned int j, isize = ivector.size();
    if(isize != 2)
      return(false);
    string time   = stripBlankEnds(ivector[0]);
    string hdelta = stripBlankEnds(ivector[1]);
    if(!isNumber(time) || !isNumber(hdelta))
      return(false);
    new_times.push_back(atof(time.c_str()));
    new_hdeltas.push_back(atof(hdelta.c_str()));
  }
  
  return(addMeasurementSet(new_times, new_hdeltas));
}

//------------------------------------------------------------------
// Procedure: clear

void AVD_Regressor::clear()
{
  m_times.clear();
  m_hdeltas.clear();

  m_coeff  = 0;
  m_scalar = 0;
}
