/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: KCache.cpp                                           */
/*    DATE: Apr 17th, 2015                                       */
/*****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "SeglrUtils.h"
#include "KCache.h"
#include "TurnPoly.h"
#include "CurveFitting.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

KCache::KCache()
{
  m_configured = false;
}
  
//----------------------------------------------------------------
// Procedure: configure()

bool KCache::configure(double hdg_gap, double spd_gap, double spd_max)
{
  bool ok1 = m_grid_seglrs_orig.configure(hdg_gap, spd_gap, spd_max);
  bool ok2 = m_grid_seglrs_nowr.configure(hdg_gap, spd_gap, spd_max);
  bool ok3 = m_grid_seglrs_nowl.configure(hdg_gap, spd_gap, spd_max);

  if(!ok1 || !ok2 || !ok3) 
    return(false);

  m_configured = true;
  return(true);
}

//----------------------------------------------------------------
// Procedure: setCellSeglrOrig

bool KCache::setCellSeglrOrig(double spd, double dh, Seglr seglr)
{
  bool ok = m_grid_seglrs_orig.setCell(spd, dh, seglr);

  return(ok);
}

//----------------------------------------------------------------
// Procedure: setCellSeglrNowR

bool KCache::setCellSeglrNowR(double spd, double dh, Seglr seglr)
{
  bool ok = m_grid_seglrs_nowr.setCell(spd, dh, seglr);

  return(ok);
}


//----------------------------------------------------------------
// Procedure: getSeglrOrig

Seglr KCache::getSeglrOrig(double spd, double dh) const
{
  Seglr seglr;

  m_grid_seglrs_orig.getCellVal(spd, dh, seglr);

  return(seglr);
}

//----------------------------------------------------------------
// Procedure: getSeglrCurr

Seglr KCache::getSeglrCurr(double spd, double dh) const
{
  Seglr seglr;

  if(dh > 0)
    m_grid_seglrs_nowr.getCellVal(spd, dh, seglr);
  else
    m_grid_seglrs_nowl.getCellVal(spd, -dh, seglr);

  return(seglr);
}

//----------------------------------------------------------------
// Procedure: print()

void KCache::print() const
{
}

//----------------------------------------------------------------
// Procedure: size()

unsigned int KCache::size() const
{
  return(m_grid_seglrs_orig.cellsSet());
}

//----------------------------------------------------------------
// Procedure: empty()

bool KCache::empty() const
{
  unsigned int cells_set = m_grid_seglrs_orig.cellsSet();
  if(cells_set == 0)
    return(true);
  return(false);
}

//----------------------------------------------------------------
// Procedure: updateCurrSeglrs()

void KCache::updateCurrSeglrs(double x, double y, double angle)
{
#if 0
  m_grid_seglrs_nowr = m_grid_seglrs_orig;
  m_grid_seglrs_nowl = m_grid_seglrs_orig;

  unsigned int rows = m_grid_seglrs_orig.getRows();
  unsigned int cols = m_grid_seglrs_orig.getCols();

  // Part 1: Translate and Rotate the NOWR cache 
  for(unsigned int i=0; i<rows; i++) {
    for(unsigned int j=0; j<cols; j++) { 
      m_grid_seglrs_nowr[i][j].translateTo(x, y);
      m_grid_seglrs_nowr[i][j] = rotateSeglr(m_grid_seglrs_nowr[i][j], angle);
    }
  }
  
  // Part 2: Reflect, Translate and Rotate the NOWL cache.
  for(unsigned int i=0; i<rows; i++) {
    for(unsigned int j=0; j<cols; j++) {
      m_grid_seglrs_nowl[i][j].reflect();
      m_grid_seglrs_nowl[i][j].translateTo(x, y);
      m_grid_seglrs_nowl[i][j] = rotateSeglr(m_grid_seglrs_nowl[i][j], angle);
    }
  }
#endif
}

