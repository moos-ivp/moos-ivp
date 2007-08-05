/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: OF_Reflector.cpp                                     */
/*    DATE: Aug 29th 2005 (derived from OFR_AOF built long ago)  */
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

#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "IvPFunction.h"
#include "PQueue.h"
#include "Regressor.h"
#include "RT_Uniform.h"
#include "RT_Priority.h"
#include "RT_Focus.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor
//     Notes: g_degree=1 makes piecewise linear functions.
//            g_degree=0 makes piecewise constant functions.
//            g_aof is the underlying function to be approximated.

OF_Reflector::OF_Reflector(const AOF *g_aof, int g_degree) 
{
  m_aof         = g_aof;
  m_pdmap       = 0;
  m_regressor   = new Regressor(m_aof, g_degree);

  m_rt_uniform  = new RT_Uniform(m_regressor);
  m_rt_priority = new RT_Priority(m_regressor);
  m_rt_focus    = new RT_Focus(m_regressor);

}

//-------------------------------------------------------------
// Procedure: Destructor
//      Note: the member variable "pdmap" is not freed since it 
//            it is common convention to use the pdmap as the 
//            product of the creation process, typically in a new
//            objective function - see OF_Reflector::pruneToOF()


OF_Reflector::~OF_Reflector()
{
  if(m_pdmap) 
    delete(m_pdmap);
  
  delete(m_regressor);
  delete(m_rt_uniform);
  delete(m_rt_priority);
  delete(m_rt_focus);
}

//-------------------------------------------------------------
// Procedure: extractOF
//   Purpose: Build and return for the caller an IvP objective
//            function built from the pdmap. Once this is done
//            the caller "owns" the PDMap. The reason for this is
//            that the pdmap is assumed to be too expensive to 
//            replicate for certain applications.

IvPFunction *OF_Reflector::extractOF(bool normalize)
{
  // By default, the pdap is normalized before handing over
  if(normalize)
    m_pdmap->normalize(0.0, 100.0);

  // The pdmap now "belongs" to the IvPFunction, given to the caller.
  // The pdmap memory from the heap now belongs to the caller
  IvPFunction *new_ipf = new IvPFunction(m_pdmap);

  m_pdmap = 0;
  return(new_ipf);
}


//-------------------------------------------------------------
// Procedure: createUniform
//   Purpose: Make a uniform IvP function based on the given amt
//            of max uniform boxes desired. First calculates the
//            largest uniform box possible given the domain. 
//            Further boxes will be created based on priority.

int OF_Reflector::createUniform(int max_amt, int qlevels)
{
  clearPDMap();
  if(!m_aof)
    return(0);

  IvPDomain domain = m_aof->getDomain();
  IvPBox unifbox = genUnifBox(domain, max_amt);
  
  m_pdmap = m_rt_uniform->create(&unifbox, &unifbox, qlevels);
  return(m_pdmap->size());
}

//-------------------------------------------------------------
// Procedure: createUniform
//   Purpose: Make a uniform IvP function based on the given box.
//            Each uniform piece will have the same size as the
//            given box.

int OF_Reflector::createUniform(const IvPBox *unifbox, 
				const IvPBox *gridbox, 
				int qlevels)
{
  clearPDMap();
  if(!m_aof)
    return(0);

  m_pdmap = m_rt_uniform->create(unifbox, gridbox, qlevels);
  return(m_pdmap->size());
}

//-------------------------------------------------------------
// Procedure: createPriority

int OF_Reflector::createPriority(int amt, double thresh)
{
  if(!m_pdmap)
    return(0);
  
  if(amt <= 0)
    return(m_pdmap->size());
  
  PQueue *pqueue = m_rt_uniform->getPQueue();
  if(!pqueue)
    return(m_pdmap->size());
  
  m_pdmap = m_rt_priority->create(m_pdmap, pqueue, amt, thresh);
  return(m_pdmap->size());
}

//-------------------------------------------------------------
// Procedure: createFocusRefine

int OF_Reflector::createFocusRefine(IvPBox region, IvPBox resbox)
{
  if(!m_pdmap)
    return(0);
  
  PDMap *new_pdmap = m_rt_focus->create(m_pdmap, region, resbox);
  if(new_pdmap != 0)
    m_pdmap = new_pdmap;
  
  return(m_pdmap->size());
}

//-------------------------------------------------------------
// Procedure: setStrictRange
//   Purpose:

void OF_Reflector::setStrictRange(bool val)
{
  if(m_regressor)
    m_regressor->setStrictRange(val);
}

//-------------------------------------------------------------
// Procedure: clearPDMap()

void OF_Reflector::clearPDMap()
{
  if(m_pdmap) {
    delete(m_pdmap);
    m_pdmap = 0;
  }
}
    







