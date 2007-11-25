/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
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
#include <string.h>
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
			  double g_high, int g_points)
{
  if(hasDomain(g_name)) return(false);
  if(g_low > g_high)    return(false);
  if(g_points < 1)      return(false);

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

  int    index  = g_domain.getIndex(g_name);
  double g_low  = g_domain.getVarLow(index);
  double g_high = g_domain.getVarHigh(index);
  int    g_pts  = g_domain.getVarPoints(index);

  return(this->addDomain(g_name, g_low, g_high, g_pts));
}

//-------------------------------------------------------------
// Procedure: hasDomain

bool IvPDomain::hasDomain(const string& g_name) const
{
  for(int i=0; (i < m_dname.size()); i++)
    if(m_dname[i] == g_name)
      return(true);
  return(false);
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

void IvPDomain::print()
{
  int dcount = size();
  cout << "Domain Size: " << dcount << endl;
  for(int i=0; i<dcount; i++) {
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
  for(int i=0; (i < m_dname.size()); i++)
    if(m_dname[i] == g_name)
      return(i);
  return(-1);
}


// Build it and they will come
// Share it and it will grow
// Keep it simple and people will use it








