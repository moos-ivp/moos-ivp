/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IvPFunction.cpp                                      */
/*    DATE:                                                      */
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
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "IvPFunction.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

IvPFunction::IvPFunction(PDMap *g_pdmap)
{
  assert(g_pdmap);

  m_pdmap = g_pdmap;
  m_pwt   = 10.0;
}

//-------------------------------------------------------------
// Procedure: Destructor

IvPFunction::~IvPFunction()
{
  if(m_pdmap) 
    delete(m_pdmap);
}

//-------------------------------------------------------------
// Procedure: setPWT
//   Purpose:

void IvPFunction::setPWT(double g_pwt)
{
  if(g_pwt >= 0.0)
    m_pwt = g_pwt;
}

//-------------------------------------------------------------
// Procedure: transDomain

bool IvPFunction::transDomain(IvPDomain gdomain)
{
  if(m_pdmap->getDomain() == gdomain)
    return(true);

  // First measure the sizes of the given domain and the 
  // number of dimensions for this objective function. Ensure
  // the number of dimensions in this function is not greater.
  int dom_dim = gdomain.size();
  int of_dim  = m_pdmap->getDim();
  if(of_dim > dom_dim)
    return(false);

  int i;

  // Now build a translation map. Ensure that all domain names
  // in this function are found in the given IvPDomain.
  bool ok = true;
  int *dmap = new int[of_dim]; 
  for(i=0; i<of_dim; i++) {
    string i_dom = this->getVarName(i);
    dmap[i] = gdomain.getIndex(i_dom);
    if(dmap[i] == -1)
      ok = false;
  }

  // Now perform the domain translation on the pdmap, check
  // the result and quit now if it failed.
  ok = ok && m_pdmap->transDomain(gdomain, dmap);

  // Clean up temp memory from the heap
  delete [] dmap;

  return(ok);
}

//-------------------------------------------------------------
// Procedure: getVarName()

string IvPFunction::getVarName(int i)
{
  return(m_pdmap->getDomain().getVarName(i));
}


//-------------------------------------------------------------
// Procedure: print

#if 0
void IvPFunction::print()
{
  cout << "pdmap is null? " << (pdmap == 0) << endl;
  int dim = pdmap->getDim();
  const IvPBox *univ = pdmap->getUniverse();
  cout << "OF Priority: " << m_pwt << endl;
  //pdmap->print();

  for(int i=0; i<dim; i++) {
    cout << "Domain[" << i << "]: " << dname[i] << "  ";
    cout << univ->pt(i,0);
    cout << ":" << univ->pt(i,1) << endl;
  }
}
#endif









