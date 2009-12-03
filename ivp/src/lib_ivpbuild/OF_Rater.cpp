/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: OF_Rater.cpp                                         */
/*    DATE: Dec 5th, 2004 (separated from OFR_AOF.cpp            */
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

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "OF_Rater.h"
#include "BuildUtils.h"

#define min(x, y) ((x)<(y)?(x):(y))
#define max(x, y) ((x)>(y)?(x):(y))

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor
//

OF_Rater::OF_Rater(const PDMap *g_pdmap, const AOF *g_aof) 
{
  m_aof   = g_aof;
  m_pdmap = g_pdmap;

  m_sample_count  = 0;
  m_total_err     = 0.0;
  m_worst_err     = 0.0;
  m_squared_err   = 0.0;
  m_samp_high     = 0.0;   // Not actual starting value. First
  m_samp_low      = 0.0;   // sample value will be assigned
  m_err          = 0;

  if(m_aof)
    m_domain = m_aof->getDomain();
}

//-------------------------------------------------------------
// Procedure: setPDMap()
//      Note: It is assumed that the pdmap is not owned by this
//            object. Thus the old one is not deleted prior to
//            setting pdmap to the new g_pdmap.

void OF_Rater::setPDMap(const PDMap *g_pdmap)
{
  m_pdmap = g_pdmap;
  resetSamples();
}

//-------------------------------------------------------------
// Procedure: setAOF()
//      Note: It is assumed that the aof is not owned by this
//            object. Thus the old one is not deleted prior to
//            setting aof to the new g_aof.

void OF_Rater::setAOF(const AOF *g_aof)
{
  m_aof = g_aof;
  resetSamples();
  
  if(m_aof)
    m_domain = m_aof->getDomain();
}

//-------------------------------------------------------------
// Procedure: takeSamples()
// Procedure: resetSamples()

void OF_Rater::takeSamples(int amount, double winterruptVal)
{
  if(!m_pdmap || !m_aof || (amount <= 0))
    return;

  double val1, val2, diff, avgErr;
  int    i;

  IvPDomain domain = m_aof->getDomain();

  double *tempErr = new double[m_sample_count];
  for(i=0; i<m_sample_count; i++)
    tempErr[i] = m_err[i];

  if(m_err) delete(m_err);
  m_err = new double[m_sample_count+amount];
  for(i=0; i<m_sample_count; i++)
    m_err[i] = tempErr[i];
  delete [] tempErr;

  if(m_sample_count == 0) {
    IvPBox rand_box = makeRand(domain);
    m_worst_err = 0.0;
    m_total_err = 0.0;
    m_squared_err = 0.0;
    val1 = this->evalPtBox(&rand_box);
    m_samp_high = val1;
    m_samp_low  = val1;
  }

  while(amount > 0) {
    IvPBox rand_box = makeRand(domain);
    m_sample_count++;
    val1 = this->evalPtBox(&rand_box);
    val2 = m_pdmap->evalPoint(&rand_box);

    diff = (val1-val2);
    if(diff<0) diff = (diff * -1.0);
    m_err[i]       = diff;
    m_total_err   += diff;
    m_squared_err += diff * diff;
    i++;

    m_worst_err  = max(m_worst_err, diff);
    m_samp_high  = max(m_samp_high, val1);
    m_samp_low   = min(m_samp_low, val1);

    if(winterruptVal && (m_worst_err > winterruptVal)) {
      cout << "    INTERRUPTING TAKE_SAMPLES!! " << amount << endl;
      amount = 0;
    }
    else
      amount--;
  }
  assert(m_sample_count > 0);
  avgErr = (double)(m_total_err / (double)m_sample_count);

}

//------------------------------------------------------------------
// Procedure: resetSamples

void OF_Rater::resetSamples()
{
  if(m_err) delete(m_err);
  m_err          = 0;
  m_total_err    = 0;
  m_worst_err    = 0;
  m_squared_err  = 0;
  m_sample_count = 0;
}

//------------------------------------------------------------------
// Procedure: getAvgErr()

double OF_Rater::getAvgErr() const
{
  if((m_samp_high-m_samp_low) <= 0) return(0.0);
  double retVal = m_total_err / (double)m_sample_count;
  //retVal = 100.0 * retVal / (m_samp_high-m_samp_low);
  return(retVal);
}

//------------------------------------------------------------------
// Procedure: getWorstErr() 

double OF_Rater::getWorstErr() const
{
  if((m_samp_high-m_samp_low) <= 0) return(0.0);
  //double retVal = 100.0 * m_worst_err / (m_samp_high-m_samp_low);
  double retVal = m_worst_err;
  return(retVal);
}

//------------------------------------------------------------------
// Procedure: getm_Squared_err()

double OF_Rater::getSquaredErr() const
{
  return m_squared_err / (double)m_sample_count;
}


//-------------------------------------------------------------
// Procedure: evalPtBox()
//   Purpose: Evaluate a point box based.

double OF_Rater::evalPtBox(const IvPBox *gbox)
{
  if(!m_aof) 
    return(0);
  
  unsigned int dim = gbox->getDim();
  if(dim != m_domain.size())
    return(0);
  
  vector<double> pvals;
  for(unsigned int d=0; d<dim; d++)
    pvals.push_back(m_domain.getVal(d, gbox->pt(d)));
  double val = m_aof->evalPoint(pvals);
  if(val == 0)
    return(m_aof->evalBox(gbox));
  return(val);
}


