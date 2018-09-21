/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_Ring.cpp                                         */
/*    DATE: Very old - modernized Jan 21 2006                    */
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
#include <cstdlib>
#include <cmath>
#include "AOF_Ring.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "IO_Utilities.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor


AOF_Ring::AOF_Ring(IvPDomain g_domain) : AOF(g_domain)
{
  dim = g_domain.size();
  
  // Set default values
  radius   = 0;       
  radLow   = 0;
  radHigh  = 0;
  exponent = 10;      
  expLow   = 10;
  expHigh  = 10;
  m_range  = 200;     
  m_base   = -100;    
  m_ring_peak = true;    
  plateau  = 0.0;

  gradient_type = 0;
  
  // Calculate default gradient_dist once here so we don't 
  // have to do it each time when we call evalPoint().

  gradient_dist  = 0.0;
  for(unsigned int d=0; d<dim; d++)
    gradient_dist += m_domain.getVarPoints(d);

  m_ring_peak = (rand()%2);  

  m_location = makeRand(m_domain);
}

//----------------------------------------------------------------
// Procedure: setParam
 
bool AOF_Ring::setParam(const string& param, double val)
{
  if(param == "base")
    m_base = val;
  else if(param == "range")
    m_range = (int)(val+0.5);
  else if(param == "radius")
    radius = (int)(val+0.5);
  else if(param == "radlow")
    radLow = (int)(val+0.5);
  else if(param == "radhigh")
    radHigh = (int)(val+0.5);
  else if(param == "exp")
    exponent = (int)(val+0.5);
  else if(param == "explow")
    expLow = (int)(val+0.5);
  else if(param == "exphigh")
    expHigh = (int)(val+0.5);
  else if(param == "plateau")
    plateau = val;
  else if(param == "gradient_dist") {
    if(val > 0)
      gradient_dist = val;
  }
  else
    return(false);
  return(true);
}

//----------------------------------------------------------------
// Procedure: setParam
 
bool AOF_Ring::setParam(const string& param, const string& val)
{
  if(param == "location") {
    if(val == "random") { 
      m_location = makeRand(m_domain);
      
      int shift_x = (int)(m_domain.getVarLow(0));
      int shift_y = (int)(m_domain.getVarLow(1));

      int loc_x = m_location.pt(0,0);
      int loc_y = m_location.pt(1,0);
      m_location.setPTS(0,loc_x+shift_x, loc_x+shift_x);
      m_location.setPTS(1,loc_y+shift_y, loc_y+shift_y);
      return(true);
    }
    else {
      string str = stripBlankEnds(val);
      vector<string> svector = parseString(val, ',');
      if(svector.size() != dim)
	return(false);
      m_location = IvPBox(dim);
      for(unsigned int i=0; i<dim; i++) {
	int pval = atoi(svector[i].c_str());
	m_location.setPTS(i, pval, pval);
      }
    }
  }
  else if(param == "peak") {
    if(val == "true")
      m_ring_peak = true;
    else if(val == "false")
      m_ring_peak = false;
    else
      return(false);
  }
  else if(param == "gradient_type") {
    if(val == "linear")
      gradient_type = 0;
    else if(val == "sigmoid")
      gradient_type = 1;
    else if(val == "exponential")
      gradient_type = 2;
    else
      return(false);
  }
  else if(param == "randomize")
    return(randomize());
  else
    return(false);
  return(true);
}

//----------------------------------------------------------------
// Procedure: randomize
 
bool AOF_Ring::randomize()
{
  // Handle RingPeak
  m_ring_peak = (rand()%2);  

  // Handle Location
  m_location = makeRand(m_domain);

  // Handle Radius
  if(radLow <= 0) 
    radLow = 1;
  if(radHigh <= radLow)
    radius = radLow;
  else
    radius = (rand() % ((radHigh - radLow) + 1)) + radLow;

  // Handle Exponent
  if(expLow <= 0) 
    expLow = 1;
  if(expHigh <= expLow)
    exponent = expLow;
  else
    exponent = (rand() % ((expHigh - expLow) + 1)) + expLow;
  return(true);
}

//----------------------------------------------------------------
// Procedure: evalBox

double AOF_Ring::evalBox(const IvPBox *gbox) const
{
  double ratio  = 0;
  double weight = 0;

  double distToCent = (double)(boxDist(*gbox, m_location));
  double distToRing = distToCent - (double)radius;

  if(distToRing < 1.0) distToRing = -1.0 * distToRing;


  if(gradient_type == 0) {
    double numer = distToRing;
    double denom = gradient_dist - (double)radius;
    
    ratio = 1.0 - (numer / denom);
    if(ratio < 0.0) 
      ratio = 0.0; 

    if(distToRing <= plateau)
      ratio = 1.0;
    else
      ratio = pow(ratio, (double)exponent);
  }
  else if(gradient_type == 1) {
    double val = distToRing;
    double mean    = 0;
    double stdDev  = double(exponent * exponent);
    double e       = 2.718281;
    double v2a     = -1.0 / (2*stdDev);
    double v2b     = (val-mean) * (val-mean);
    double v2      = v2a * v2b;
    ratio          = pow(e,v2);
  }
  else if(gradient_type == 2) {

    double fudge =  (sqrt(0.5) - (0.5 * 0.5));
    ratio = distToRing / gradient_dist; 

    if(distToRing < (gradient_dist/2.0)) {
      ratio *= ratio;
      ratio = 1 - ratio;
      ratio = (ratio - fudge) / (1.0 - fudge);
    }
    else if(distToRing < gradient_dist) {
      ratio = sqrt(ratio);
      ratio = (1 - ratio) + fudge;
      ratio = (ratio - fudge) / (1.0 - fudge);
    }
    else
      ratio = 0;
  }

  double pctOfRange = ratio * m_range;

  if(m_ring_peak) 
    weight =  m_base + pctOfRange;
  else 
    weight = (m_base + m_range) - pctOfRange;
  return(weight);
}

//----------------------------------------------------------------
// Procedure: latexSTR

string AOF_Ring::latexSTR(int full) const
{
  string retstr;

  if(full) retstr += "\\fbox{ \\LARGE \\begin{tabular}{ll} $f(x, y)=$";

  int d = m_location.getDim();
  if(d>2) {
    retstr += "Unavailable for 3+ dimensions";
    return(retstr);
  }

  int       h = m_location.pt(0, 0);
  int       k = m_location.pt(1, 0);
  
  retstr     += " & $ (((1-{\\frac {(\\sqrt{(x-";
  retstr     += intToString(h);
  retstr     += ")^2 + (y-";
  retstr     += intToString(k);
  retstr     += ")^2 }-";
  retstr     += intToString(radius);
  retstr     += ")}{";
  retstr     += doubleToString(gradient_dist);
  retstr     += "}})^{";
  retstr     += intToString(exponent);
  retstr     += "} *";
  retstr     += doubleToString(m_range);
  retstr     += ") +";
  retstr     += doubleToString(m_base);
  retstr     += ") +$ \\\\ \\\\";
  if(full) retstr += " \\end{tabular}} \\normalsize";
  return(retstr);
}

//----------------------------------------------------------------
// Procedure: print

void AOF_Ring::print() const
{
  cout << "Location:  ";  m_location.print();
  cout << "  Radius:   " << radius;
  cout << "  exponent: " << exponent;
  cout << "  radius:   " << radius  << endl;
  cout << "  gradient: " << gradient_type  << endl;
}

//---------------------------------------------------------------
// Procdudure: boxDist
//    Purpose: Determine distance between the two given boxes
//      Notes: The distance is the distance between closest edges

double AOF_Ring::boxDist(const IvPBox& box1, const IvPBox& box2) const
{
  double tally = 0.0;

  for(unsigned int i=0; i<dim; i++) {
    double ival = box1.pt(i,0) - box2.pt(i,0);
    tally += ival * ival;
  }
  return(sqrt(tally));
}

















