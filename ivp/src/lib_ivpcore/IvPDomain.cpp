/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IvPDomain                                            */
/*    DATE: May 29 2004 At Indigo cafe in Montreal               */
/*                                                               */
/* (IvP) The interval programming model is a mathematical        */
/* programming model for representing and solving multi-         */
/* objective optimization problems.                              */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#include <iostream>
#include <cstring>
#include "IvPDomain.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: operator==

bool IvPDomain::operator==(const IvPDomain& right) const
{
  if(m_dname.size() != right.size())
    return(false);

  int vsize = m_dname.size();
  for(int i=0; i<vsize; i++) {
    if(m_dname[i] != right.getVarName(i))
      return(false);
    if(m_dlow[i] != right.getVarLow(i))
      return(false);
    if(m_dhigh[i] != right.getVarHigh(i))
      return(false);
    if(m_dpoints[i] != right.getVarPoints(i))
      return(false);
  }
  return(true);
}

//-------------------------------------------------------------
// Procedure: addDomain

bool IvPDomain::addDomain(const string& g_name, double g_low, 
			  double g_high, unsigned int g_points)
{
  if(hasDomain(g_name) || (g_low > g_high) || (g_points < 1))
    return(false);

  // The only way in which one domain point is allowed is if the
  // the high and low value of the domain are equivalent.
  if((g_points == 1) && (g_high != g_low))
    return(false);

  m_dname.push_back(g_name);
  m_dlow.push_back(g_low);
  m_dhigh.push_back(g_high);
  m_dpoints.push_back(g_points);

  double new_delta = 0.0;
  if(g_points > 1)
    new_delta = (g_high-g_low) / (g_points-1.0);
  m_ddelta.push_back(new_delta);
  return(true);
}

//-------------------------------------------------------------
// Procedure: addDomain

bool IvPDomain::addDomain(const IvPDomain& g_domain, 
			  const string& g_name)
{
  // Make sure GIVEN domain does indeed have given domain variable
  if(!g_domain.hasDomain(g_name))
    return(false);

  unsigned int index  = g_domain.getIndex(g_name);
  unsigned int g_pts  = g_domain.getVarPoints(index);
  double g_low  = g_domain.getVarLow(index);
  double g_high = g_domain.getVarHigh(index);

  return(this->addDomain(g_name, g_low, g_high, g_pts));
}

//-------------------------------------------------------------
// Procedure: hasDomain

bool IvPDomain::hasDomain(const string& g_name) const
{
  unsigned int i;
  for(i=0; (i < m_dname.size()); i++)
    if(m_dname[i] == g_name)
      return(true);
  return(false);
}

//-------------------------------------------------------------
// Procedure: hasOnlyDomain
//   Purpose: Return true iff the domain has exactly the one or two
//            domain variables provided as arguments.

bool IvPDomain::hasOnlyDomain(const string& var1, const string& var2) const
{
  bool has_var1 = false;
  bool has_var2 = false;
  bool has_others = false;

  unsigned int i;
  for(i=0; (i < m_dname.size()); i++) {
    if(m_dname[i] == var1)
      has_var1 = true;
    else if(m_dname[i] == var2)
      has_var2 = true;
    else
      has_others = true;
  }

  if(!has_var1)
    return(false);
  if((var2 != "") && !has_var2)
    return(false);
  if(has_others)
    return(false);

  return(true);
}

//-------------------------------------------------------------
// Procedure: clear()

void IvPDomain::clear()
{
  m_dname.clear();
  m_dlow.clear();
  m_dhigh.clear();
  m_ddelta.clear();
  m_dpoints.clear();
}

//-------------------------------------------------------------
// Procedure: print()

void IvPDomain::print() const
{
  unsigned int dcount = size();
  cout << "Domain Size: " << dcount << endl;
  for(unsigned int i=0; i<dcount; i++) {
    cout << getVarName(i);
    cout << ",";
    cout << getVarLow(i);
    cout << ",";
    cout << getVarHigh(i);
    cout << ",";
    cout << getVarPoints(i);
    cout << endl;
  }
}

//-------------------------------------------------------------
// Procedure: getIndex

int IvPDomain::getIndex(const string &g_name) const
{
  for(unsigned int i=0; (i < m_dname.size()); i++) {
    if(m_dname[i] == g_name)
      return(i);
  }
  return(-1);
}


// Build it well and they will come
// Share it and it will grow
// Keep it simple and people will use it



//-------------------------------------------------------------
// Procedure: getTotalPts

double IvPDomain::getTotalPts() const
{
  double total_pts = 1;
  unsigned int i, vsize = m_dname.size();
  for(i=0; i<vsize; i++) 
    total_pts *= (double)(m_dpoints[i]);
  return(total_pts);
}

//-------------------------------------------------------------
// Procedure: getVarPoints()

unsigned int IvPDomain::getVarPoints(unsigned int index) const
{
  unsigned int vsize = m_dname.size();
  if(index < vsize)
    return(m_dpoints[index]);
  else
    return(0);
}

//-------------------------------------------------------------
// Procedure: getVarPoints()

unsigned int IvPDomain::getVarPoints(const string& str) const
{
  int index = getIndex(str);

  if(index < 0)
    return(0);
  if((unsigned int)(index) >= m_dpoints.size())
    return(0);

  return(m_dpoints[index]);
}


//-------------------------------------------------------------
// Procedure: getVarLow

double IvPDomain::getVarLow(const string& str) const
{
  int index = getIndex(str);
  if(index < 0)
    return(0);
  if((unsigned int)(index) >= m_dlow.size())
    return(0);

  return(m_dlow[index]);
}

//-------------------------------------------------------------
// Procedure: getVarHigh

double IvPDomain::getVarHigh(const string& str) const
{
  int index = getIndex(str);
  if(index < 0)
    return(0);
  if((unsigned int)(index) >= m_dhigh.size())
    return(0);

  return(m_dhigh[index]);
}

//-------------------------------------------------------------
// Procedure: getVarDelta

double IvPDomain::getVarDelta(const string& str) const
{
  int index = getIndex(str);
  if(index < 0)
    return(0);
  if((unsigned int)(index) >= m_ddelta.size())
    return(0);
  
  return(m_ddelta[index]);
}

//-------------------------------------------------------------
// Procedure: getDiscreteVal
//   Purpose: Return the discrete index into the domain given 
//            by a double input value. 
//      Note: The snaptype refers to the manner in which float
//            values not exactly on a discrete point in the domain
//            are settled:
//            snaptype = 0: values are truncated down
//            snaptype = 1: values are ceiling'ed up
//            snaptype = 2: values are to the nearest discrete point
//
//  Examples: For IvPDomain: x:0:20:41, y:2:7:11
//            getDiscreteVal(0, 2.1, 0)  --> 4   
//            getDiscreteVal(0, 2.1, 1)  --> 5   
//            getDiscreteVal(0, 2.1, 2)  --> 4   
//
//            getDiscreteVal(1, 2.1, 0)  --> 0   
//            getDiscreteVal(1, 2.1, 1)  --> 1   
//            getDiscreteVal(1, 2.3, 2)  --> 1   


unsigned int IvPDomain::getDiscreteVal(unsigned int index, 
				       double val, int snaptype) const
{
  if((snaptype < 0) || (snaptype > 2))
    return(0);

  if(val <= m_dlow[index])
    return(0);
  else if(val >= m_dhigh[index])
    return(m_dpoints[index]-1);
  else { 
    if(snaptype == 0)
      return((int)((val-m_dlow[index])/m_ddelta[index]));
    else {
      if(snaptype == 2)
	val -= (m_ddelta[index]/2.0);
      double dval = ((val-m_dlow[index])/m_ddelta[index]);
      int    ival = (int)(dval);
      if(dval > ((double)(ival)))
	return(ival + 1);
      else
	return(ival);
      //return((int)(ceil((val-m_dlow[index])/m_ddelta[index])));
    }
  }
}



