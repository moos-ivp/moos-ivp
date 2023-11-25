/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: OF_Rater.h                                           */
/*    DATE: Nov 14, 2000 (separated from RandOF.H)               */
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

#ifndef OF_RATER_HEADER
#define OF_RATER_HEADER

#include "PDMap.h"
#include "AOF.h"

class OF_Rater {
public:
  OF_Rater(const PDMap *pdmap=0, const AOF *aof=0);
  virtual ~OF_Rater() {if(m_err) delete [] m_err;}

public: 
  void    setPDMap(const PDMap*);
  void    setAOF(const AOF*);
  void    takeSamples(int i=1000, double wi=0.0);
  int     getSampleCount()  {return(m_sample_count);}
  double  getSampHigh()     {return(m_samp_high);}
  double  getSampLow()      {return(m_samp_low);}

  void    resetSamples();
  double  getAvgErr() const;
  double  getWorstErr() const;
  double  getSquaredErr() const;

protected:
  double  evalPtBox(const IvPBox*);


protected:
  const AOF*   m_aof;
  const PDMap* m_pdmap;

  IvPDomain    m_domain;
  
  int     m_sample_count;   // Count of samples taken so far
  double  m_total_err;      // Total Err of all samples so far
  double  m_worst_err;      // Worst val of all samples so far
  double  m_squared_err;
  double  m_samp_high;      // Highest value of samples
  double  m_samp_low;       // Lowest  value of samples
  double* m_err;            // Err val of all samples so far
};

#endif















