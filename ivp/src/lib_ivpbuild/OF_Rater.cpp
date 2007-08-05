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
  aof   = g_aof;
  pdmap = g_pdmap;

  sampleCount  = 0;
  totalErr     = 0.0;
  worstErr     = 0.0;
  squaredErr   = 0.0;
  sampHigh     = 0.0;   // Not actual starting value. First
  sampLow      = 0.0;   // sample value will be assigned
  err          = 0;
}

//-------------------------------------------------------------
// Procedure: setPDMap()
//      Note: It is assumed that the pdmap is not owned by this
//            object. Thus the old one is not deleted prior to
//            setting pdmap to the new g_pdmap.

void OF_Rater::setPDMap(const PDMap *g_pdmap)
{
  pdmap = g_pdmap;
  resetSamples();
}

//-------------------------------------------------------------
// Procedure: setAOF()
//      Note: It is assumed that the aof is not owned by this
//            object. Thus the old one is not deleted prior to
//            setting aof to the new g_aof.

void OF_Rater::setAOF(const AOF *g_aof)
{
  aof = g_aof;
  resetSamples();
}

//-------------------------------------------------------------
// Procedure: takeSamples()
// Procedure: resetSamples()

void OF_Rater::takeSamples(int amount, double winterruptVal)
{
  if(!pdmap || !aof || (amount <= 0))
    return;

  double val1, val2, diff, avgErr;
  int    i;

  IvPDomain domain = aof->getDomain();

  double *tempErr = new double[sampleCount];
  for(i=0; i<sampleCount; i++)
    tempErr[i] = err[i];

  if(err) delete(err);
  err = new double[sampleCount+amount];
  for(i=0; i<sampleCount; i++)
    err[i] = tempErr[i];
  delete [] tempErr;

  if(sampleCount == 0) {
    IvPBox rand_box = makeRand(domain);
    worstErr = 0.0;
    totalErr = 0.0;
    squaredErr = 0.0;
    val1 = aof->evalBox(&rand_box);
    sampHigh = val1;
    sampLow  = val1;
  }

  while(amount > 0) {
    IvPBox rand_box = makeRand(domain);
    sampleCount++;
    val1 = aof->evalBox(&rand_box);
    val2 = pdmap->evalPoint(&rand_box);

    diff = (val1-val2);
    if(diff<0) diff = (diff * -1.0);
    err[i]    = diff;
    totalErr += diff;
    squaredErr += diff * diff;
    i++;

    worstErr  = max(worstErr, diff);
    sampHigh  = max(sampHigh, val1);
    sampLow   = min(sampLow, val1);

    if(winterruptVal && (worstErr>winterruptVal)) {
      cout << "    INTERRUPTING TAKE_SAMPLES!! " << amount << endl;
      amount = 0;
    }
    else
      amount--;
  }
  assert(sampleCount > 0);
  avgErr = (double)(totalErr / (double)sampleCount);

}

//------------------------------------------------------------------
// Procedure: resetSamples

void OF_Rater::resetSamples()
{
  if(err) delete(err);
  err         = 0;
  totalErr    = 0;
  worstErr    = 0;
  squaredErr  = 0;
  sampleCount = 0;
}

//------------------------------------------------------------------
// Procedure: getAvgErr()

double OF_Rater::getAvgErr() const
{
  if((sampHigh-sampLow) <= 0) return(0.0);
  double retVal = totalErr / (double)sampleCount;
  //retVal = 100.0 * retVal / (sampHigh-sampLow);
  return(retVal);
}

//------------------------------------------------------------------
// Procedure: getWorstErr() 

double OF_Rater::getWorstErr() const
{
  if((sampHigh-sampLow) <= 0) return(0.0);
  //double retVal = 100.0 * worstErr / (sampHigh-sampLow);
  double retVal = worstErr;
  return(retVal);
}

//------------------------------------------------------------------
// Procedure: getSquaredErr()

double OF_Rater::getSquaredErr() const
{
  return squaredErr / (double)sampleCount;
}








