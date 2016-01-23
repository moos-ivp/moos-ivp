/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: KCache.h                                             */
/*    DATE: Apr 17th, 2015                                       */
/*****************************************************************/

#ifndef KCACHE_HEADER
#define KCACHE_HEADER

#include <vector>
#include "DHVGrid.h"
#include "Seglr.h"
#include "SeglrUtils.h"

class KCache 
{
 public:
  KCache();
  ~KCache() {};
  
 public: // Setters
  bool   configure(double dh, double dv, double mv);
  bool   setCellSeglrOrig(double spd, double dh, Seglr seglr);
  bool   setCellSeglrNowR(double spd, double dh, Seglr seglr);

 public: // Getters
  Seglr  getSeglrOrig(double spd, double dh) const;
  Seglr  getSeglrCurr(double spd, double dh) const;

  double getHdgGap() const {return(m_grid_seglrs_orig.getHdgGap());};
  double getSpdGap() const {return(m_grid_seglrs_orig.getSpdGap());};
  double getSpdMax() const {return(m_grid_seglrs_orig.getSpdMax());};

 public: // Modifiers
  void   updateCurrSeglrs(double x, double y, double angle);

 public: // Analyzers
  bool  configured() const   {return(m_configured);};
  bool  empty() const;
  void  print() const;

  unsigned int size() const;

 protected: // State variables
  
  DHVGrid<Seglr>    m_grid_seglrs_orig;
  DHVGrid<Seglr>    m_grid_seglrs_nowr;
  DHVGrid<Seglr>    m_grid_seglrs_nowl;

  bool  m_configured;
};

#endif 
